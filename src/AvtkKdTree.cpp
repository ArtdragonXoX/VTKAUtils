#include "AvtkKdTree.h"
#include "vtkObjectFactory.h"
#include "vtkKdNode.h"

vtkStandardNewMacro(AvtkKdTree);

std::vector<CubeFrame *> AvtkKdTree::GetRegionsBoundariesByLevel(int level)
{
    std::vector<CubeFrame *> frames;

    // 获取指定层级的所有节点
    int numRegions = this->GetNumberOfRegionsAtLevel(level);
    vtkKdNode **nodes = new vtkKdNode *[numRegions];
    this->GetRegionsAtLevel(level, nodes);

    // 遍历每个节点，获取边界并创建对应的CubeFrame
    for (int i = 0; i < numRegions; ++i)
    {
        double bounds[6];
        nodes[i]->GetBounds(bounds);
        CubeFrame *frame = new CubeFrame(bounds);
        frames.push_back(frame);
    }

    // 释放节点数组的内存
    delete[] nodes;
    return frames;
}

int AvtkKdTree::GetNumberOfRegionsAtLevel(int level)
{
    if (level < 0)
        return 0;
    // 处理层级小于0的情况，直接返回0

    auto maxLevel = this->GetMaxLevel();
    if (level > maxLevel)
        return maxLevel;
    // 如果层级超过最大层级，返回最大层级值

    return CountNodesAtLevel(this->Top, 0, level);
    // 调用递归或迭代方法统计指定层级的节点数量
}

int AvtkKdTree::GetMaxLevel() const
{
    return CalculateMaxLevel(this->Top, 0);
}

int AvtkKdTree::CalculateMaxLevel(vtkKdNode *node, int currentLevel) const
{
    if (!node)
        return -1; // 空节点返回无效层级

    // 递归计算左右子节点的最大层级
    int leftMax = CalculateMaxLevel(node->GetLeft(), currentLevel + 1);
    int rightMax = CalculateMaxLevel(node->GetRight(), currentLevel + 1);

    // 比较左右子树的最大层级与当前层级，返回较大值
    return std::max(leftMax, rightMax) > currentLevel ? std::max(leftMax, rightMax) : currentLevel;
}

std::vector<CubeFrame *> AvtkKdTree::GetRegionBoundsByPoint(double x, double y, double z)
{
    int regionID = this->GetRegionContainingPoint(x, y, z);
    if (regionID < 0)
        return {};

    vtkKdNode *targetNode = this->RegionList[regionID];
    if (!targetNode)
        return {};

    /**
     * 获取从根节点到目标节点的完整路径
     */
    std::vector<vtkKdNode *> path = this->GetPathFromRootToNode(targetNode);
    if (path.empty())
        return {};

    /**
     * 遍历路径中的每个节点，收集其边界框信息
     */
    std::vector<CubeFrame *> frames;
    for (auto node : path)
    {
        double bounds[6];
        node->GetBounds(bounds);
        frames.push_back(new CubeFrame(bounds));
    }
    return frames;
}

void AvtkKdTree::FindPointsInArea(double *area, vtkIdList *ids)
{
    vtkNew<vtkIdTypeArray> idList;
    this->vtkKdTree::FindPointsInArea(area, idList);
    AUtils::IdTypeArrayToIdList(idList, ids);
}

void AvtkKdTree::FindPointsInCuboid(double cuboid[8][3], vtkIdList *ids)
{
    // 计算正轴包围盒
    double bounds[6] = {DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX};
    for (int i = 0; i < 8; ++i)
    {
        bounds[0] = std::min(bounds[0], cuboid[i][0]); // xmin
        bounds[1] = std::max(bounds[1], cuboid[i][0]); // xmax
        bounds[2] = std::min(bounds[2], cuboid[i][1]); // ymin
        bounds[3] = std::max(bounds[3], cuboid[i][1]); // ymax
        bounds[4] = std::min(bounds[4], cuboid[i][2]); // zmin
        bounds[5] = std::max(bounds[5], cuboid[i][2]); // zmax
    }

    // 使用KD树快速查找包围盒内的候选点
    vtkNew<vtkIdList> candidateIds;
    this->FindPointsInArea(bounds, candidateIds);

    // 如果没有候选点，直接返回
    if (candidateIds->GetNumberOfIds() == 0)
    {
        return;
    }

    // 获取所有点的数据集
    vtkDataSet *dataSet = this->GetDataSet();
    vtkPointSet *pointSet = vtkPointSet::SafeDownCast(dataSet);
    if (!pointSet)
        return;
    vtkPoints *points = pointSet->GetPoints();

    // 计算立方体的6个面，每个面由4个点定义
    // 面的定义参考AUtils::cubeEdges的顺序
    // 底面: 0,1,3,2
    // 顶面: 4,5,7,6
    // 侧面1: 0,1,5,4
    // 侧面2: 1,3,7,5
    // 侧面3: 3,2,6,7
    // 侧面4: 2,0,4,6
    int faces[6][4] = {
        {0, 1, 3, 2}, // 底面
        {4, 5, 7, 6}, // 顶面
        {0, 1, 5, 4}, // 侧面1
        {1, 3, 7, 5}, // 侧面2
        {3, 2, 6, 7}, // 侧面3
        {2, 0, 4, 6}  // 侧面4
    };

    // 计算每个面的法向量和平面方程
    double normals[6][3];
    double d[6];

    for (int i = 0; i < 6; ++i)
    {
        // 计算面的两个向量
        double v1[3], v2[3];
        for (int j = 0; j < 3; ++j)
        {
            v1[j] = cuboid[faces[i][1]][j] - cuboid[faces[i][0]][j];
            v2[j] = cuboid[faces[i][2]][j] - cuboid[faces[i][1]][j];
        }

        // 计算法向量 (v1 x v2)
        normals[i][0] = v1[1] * v2[2] - v1[2] * v2[1];
        normals[i][1] = v1[2] * v2[0] - v1[0] * v2[2];
        normals[i][2] = v1[0] * v2[1] - v1[1] * v2[0];

        // 归一化法向量
        double length = sqrt(normals[i][0] * normals[i][0] +
                             normals[i][1] * normals[i][1] +
                             normals[i][2] * normals[i][2]);
        if (length > 0)
        {
            normals[i][0] /= length;
            normals[i][1] /= length;
            normals[i][2] /= length;
        }

        // 计算平面方程的常数项 d (ax + by + cz + d = 0)
        d[i] = -(normals[i][0] * cuboid[faces[i][0]][0] +
                 normals[i][1] * cuboid[faces[i][0]][1] +
                 normals[i][2] * cuboid[faces[i][0]][2]);

        // 确保法向量指向立方体外部
        // 计算立方体中心点
        double center[3] = {0, 0, 0};
        for (int j = 0; j < 8; ++j)
        {
            center[0] += cuboid[j][0];
            center[1] += cuboid[j][1];
            center[2] += cuboid[j][2];
        }
        center[0] /= 8.0;
        center[1] /= 8.0;
        center[2] /= 8.0;

        // 检查法向量方向
        double dot = normals[i][0] * (center[0] - cuboid[faces[i][0]][0]) +
                     normals[i][1] * (center[1] - cuboid[faces[i][0]][1]) +
                     normals[i][2] * (center[2] - cuboid[faces[i][0]][2]);

        // 如果法向量指向内部，则反转
        if (dot > 0)
        {
            normals[i][0] = -normals[i][0];
            normals[i][1] = -normals[i][1];
            normals[i][2] = -normals[i][2];
            d[i] = -d[i];
        }
    }

    // 遍历所有候选点，检查是否在立方体内部
    ids->Reset();
    for (vtkIdType i = 0; i < candidateIds->GetNumberOfIds(); ++i)
    {
        vtkIdType pointId = candidateIds->GetId(i);
        double point[3];
        points->GetPoint(pointId, point);

        // 检查点是否在所有平面的内侧
        bool inside = true;
        for (int j = 0; j < 6; ++j)
        {
            // 计算点到平面的距离
            double distance = normals[j][0] * point[0] +
                              normals[j][1] * point[1] +
                              normals[j][2] * point[2] + d[j];

            // 如果距离为正，则点在平面外部
            if (distance > 0)
            {
                inside = false;
                break;
            }
        }

        // 如果点在所有平面内侧，则添加到结果中
        if (inside)
        {
            ids->InsertNextId(pointId);
        }
    }
}

std::vector<vtkKdNode *> AvtkKdTree::GetPathFromRootToNode(vtkKdNode *target) const
{
    return this->getPath(this->Top, target);
}

std::vector<vtkKdNode *> AvtkKdTree::getPath(vtkKdNode *current, vtkKdNode *target) const
{
    // 处理空节点情况，直接返回空路径
    if (!current)
        return {};

    // 当前节点即为目标节点，返回单节点路径
    if (current == target)
        return {current};

    // 递归查找左子树路径
    auto leftPath = this->getPath(current->GetLeft(), target);
    if (!leftPath.empty())
    {
        // 将当前节点插入左子路径开头并返回
        leftPath.insert(leftPath.begin(), current);
        return leftPath;
    }

    // 递归查找右子树路径
    auto rightPath = this->getPath(current->GetRight(), target);
    if (!rightPath.empty())
    {
        // 将当前节点插入右子路径开头并返回
        rightPath.insert(rightPath.begin(), current);
        return rightPath;
    }

    // 双子树均未找到路径，返回空列表
    return {};
}

int AvtkKdTree::CountNodesAtLevel(vtkKdNode *node, int currentLevel, int targetLevel)
{
    /* 如果节点为空，返回0 */
    if (!node)
        return 0;

    /* 如果当前层级超过目标层级，提前返回0 */
    if (currentLevel > targetLevel)
        return 0;

    /* 根据当前层级是否为目标层级返回计数结果或递归统计左右子节点 */
    return (currentLevel == targetLevel) ? 1
                                         : CountNodesAtLevel(node->GetLeft(), currentLevel + 1, targetLevel) + CountNodesAtLevel(node->GetRight(), currentLevel + 1, targetLevel);
}

int AvtkKdTree::GetNodeLevel(vtkKdNode *node) const
{
    return FindNodeLevel(this->Top, node, 0);
}

int AvtkKdTree::FindNodeLevel(
    vtkKdNode *current, vtkKdNode *target, int currentLevel) const
{
    if (!current)
        return -1;
    if (current == target)
        return currentLevel;
    // 递归搜索左子节点层级
    int left = FindNodeLevel(current->GetLeft(), target, currentLevel + 1);
    if (left != -1)
        return left;
    // 若左子树未找到，则递归搜索右子节点层级
    return FindNodeLevel(current->GetRight(), target, currentLevel + 1);
}
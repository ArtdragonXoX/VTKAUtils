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
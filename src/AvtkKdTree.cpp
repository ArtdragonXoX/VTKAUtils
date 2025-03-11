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

    for (int i = 0; i < numRegions; ++i)
    {
        double bounds[6];
        nodes[i]->GetBounds(bounds);
        CubeFrame *frame = new CubeFrame(bounds);
        frames.push_back(frame);
    }

    delete[] nodes;
    return frames;
}

int AvtkKdTree::GetNumberOfRegionsAtLevel(int level)
{
    if (level < 0)
        return 0;
    auto maxLevel = this->GetMaxLevel();
    if (level > maxLevel)
        return maxLevel;
    return CountNodesAtLevel(this->Top, 0, level);
}

int AvtkKdTree::GetMaxLevel() const
{
    return CalculateMaxLevel(this->Top, 0);
}

int AvtkKdTree::CalculateMaxLevel(vtkKdNode *node, int currentLevel) const
{
    if (!node)
        return -1; // 空节点返回无效层级
    int leftMax = CalculateMaxLevel(node->GetLeft(), currentLevel + 1);
    int rightMax = CalculateMaxLevel(node->GetRight(), currentLevel + 1);
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

    // 获取路径
    std::vector<vtkKdNode *> path = this->GetPathFromRootToNode(targetNode);
    if (path.empty())
        return {};

    // 收集边界框
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
    if (!current)
        return {};
    if (current == target)
        return {current};
    auto leftPath = this->getPath(current->GetLeft(), target);
    if (!leftPath.empty())
    {
        leftPath.insert(leftPath.begin(), current);
        return leftPath;
    }
    auto rightPath = this->getPath(current->GetRight(), target);
    if (!rightPath.empty())
    {
        rightPath.insert(rightPath.begin(), current);
        return rightPath;
    }
    return {};
}
int AvtkKdTree::CountNodesAtLevel(vtkKdNode *node, int currentLevel, int targetLevel)
{
    if (!node)
        return 0;
    if (currentLevel > targetLevel)
        return 0;
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
    int left = FindNodeLevel(current->GetLeft(), target, currentLevel + 1);
    if (left != -1)
        return left;
    return FindNodeLevel(current->GetRight(), target, currentLevel + 1);
}
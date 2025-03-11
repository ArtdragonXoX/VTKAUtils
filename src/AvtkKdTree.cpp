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

int AvtkKdTree::CountNodesAtLevel(vtkKdNode *node, int currentLevel, int targetLevel)
{
    if (!node)
        return 0;
    if (currentLevel > targetLevel)
        return 0;
    return (currentLevel == targetLevel) ? 1
                                         : CountNodesAtLevel(node->GetLeft(), currentLevel + 1, targetLevel) + CountNodesAtLevel(node->GetRight(), currentLevel + 1, targetLevel);
}
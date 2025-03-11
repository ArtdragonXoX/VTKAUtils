#pragma once

#include <vtkKdTree.h>
#include <vtkKdNode.h>
#include "CubeFrame.h"

class AvtkKdTree : public vtkKdTree
{
public:
    vtkTypeMacro(AvtkKdTree, vtkKdTree);
    static AvtkKdTree *New();

    std::vector<CubeFrame *> GetRegionsBoundariesByLevel(int level);

    int GetNumberOfRegionsAtLevel(int level);

    int GetMaxLevel() const;

    int CalculateMaxLevel(vtkKdNode *node, int currentLevel) const;

    std::vector<CubeFrame *> GetRegionBoundsByPoint(double x, double y, double z);

protected:
    int CountNodesAtLevel(vtkKdNode *, int, int);
    int GetNodeLevel(vtkKdNode *node) const;
    int FindNodeLevel(vtkKdNode *, vtkKdNode *, int) const;

    std::vector<vtkKdNode *> GetPathFromRootToNode(vtkKdNode *target) const;
    std::vector<vtkKdNode *> getPath(vtkKdNode *current, vtkKdNode *target) const;
};

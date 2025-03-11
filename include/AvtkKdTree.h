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

protected:
    int CountNodesAtLevel(vtkKdNode *, int, int);
};

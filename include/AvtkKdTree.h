#pragma once

#include <vtkKdTree.h>

class AvtkKdTree : public vtkKdTree
{
public:
    vtkTypeMacro(AvtkKdTree, vtkKdTree);
    static AvtkKdTree* New();

protected:
};

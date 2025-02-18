#pragma once

#include "VisualizationPipeline.h"
#include "AUtils.h"

class CubeFrame
{
public:
    PipelineMacro

    CubeFrame();
    CubeFrame(const double bounds[6]);

    void SetBounds(const double bounds[6]);
    void GetBounds(double *bounds) const;

    void Update();

private:
    double bounds[6];
    vtkSmartPointer<vtkPoints> points;
    vtkSmartPointer<vtkCellArray> lines;
    vtkSmartPointer<vtkPolyData> polyData;
};
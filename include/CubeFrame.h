#pragma once

#include "VisualizationPipeline.h"
#include "AUtils.h"

class CubeFrame
{
public:
    PipelineMacro(pipline)

        CubeFrame();
    CubeFrame(const double bounds[6]);
    CubeFrame(const double points[8][3]); // 通过8个顶点构造长方体

    void SetBounds(const double bounds[6]); // 通过包围盒设置顶点
    void GetBounds(double *bounds) const;   // 从顶点计算包围盒
    void SetOBB(double corner[3], double axes[3][3], double size[3]);
    void SetPoints(const double points[8][3]); // 设置8个顶点
    void GetPoints(double *points[3]);
    void SetLineWidth(double width);

    void Update();

private:
    void BoundsToPoints(const double bounds[6]); // 从包围盒计算顶点

    void OBBToPoints(double corner[3], double axes[3][3], double size[3], double points[8][3]);

    std::unique_ptr<VisualizationPipeline> pipline = std::make_unique<VisualizationPipeline>();
    double cuboidPoints[8][3]; // 存储长方体的8个顶点
    vtkSmartPointer<vtkPoints> points;
    vtkSmartPointer<vtkCellArray> lines;
    vtkSmartPointer<vtkPolyData> polyData;
};
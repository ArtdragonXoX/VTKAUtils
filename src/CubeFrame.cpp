#include "CubeFrame.h"

CubeFrame::CubeFrame()
{
    points = vtkSmartPointer<vtkPoints>::New();
    lines = vtkSmartPointer<vtkCellArray>::New();
    polyData = vtkSmartPointer<vtkPolyData>::New();
    visualizationPipeline->SetInput(polyData);
}

CubeFrame::CubeFrame(const double bounds[6]) : CubeFrame()
{
    SetBounds(bounds);
}

void CubeFrame::SetBounds(const double bounds[6])
{
    for (int i = 0; i < 6; ++i)
    {
        this->bounds[i] = bounds[i];
    }
    Update();
}

void CubeFrame::GetBounds(double *bounds) const
{
    for (int i = 0; i < 6; ++i)
    {
        bounds[i] = this->bounds[i];
    }
}

void CubeFrame::Update()
{
    const double &xmin = bounds[0];
    const double &xmax = bounds[1];
    const double &ymin = bounds[2];
    const double &ymax = bounds[3];
    const double &zmin = bounds[4];
    const double &zmax = bounds[5];

    // 生成立方体的8个顶点
    points->InsertNextPoint(xmin, ymin, zmin); // 点0
    points->InsertNextPoint(xmax, ymin, zmin); // 点1
    points->InsertNextPoint(xmax, ymax, zmin); // 点2
    points->InsertNextPoint(xmin, ymax, zmin); // 点3
    points->InsertNextPoint(xmin, ymin, zmax); // 点4
    points->InsertNextPoint(xmax, ymin, zmax); // 点5
    points->InsertNextPoint(xmax, ymax, zmax); // 点6
    points->InsertNextPoint(xmin, ymax, zmax); // 点7

    // 添加所有边
    for (int i = 0; i < 12; ++i)
    {
        lines->InsertNextCell(2);
        lines->InsertCellPoint(AUtils::cubeEdges[i][0]);
        lines->InsertCellPoint(AUtils::cubeEdges[i][1]);
    }

    // 构建PolyData
    polyData->SetPoints(points);
    polyData->SetLines(lines);
}
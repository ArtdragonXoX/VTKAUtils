#include "CubeFrame.h"

CubeFrame::CubeFrame()
{
    points = vtkSmartPointer<vtkPoints>::New();
    lines = vtkSmartPointer<vtkCellArray>::New();
    polyData = vtkSmartPointer<vtkPolyData>::New();
    pipline->SetInput(polyData);
}

CubeFrame::CubeFrame(const double bounds[6]) : CubeFrame()
{
    BoundsToPoints(bounds);
    Update();
}

CubeFrame::CubeFrame(const double points[8][3]) : CubeFrame()
{
    SetPoints(points);
}

void CubeFrame::SetPoints(const double points[8][3])
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            cuboidPoints[i][j] = points[i][j];
        }
    }
    Update();
}

void CubeFrame::GetPoints(double *points[3])
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            points[i][j] = cuboidPoints[i][j];
        }
    }
}

void CubeFrame::SetBounds(const double bounds[6])
{
    BoundsToPoints(bounds);
    Update();
}

void CubeFrame::GetBounds(double *bounds) const
{
    double xmin = cuboidPoints[0][0], xmax = cuboidPoints[0][0];
    double ymin = cuboidPoints[0][1], ymax = cuboidPoints[0][1];
    double zmin = cuboidPoints[0][2], zmax = cuboidPoints[0][2];

    for (int i = 1; i < 8; ++i)
    {
        xmin = std::min(xmin, cuboidPoints[i][0]);
        xmax = std::max(xmax, cuboidPoints[i][0]);
        ymin = std::min(ymin, cuboidPoints[i][1]);
        ymax = std::max(ymax, cuboidPoints[i][1]);
        zmin = std::min(zmin, cuboidPoints[i][2]);
        zmax = std::max(zmax, cuboidPoints[i][2]);
    }

    bounds[0] = xmin;
    bounds[1] = xmax;
    bounds[2] = ymin;
    bounds[3] = ymax;
    bounds[4] = zmin;
    bounds[5] = zmax;
}

void CubeFrame::SetOBB(double corner[3], double axes[3][3], double size[3])
{
    OBBToPoints(corner, axes, size, cuboidPoints);
    Update();
}

void CubeFrame::SetLineWidth(double width)
{
    pipline->GetActor()->GetProperty()->SetLineWidth(width);
}

void CubeFrame::Update()
{
    // 清空points
    points->Reset();

    // 使用存储的顶点更新
    for (int i = 0; i < 8; ++i)
    {
        points->InsertNextPoint(cuboidPoints[i]);
    }

    // 清空lines
    lines->Reset();
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

void CubeFrame::BoundsToPoints(const double bounds[6])
{
    // 使用AUtils::cubeIndices来设置8个顶点
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            cuboidPoints[i][j] = bounds[AUtils::cubeIndices[i][j]];
        }
    }
}

void CubeFrame::OBBToPoints(double corner[3], double axes[3][3], double size[3], double points[8][3])
{
    // 计算OBB的8个顶点
    // 首先计算每个顶点相对于角点的偏移量
    for (int i = 0; i < 8; ++i)
    {
        // 根据AUtils::cubeIndices确定每个顶点在三个轴上的方向
        for (int j = 0; j < 3; ++j)
        {
            // 初始化为角点坐标
            points[i][j] = corner[j];
            if (AUtils::cubeIndices[i][j] % 2)
            {
                for (int k = 0; k < 3; ++k)
                {
                    // 加上相应的轴方向和大小
                    points[i][j] += axes[j][k];
                }
            }
        }
    }
}
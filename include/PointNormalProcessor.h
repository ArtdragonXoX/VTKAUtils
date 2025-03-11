#pragma once
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkPolyDataNormals.h>
#include <AvtkKdTreePointLocator.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkAlgorithmOutput.h>
#include <unordered_set>
#include <cmath>
#include <limits>

#include <vector>
#include <array>
#include <cmath>
#include <stdexcept>

#include "VisualizationPipeline.h"
#include "CubeFrame.h"
#include "AvtkKdTree.h"

class PointNormalProcessor
{
public:
    PointNormalProcessor();

    void SetInput(VisualizationPipeline *pipeline);
    void SetInput(vtkPolyData *polyData);

    void SetInputConnection(vtkAlgorithmOutput *port);

    vtkSmartPointer<vtkPolyData> GetPolyData() const { return processedPolyData; }

    vtkPointData *GetPointData() const { return processedPolyData->GetPointData(); }

    vtkDataArray *GetNormals() const { return processedPolyData->GetPointData()->GetNormals(); }

    AvtkKdTreePointLocator *GetPointLocator() const { return pointLocator; }

    double *GetPoint(vtkIdType id) const;
    void GetPoint(vtkIdType id, double *point) const;

    vtkSmartPointer<vtkIdList> FindPointsWithinRadius(double radius, const double *center) const;

    void FindPointsWithinRadius(double radius, const double *center, vtkIdList *resultIds) const;

    vtkSmartPointer<vtkIdList> FindPointsInCylinder(const double *point, const double *direction, double radius);

    void FindPointsInCylinder(const double *point, const double *direction, double radius, vtkIdList *resultIds);

    void SetGlyph3DVisibility(bool visibility);

    void SetGlyph3DScaleFactor(double scaleFactor);

    vtkSmartPointer<vtkActor> GetArrowActor();

    void SetRadiusRatio(double ratio);
    void SetIntervalRatio(double ratio);

    double GetRadiusRatio() const { return radiusRatio; }
    double GetIntervalRatio() const { return intervalRatio; }

    void Update();

    vtkSmartPointer<vtkIdList> GetUniquePointsInSpheres(
        const std::vector<std::array<double, 3>> &sphereCenters,
        double sphereRadius) const;

    static std::vector<std::array<double, 3>> GenerateSphereCenters(
        const double start[3],
        const double end[3],
        double sphereInterval,
        double sphereRadius);

    std::vector<CubeFrame *> GetRegionsBoundariesByLevel(int level);

    std::vector<CubeFrame *> GetRegionBoundsByPoint(double x, double y, double z);

private:
    double ComputeProjection(const double v[3], const double u[3]) const;
    std::array<double, 2> ComputeBoundingBoxProjectionRange(const double point[3], const double direction[3]) const;
    void BuildLocator();

    double radiusRatio = 1.2247;
    double intervalRatio = 1.4142;

    vtkSmartPointer<vtkPolyData> inputData;
    vtkSmartPointer<vtkPolyData> processedPolyData;
    vtkSmartPointer<AvtkKdTreePointLocator> pointLocator;
    vtkSmartPointer<vtkArrowSource> arrowSource;
    vtkSmartPointer<vtkGlyph3D> glyph3D;

    std::unique_ptr<VisualizationPipeline> arrowPipeline;
};
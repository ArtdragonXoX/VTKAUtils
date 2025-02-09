#pragma once
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkPolyDataNormals.h>
#include <vtkKdTreePointLocator.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <unordered_set>
#include <cmath>
#include <limits>

#include "VisualizationPipeline.h"

class PointNormalProcessor
{
public:
    PointNormalProcessor();
    
    void SetInput(VisualizationPipeline* pipeline);
    void SetInput(vtkPolyData* polyData);
    
    vtkSmartPointer<vtkPolyData> GetPolyData() const { return processedPolyData; }

    vtkPointData* GetPointData() const { return processedPolyData->GetPointData(); }

    vtkDataArray* GetNormals() const { return processedPolyData->GetPointData()->GetNormals(); }

    vtkSmartPointer<vtkKdTreePointLocator> GetPointLocator() const { return pointLocator; }

    double* GetPoint(vtkIdType id) const;
    void GetPoint(vtkIdType id, double* point) const;

    vtkSmartPointer<vtkIdList> FindPointsWithinRadius(double radius, const double* center) const;

    vtkSmartPointer<vtkIdList> FindPointsWithinRadius(double radius, const double* center, vtkIdList* resultIds) const;

    vtkSmartPointer<vtkIdList> FindPointsInCylinder(const double* point, const double* direction, double radius) const;

    void FindPointsInCylinder(const double* point, const double* direction, double radius, vtkIdList* resultIds) const;

    void SetGlyph3DVisibility(bool visibility);

    void SetGlyph3DScaleFactor(double scaleFactor);

    vtkSmartPointer<vtkActor> GetArrowActor();

    void Update();

private:
    void BuildLocator();
    
    vtkSmartPointer<vtkPolyData> inputData;
    vtkSmartPointer<vtkPolyData> processedPolyData;
    vtkSmartPointer<vtkKdTreePointLocator> pointLocator;
    vtkSmartPointer<vtkArrowSource> arrowSource;
    vtkSmartPointer<vtkGlyph3D> glyph3D;
    std::unique_ptr<VisualizationPipeline> arrowPipeline;
};
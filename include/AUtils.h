#pragma once
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkDataSet.h>

#include "OBB.h"

namespace AUtils
{

    const int cubeIndices[8][3] = {
        {0, 2, 4}, // xmin, ymin, zmin
        {1, 2, 4}, // xmax, ymin, zmin
        {0, 3, 4}, // xmin, ymax, zmin
        {1, 3, 4}, // xmax, ymax, zmin
        {0, 2, 5}, // xmin, ymin, zmax
        {1, 2, 5}, // xmax, ymin, zmax
        {0, 3, 5}, // xmin, ymax, zmax
        {1, 3, 5}  // xmax, ymax, zmax
    };

    const int cubeEdges[12][2] = {
        {0, 1}, {1, 3}, {0, 2}, {2, 3}, // 底面
        {4, 5},
        {5, 7},
        {4, 6},
        {6, 7}, // 顶面
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7} // 垂直边
    };

    void GetCornersFromBounds(const double *bounds, double **corners);

    void IdTypeArrayToIdList(vtkIdTypeArray *idTypeArray, vtkIdList *idList);

    void GetMeanNormal(double *normal, vtkDataArray *array);

    template <typename... Arrays>
    void GetMeanNormal(double *normal, vtkDataArray *firstArray, Arrays *...arrays);

    void GetOBB(vtkDataSet *data, double corner[8][3]);
};
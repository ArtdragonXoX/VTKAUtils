#pragma once

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
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // 底面
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4}, // 顶面
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7} // 垂直边
    };

    void GetCornersFromBounds(const double *bounds, double **corners)
    {
        for (int i = 0; i < 8; ++i)
        {
            corners[i][0] = bounds[AUtils::cubeIndices[i][0]]; // x
            corners[i][1] = bounds[AUtils::cubeIndices[i][1]]; // y
            corners[i][2] = bounds[AUtils::cubeIndices[i][2]]; // z
        }
    }

    void IdTypeArrayToIdList(vtkIdTypeArray *idTypeArray, vtkIdList *idList)
    {
        idList->SetNumberOfIds(idTypeArray->GetNumberOfTuples());
        for (int i = 0; i < idTypeArray->GetNumberOfTuples(); ++i)
        {
            idList->SetId(i, idTypeArray->GetValue(i));
        }
        idList->Squeeze();
    }
};
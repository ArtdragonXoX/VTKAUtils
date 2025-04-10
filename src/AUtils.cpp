#include "AUtils.h"

void AUtils::GetCornersFromBounds(const double *bounds, double **corners)
{
    for (int i = 0; i < 8; ++i)
    {
        corners[i][0] = bounds[AUtils::cubeIndices[i][0]]; // x
        corners[i][1] = bounds[AUtils::cubeIndices[i][1]]; // y
        corners[i][2] = bounds[AUtils::cubeIndices[i][2]]; // z
    }
}

void AUtils::IdTypeArrayToIdList(vtkIdTypeArray *idTypeArray, vtkIdList *idList)
{
    idList->SetNumberOfIds(idTypeArray->GetNumberOfTuples());
    for (int i = 0; i < idTypeArray->GetNumberOfTuples(); ++i)
    {
        idList->SetId(i, idTypeArray->GetValue(i));
    }
    idList->Squeeze();
}

void AUtils::GetMeanNormal(double *normal, vtkDataArray *array)
{
    if (array->GetNumberOfTuples() == 0)
    {
        return;
    }
    for (int i = 0; i < array->GetNumberOfTuples(); ++i)
    {
        double *point = array->GetTuple(i);
        normal[0] += point[0];
        normal[1] += point[1];
        normal[2] += point[2];
    }
    double len = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] /= len;
    normal[1] /= len;
    normal[2] /= len;
}

template <typename... Arrays>
void AUtils::GetMeanNormal(double *normal, vtkDataArray *firstArray, Arrays *...arrays)
{
    GetMeanNormal(normal, firstArray);
    if constexpr (sizeof...(arrays) > 0)
    {
        GetMeanNormal(normal, arrays...);
    }
}

void AUtils::GetOBB(vtkDataSet *data, double corner[3], double axes[3][3], double size[3])
{
    vtkNew<vtkOBBTree> obbTree;
    obbTree->SetDataSet(data);
    obbTree->SetMaxLevel(1);
    obbTree->BuildLocator();
    obbTree->ComputeOBB(data, corner, axes[0], axes[1], axes[2], size);
}

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

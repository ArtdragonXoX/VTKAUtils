#include "AUtils.h"
void AUtils::GetCornersFromBounds(const double *bounds, double **corners)
{
    for (int i = 0; i < 8; ++i)
    {
        corners[i][0] = bounds[cubeIndices[i][0]]; // x
        corners[i][1] = bounds[cubeIndices[i][1]]; // y
        corners[i][2] = bounds[cubeIndices[i][2]]; // z
    }
}
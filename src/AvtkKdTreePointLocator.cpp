#include "AvtkKdTreePointLocator.h"

#include "AvtkKdTree.h"
#include "vtkObjectFactory.h"
#include "vtkPointSet.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(AvtkKdTreePointLocator);

//------------------------------------------------------------------------------
AvtkKdTreePointLocator::AvtkKdTreePointLocator()
{
  this->KdTree = nullptr;
}

//------------------------------------------------------------------------------
AvtkKdTreePointLocator::~AvtkKdTreePointLocator()
{
  if (this->KdTree)
  {
    this->KdTree->Delete();
  }
}

//------------------------------------------------------------------------------
vtkIdType AvtkKdTreePointLocator::FindClosestPoint(const double x[3])
{
  this->BuildLocator();
  double dist2;

  return this->KdTree->FindClosestPoint(x[0], x[1], x[2], dist2);
}

//------------------------------------------------------------------------------
vtkIdType AvtkKdTreePointLocator::FindClosestPointWithinRadius(
    double radius, const double x[3], double &dist2)
{
  this->BuildLocator();
  return this->KdTree->FindClosestPointWithinRadius(radius, x, dist2);
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::FindClosestNPoints(int N, const double x[3], vtkIdList *result)
{
  this->BuildLocator();
  this->KdTree->FindClosestNPoints(N, x, result);
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::FindPointsWithinRadius(double R, const double x[3], vtkIdList *result)
{
  this->BuildLocator();
  this->KdTree->FindPointsWithinRadius(R, x, result);
}

void AvtkKdTreePointLocator::FindPointsWithinArea(double *area, vtkIdList *result)
{
  this->BuildLocator();
  this->KdTree->FindPointsWithInArea(area, result);
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::FreeSearchStructure()
{
  if (this->KdTree)
  {
    this->KdTree->Delete();
    this->KdTree = nullptr;
  }
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::BuildLocator()
{
  // don't rebuild if build time is newer than modified and dataset modified time
  if (this->KdTree && this->BuildTime > this->MTime && this->BuildTime > this->DataSet->GetMTime())
  {
    return;
  }
  // don't rebuild if UseExistingSearchStructure is ON and a search structure already exists
  if (this->KdTree && this->UseExistingSearchStructure)
  {
    this->BuildTime.Modified();
    vtkDebugMacro(<< "BuildLocator exited - UseExistingSearchStructure");
    return;
  }
  this->BuildLocatorInternal();
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::ForceBuildLocator()
{
  this->BuildLocatorInternal();
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::BuildLocatorInternal()
{
  if (!this->DataSet || (this->DataSet->GetNumberOfPoints()) < 1)
  {
    vtkErrorMacro(<< "No points to build");
    return;
  }
  // Prepare
  this->FreeSearchStructure();

  vtkPointSet *pointSet = vtkPointSet::SafeDownCast(this->GetDataSet());
  if (!pointSet)
  {
    vtkErrorMacro("AvtkKdTreePointLocator requires a PointSet to build locator.");
    return;
  }
  this->KdTree = AvtkKdTree::New();
  this->KdTree->SetUseExistingSearchStructure(this->UseExistingSearchStructure);
  this->KdTree->BuildLocatorFromPoints(pointSet);
  this->KdTree->GetBounds(this->Bounds);
  this->BuildTime.Modified();
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::GenerateRepresentation(int level, vtkPolyData *pd)
{
  this->BuildLocator();
  this->KdTree->GenerateRepresentation(level, pd);
}

AvtkKdTree *AvtkKdTreePointLocator::GetKdTree()
{
  return KdTree;
}

//------------------------------------------------------------------------------
void AvtkKdTreePointLocator::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "KdTree " << this->KdTree << "\n";
}

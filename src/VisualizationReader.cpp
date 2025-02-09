#include "VisualizationReader.h"

VisualizationReader::VisualizationReader() : VisualizationPipeline()
{
}

void VisualizationReader::ReadDicom(const char* dn)
{
	vtkNew<vtkDICOMImageReader> DICOMren;
	this->reader = DICOMren;
	DICOMren->SetDataByteOrderToLittleEndian();
	DICOMren->SetDirectoryName(dn);
	DICOMren->Update();
	SetInputConnection(DICOMren->GetOutputPort());
}

void VisualizationReader::ReadStl(const char* fn)
{
	vtkNew<vtkSTLReader> reader;
	this->reader = reader;
	reader->SetFileName(fn);
	reader->Update();
	SetInputConnection(reader->GetOutputPort());
}

void VisualizationReader::ReadDcm(const char* fn)
{
	vtkNew<vtkDICOMImageReader> DICOMren;
	this->reader = DICOMren;
	DICOMren->SetDirectoryName(fn);
	DICOMren->Update();
	SetInputConnection(DICOMren->GetOutputPort());

}
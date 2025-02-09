#pragma once

#include <vtkDICOMImageReader.h>
#include <vtkSTLReader.h>

#include "VisualizationPipeline.h"

class VisualizationReader : public VisualizationPipeline
{
public:
	VisualizationReader();
	~VisualizationReader() = default;

	void SetDicom(const char* dn);
	void SetStl(const char* fn);
	void SetDcm(const char* fn);

private:
	vtkSmartPointer<vtkAlgorithm> reader;
};
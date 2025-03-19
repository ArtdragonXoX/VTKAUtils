#pragma once

#include <vtkDICOMImageReader.h>
#include <vtkSTLReader.h>

#include "VisualizationPipeline.h"

class VisualizationReader : public VisualizationPipeline
{
public:
	VisualizationReader();
	~VisualizationReader() = default;

	void ReadDicom(const char *dn);
	void ReadStl(const char *fn);
	void ReadDcm(const char *fn);

private:
	vtkSmartPointer<vtkAlgorithm> reader;
};
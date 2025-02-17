﻿#pragma once

#define PipelineMacro                                                    \
	vtkActor *GetActor() const { return visualizationPipeline->GetActor(); }          \
	void SetVisibility(bool arg) const { visualizationPipeline->SetVisibility(arg); } \
	void SetOpacity(double arg) { visualizationPipeline->SetOpacity(arg); }           \
	vtkProperty *GetProperty() const { return visualizationPipeline->GetProperty(); } \
	bool GetVisibility() const { return visualizationPipeline->GetVisibility(); }     \
	double GetOpacity() const { return visualizationPipeline->GetOpacity(); } \
protected: \
	std::unique_ptr<VisualizationPipeline> visualizationPipeline = std::make_unique<VisualizationPipeline>(); \
public:

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSTLWriter.h>

class VisualizationPipeline
{
public:
	VisualizationPipeline();
	VisualizationPipeline(vtkSmartPointer<vtkPolyData> polyData);

	~VisualizationPipeline() = default;

	void SetInput(vtkSmartPointer<vtkPolyData> polyData);
	void SetInputConnection(vtkAlgorithmOutput *port);
	vtkSmartPointer<vtkPolyData> GetOutput() const;
	vtkAlgorithmOutput *GetInputPort() const;
	vtkSmartPointer<vtkPolyDataMapper> GetMapper();
	vtkSmartPointer<vtkActor> GetActor() const;
	vtkAlgorithmOutput *GetOutputPort() const;
	void SetVisibility(bool arg) const;
	void SetOpacity(double arg);
	vtkProperty *GetProperty() const;

	bool GetVisibility() const;
	double GetOpacity() const;

	void AddAlgorithm(vtkSmartPointer<vtkPolyDataAlgorithm> algorithm);
	vtkSmartPointer<vtkPolyDataAlgorithm> GetAlgorithm(int index) const;

	void ApplyAlgorithms();

	void WriteSTL(const char *fn);

	void Update();

private:
	std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> algorithms;
	vtkSmartPointer<vtkPolyData> polyData;
	vtkAlgorithmOutput *inputPort;
	vtkSmartPointer<vtkPolyDataMapper> polyDataMapper;
	vtkSmartPointer<vtkActor> actor;
};

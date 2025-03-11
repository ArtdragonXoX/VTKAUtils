#pragma once

#define PipelineMacro(class_name)                                                   \
	vtkActor *GetActor() const { return class_name->GetActor(); }                   \
	void SetVisibility(bool arg) const { class_name->SetVisibility(arg); }          \
	void SetOpacity(double arg) { class_name->SetOpacity(arg); }                    \
	vtkProperty *GetProperty() const { return class_name->GetProperty(); }          \
	bool GetVisibility() const { return class_name->GetVisibility(); }              \
	double GetOpacity() const { return class_name->GetOpacity(); }                  \
	void SetColor(double r, double g, double b) { class_name->SetColor(r, g, b); }  \
	vtkPolyData *GetOutput() const { return class_name->GetOutput(); }              \
	vtkAlgorithmOutput *GetInputPort() const { return class_name->GetInputPort(); } \
	void ScalarVisibilityOff() { class_name->ScalarVisibilityOff(); }               \
	void ScalarVisibilityOn() { class_name->ScalarVisibilityOn(); }                 \
                                                                                    \
public:

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSTLWriter.h>
#include <vtkAlgorithmOutput.h>

class VisualizationPipeline
{
public:
	VisualizationPipeline();
	VisualizationPipeline(vtkPolyData *polyData);

	~VisualizationPipeline() = default;

	void ScalarVisibilityOff();
	void ScalarVisibilityOn();

	void SetInput(vtkPolyData *polyData);
	void SetInputConnection(vtkAlgorithmOutput *port);
	vtkPolyData *GetOutput() const;
	vtkAlgorithmOutput *GetInputPort() const;
	vtkPolyDataMapper *GetMapper();
	vtkActor *GetActor() const;
	vtkAlgorithmOutput *GetOutputPort() const;
	void SetVisibility(bool arg) const;
	void SetOpacity(double arg);
	vtkProperty *GetProperty() const;

	void SetColor(double r, double g, double b);

	bool GetVisibility() const;
	double GetOpacity() const;

	void AddAlgorithm(vtkPolyDataAlgorithm *algorithm);
	vtkPolyDataAlgorithm *GetAlgorithm(int index) const;

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

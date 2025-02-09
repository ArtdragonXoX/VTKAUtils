#pragma once

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
	void SetInputConnection(vtkAlgorithmOutput* port);
	vtkSmartPointer<vtkPolyData> GetOutput() const;
	vtkAlgorithmOutput* GetInputPort() const;
	vtkSmartPointer<vtkPolyDataMapper> GetMapper();
	vtkSmartPointer<vtkActor> GetActor() const;
	vtkAlgorithmOutput* GetOutputPort() const;
	void SetVisibility(bool arg) const;
	void SetOpacity(double arg);
	vtkProperty* GetProperty() const;

	bool GetVisibility() const;
	double GetOpacity() const;

	void AddAlgorithm(vtkSmartPointer<vtkPolyDataAlgorithm> algorithm);
	vtkSmartPointer<vtkPolyDataAlgorithm> GetAlgorithm(int index) const;

	void ApplyAlgorithms();

	void WriteSTL(const char* fn);

	void Update();
private:

	std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> algorithms;
	vtkSmartPointer<vtkPolyData> polyData;
	vtkAlgorithmOutput* inputPort;
	vtkSmartPointer<vtkPolyDataMapper> polyDataMapper;
	vtkSmartPointer<vtkActor> actor;
};

template <typename DerivedType>
DerivedType* VtkCastToDerived(vtkAlgorithm* algorithm) {
	// 使用 dynamic_cast 进行类型转换
	return dynamic_cast<DerivedType*>(algorithm);
}
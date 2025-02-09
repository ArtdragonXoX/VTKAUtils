#include "VisualizationPipeline.h"

VisualizationPipeline::VisualizationPipeline()
{
	polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(polyDataMapper);
}

VisualizationPipeline::VisualizationPipeline(vtkSmartPointer<vtkPolyData> polyData)
{
	polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(polyDataMapper);
	SetInput(polyData);
}

void VisualizationPipeline::SetInput(vtkSmartPointer<vtkPolyData> polyData)
{
	this->polyData = polyData;
	inputPort = nullptr;
	polyDataMapper->SetInputData(polyData);
}

void VisualizationPipeline::SetInputConnection(vtkAlgorithmOutput* port)
{
	polyData = nullptr;
	inputPort = port;
	polyDataMapper->SetInputConnection(inputPort);
}

vtkSmartPointer<vtkPolyData> VisualizationPipeline::GetOutput() const
{
	if (polyData)
		return polyData;
	return polyDataMapper->GetInput();
}

vtkAlgorithmOutput* VisualizationPipeline::GetInputPort() const
{
	return inputPort;
}

vtkSmartPointer<vtkPolyDataMapper> VisualizationPipeline::GetMapper()
{
	return polyDataMapper;
}

vtkSmartPointer<vtkActor> VisualizationPipeline::GetActor() const
{
	return actor;
}

vtkAlgorithmOutput* VisualizationPipeline::GetOutputPort() const
{
	return polyDataMapper->GetInputConnection(0, 0);
}

void VisualizationPipeline::SetVisibility(bool arg) const
{
	actor->SetVisibility(arg);
}

void VisualizationPipeline::SetOpacity(double arg)
{
	actor->GetProperty()->SetOpacity(arg);
}

vtkProperty* VisualizationPipeline::GetProperty() const
{
	return GetActor()->GetProperty();
}

bool VisualizationPipeline::GetVisibility() const
{
	return actor->GetVisibility();
}

double VisualizationPipeline::GetOpacity() const
{
	return actor->GetProperty()->GetOpacity();
}

void VisualizationPipeline::AddAlgorithm(vtkSmartPointer<vtkPolyDataAlgorithm> algorithm)
{
	algorithms.push_back(algorithm);
	ApplyAlgorithms();
}

vtkSmartPointer<vtkPolyDataAlgorithm> VisualizationPipeline::GetAlgorithm(int index) const
{
	return algorithms[index];
}

void VisualizationPipeline::ApplyAlgorithms() {
	if (algorithms.empty()) {
		if (polyData == nullptr)
			polyDataMapper->SetInputConnection(inputPort);
		else
			polyDataMapper->SetInputData(polyData);
		return;
	}

	if (polyData == nullptr)
		algorithms[0]->SetInputConnection(inputPort);
	else
		algorithms[0]->SetInputData(polyData);

	for (size_t i = 1; i < algorithms.size(); ++i) {
		algorithms[i]->SetInputConnection(algorithms[i - 1]->GetOutputPort());
	}

	polyDataMapper->SetInputConnection(algorithms.back()->GetOutputPort());

	// 更新整个流水线
	this->Update();
}

void VisualizationPipeline::Update()
{
	for (auto& algorithm : algorithms)
	{
		algorithm->Update();
	}
}
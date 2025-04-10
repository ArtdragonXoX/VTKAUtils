#include "VisualizationPipeline.h"

VisualizationPipeline::VisualizationPipeline()
{
	polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(polyDataMapper);
}

VisualizationPipeline::VisualizationPipeline(vtkPolyData *polyData)
{
	polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(polyDataMapper);
	SetInput(polyData);
}

void VisualizationPipeline::ScalarVisibilityOff()
{
	polyDataMapper->ScalarVisibilityOff();
}

void VisualizationPipeline::ScalarVisibilityOn()
{
	polyDataMapper->ScalarVisibilityOn();
}

void VisualizationPipeline::SetInput(vtkPolyData *polyData)
{
	this->polyData = polyData;
	inputPort = nullptr;
	polyDataMapper->SetInputData(polyData);
}

void VisualizationPipeline::SetInputConnection(vtkAlgorithmOutput *port)
{
	polyData = nullptr;
	inputPort = port;
	polyDataMapper->SetInputConnection(inputPort);
}

vtkPolyData *VisualizationPipeline::GetOutput() const
{
	return polyData;
}

vtkAlgorithmOutput *VisualizationPipeline::GetInputPort() const
{
	return inputPort;
}

vtkPolyDataMapper *VisualizationPipeline::GetMapper()
{
	return polyDataMapper;
}

vtkActor *VisualizationPipeline::GetActor() const
{
	return actor;
}

vtkAlgorithmOutput *VisualizationPipeline::GetOutputPort() const
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

vtkProperty *VisualizationPipeline::GetProperty() const
{
	return GetActor()->GetProperty();
}

void VisualizationPipeline::GetBounds(double bounds[6])
{
	// 如果有算法链，从最后一个算法的输出获取边界
	if (!algorithms.empty())
	{
		vtkPolyData *output = vtkPolyData::SafeDownCast(algorithms.back()->GetOutput());
		if (output)
		{
			output->GetBounds(bounds);
			return;
		}
	}

	// 如果没有算法链或获取失败，尝试从polyData获取边界
	if (polyData)
	{
		polyData->GetBounds(bounds);
		return;
	}

	if (inputPort && inputPort->GetProducer())
	{
		// 先更新生产者以确保获取最新数据
		inputPort->GetProducer()->Update();
		vtkPolyData *output = vtkPolyData::SafeDownCast(inputPort->GetProducer()->GetOutputDataObject(0));
		if (output)
		{
			output->GetBounds(bounds);
			return;
		}
	}

	// 如果polyData也不存在，尝试从actor获取边界
	if (actor)
	{
		actor->GetBounds(bounds);
		return;
	}

	// 如果所有方法都失败，设置默认边界值
	for (int i = 0; i < 6; ++i)
	{
		bounds[i] = 0.0;
	}
}

void VisualizationPipeline::GetOBB(double corner[8][3])
{
	if (!algorithms.empty())
	{
		vtkDataSet *dataSet = vtkPolyData::SafeDownCast(algorithms.back()->GetOutput());
		if (dataSet)
		{
			AUtils::GetOBB(dataSet, corner);
			return;
		}
	}

	if (polyData)
	{
		AUtils::GetOBB(polyData, corner);
		return;
	}

	if (inputPort && inputPort->GetProducer())
	{
		inputPort->GetProducer()->Update();
		vtkDataSet *dataSet = vtkPolyData::SafeDownCast(inputPort->GetProducer()->GetOutputDataObject(0));
		if (dataSet)
		{
			AUtils::GetOBB(dataSet, corner);
			return;
		}
	}

	if (actor)
	{
		vtkDataSet *dataSet = actor->GetMapper()->GetInput();
		AUtils::GetOBB(dataSet, corner);
		return;
	}
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			corner[i][j] = 0.0;
		}
	}
}

void VisualizationPipeline::SetColor(double r, double g, double b)
{
	actor->GetProperty()->SetColor(r, g, b);
}

bool VisualizationPipeline::GetVisibility() const
{
	return actor->GetVisibility();
}

double VisualizationPipeline::GetOpacity() const
{
	return actor->GetProperty()->GetOpacity();
}

void VisualizationPipeline::AddAlgorithm(vtkPolyDataAlgorithm *algorithm)
{
	algorithms.push_back(algorithm);
	ApplyAlgorithms();
}

vtkPolyDataAlgorithm *VisualizationPipeline::GetAlgorithm(int index) const
{
	return algorithms[index];
}

void VisualizationPipeline::ApplyAlgorithms()
{
	if (algorithms.empty())
	{
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

	for (size_t i = 1; i < algorithms.size(); ++i)
	{
		algorithms[i]->SetInputConnection(algorithms[i - 1]->GetOutputPort());
	}

	polyDataMapper->SetInputConnection(algorithms.back()->GetOutputPort());

	// 更新整个流水线
	this->Update();
}

void VisualizationPipeline::WriteSTL(const char *fn)
{
	vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
	writer->SetFileName(fn);
	writer->SetInputData(GetOutput());
	writer->Write();
}

void VisualizationPipeline::Update()
{
	for (auto &algorithm : algorithms)
	{
		algorithm->Update();
	}
}
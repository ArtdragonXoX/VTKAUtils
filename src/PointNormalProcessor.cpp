#include "PointNormalProcessor.h"

PointNormalProcessor::PointNormalProcessor()
{
    pointLocator = vtkSmartPointer<AvtkKdTreePointLocator>::New();
    arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
    glyph3D->SetSourceConnection(arrowSource->GetOutputPort());
    arrowPipeline = std::make_unique<VisualizationPipeline>();
    arrowPipeline->SetVisibility(false);
}

void PointNormalProcessor::SetInput(VisualizationPipeline *pipeline)
{
    inputData = pipeline->GetOutput();
    if (!inputData)
        throw std::runtime_error("Input data is not set");
    Update();
}

void PointNormalProcessor::SetInput(vtkPolyData *polyData)
{
    inputData = polyData;
    if (!inputData)
        throw std::runtime_error("Input data is not set");
    Update();
}

// 设置PointNormalProcessor的输入连接
// 参数 port 指向一个vtkAlgorithmOutput对象，代表数据流的输出端口
void PointNormalProcessor::SetInputConnection(vtkAlgorithmOutput *port)
{
    // 通过 port 获取生产该输出的算法和对应的输出索引
    vtkAlgorithm *producer = port->GetProducer();
    int index = port->GetIndex();

    // 将生产者的数据对象安全转换为vtkPolyData类型
    vtkPolyData *polyData = vtkPolyData::SafeDownCast(producer->GetOutputDataObject(index));
    // 如果转换失败，抛出运行时错误
    if (!polyData)
        throw std::runtime_error("Pipeline output is not vtkPolyData");

    // 创建一个新的vtkPolyData对象作为输入数据的深拷贝
    inputData = vtkSmartPointer<vtkPolyData>::New();
    // 浅拷贝polyData到inputData，因为数据结构不会被修改，仅复制指针
    inputData->ShallowCopy(polyData);

    // 调用Update方法处理输入数据，尽管这里没有显示Update的实现
    Update();
}

void PointNormalProcessor::BuildLocator()
{
    pointLocator->SetDataSet(processedPolyData);
    pointLocator->BuildLocator();
}

double *PointNormalProcessor::GetPoint(vtkIdType id) const
{
    return processedPolyData->GetPoint(id);
}

void PointNormalProcessor::GetPoint(vtkIdType id, double *point) const
{
    processedPolyData->GetPoint(id, point);
}

vtkSmartPointer<vtkIdList> PointNormalProcessor::FindPointsWithinRadius(double radius, const double *center) const
{
    vtkNew<vtkIdList> resultIds;
    FindPointsWithinRadius(radius, center, resultIds);
    return resultIds;
}

void PointNormalProcessor::FindPointsWithinRadius(double radius, const double *center, vtkIdList *resultIds) const
{
    pointLocator->FindPointsWithinRadius(radius, center, resultIds);
}

vtkSmartPointer<vtkIdList> PointNormalProcessor::FindPointsInCylinder(const double *point, const double *direction, double radius)
{
    vtkNew<vtkIdList> resultIds;
    FindPointsInCylinder(point, direction, radius, resultIds);
    return resultIds;
}

void PointNormalProcessor::FindPointsInCylinder(const double *point, const double *direction, double radius, vtkIdList *resultIds)
{
    // 归一化方向向量
    double norm = std::sqrt(direction[0] * direction[0] +
                            direction[1] * direction[1] +
                            direction[2] * direction[2]);
    if (norm < 1e-8)
        throw std::invalid_argument("Direction vector is zero.");
    double u[3] = {direction[0] / norm, direction[1] / norm, direction[2] / norm};

    // 调用新函数获取包围盒的投影范围
    std::array<double, 2> projRange = ComputeBoundingBoxProjectionRange(point, u);
    double minProj = projRange[0];
    double maxProj = projRange[1];

    // 将范围适当扩展，以确保覆盖整个圆柱体区域（在两端各扩展一个 radius）
    double start = minProj - radius * radiusRatio;
    double end = maxProj + radius * radiusRatio;

    // 基于球体搜索方式获取候选点集：先生成球体中心，再利用 GetUniquePointsInSpheres 汇总查询结果
    double sphereRadius = radius * radiusRatio;
    double step = sphereRadius * intervalRatio;

    double vecStart[3] = {
        point[0] + start * u[0],
        point[1] + start * u[1],
        point[2] + start * u[2]};
    double vecEnd[3] = {
        point[0] + end * u[0],
        point[1] + end * u[1],
        point[2] + end * u[2]};

    std::vector<std::array<double, 3>> sphereCenters = GenerateSphereCenters(vecStart, vecEnd, step, sphereRadius);
    vtkSmartPointer<vtkIdList> candidatePoints = GetUniquePointsInSpheres(sphereCenters, sphereRadius);

    // 对候选点进行圆柱体内的筛选
    for (vtkIdType i = 0; i < candidatePoints->GetNumberOfIds(); i++)
    {
        vtkIdType pid = candidatePoints->GetId(i);
        double p[3];
        processedPolyData->GetPoint(pid, p);
        double v[3] = {p[0] - point[0],
                       p[1] - point[1],
                       p[2] - point[2]};
        double proj = ComputeProjection(v, u);
        double vLenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        double perpSq = vLenSq - proj * proj;
        if (perpSq <= radius * radius)
            resultIds->InsertNextId(pid);
    }
}

void PointNormalProcessor::SetGlyph3DVisibility(bool visibility)
{
    arrowPipeline->SetVisibility(visibility);
}

void PointNormalProcessor::SetGlyph3DScaleFactor(double scaleFactor)
{
    glyph3D->SetScaleFactor(scaleFactor);
    glyph3D->Update();
}

vtkSmartPointer<vtkActor> PointNormalProcessor::GetArrowActor()
{
    return arrowPipeline->GetActor();
}

void PointNormalProcessor::SetRadiusRatio(double ratio)
{
    radiusRatio = ratio;
}

void PointNormalProcessor::SetIntervalRatio(double ratio)
{
    intervalRatio = ratio;
}

void PointNormalProcessor::Update()
{
    // 三角化处理
    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->SetInputData(inputData);
    triangleFilter->Update();
    vtkSmartPointer<vtkPolyData> processedData = triangleFilter->GetOutput();

    // 计算法向量
    vtkNew<vtkPolyDataNormals> normalGenerator;
    normalGenerator->SetInputData(processedData);
    normalGenerator->SetComputePointNormals(true);
    normalGenerator->SetSplitting(false);
    normalGenerator->SetConsistency(false);
    normalGenerator->SetAutoOrientNormals(true);
    normalGenerator->Update();

    processedPolyData = normalGenerator->GetOutput();
    BuildLocator();

    glyph3D->SetInputData(processedPolyData);

    glyph3D->SetVectorModeToUseNormal();
    glyph3D->SetScaleFactor(1.0);
    glyph3D->Update();
    arrowPipeline->SetInput(glyph3D->GetOutput());
}

std::vector<std::array<double, 3>> PointNormalProcessor::GenerateSphereCenters(
    const double start[3],
    const double end[3],
    double sphereInterval,
    double sphereRadius)
{
    std::vector<std::array<double, 3>> centers;
    // 计算起点到终点的差值和距离
    double diff[3] = {end[0] - start[0],
                      end[1] - start[1],
                      end[2] - start[2]};
    double totalDist = std::sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

    // 如果起点和终点几乎相同，则只返回一个球中心
    if (totalDist < 1e-8)
    {
        centers.push_back({start[0], start[1], start[2]});
        return centers;
    }

    // 计算单位方向向量
    double u[3] = {diff[0] / totalDist,
                   diff[1] / totalDist,
                   diff[2] / totalDist};

    // 使用输入的起点和终点，不再额外扩展范围
    double effectiveDistance = totalDist;

    // 根据球间隔确定需要的球数（+1保证起点也包含）
    int numSpheres = static_cast<int>(std::ceil(effectiveDistance / sphereInterval)) + 1;

    for (int i = 0; i < numSpheres; i++)
    {
        double d = i * sphereInterval;
        if (d > effectiveDistance)
            d = effectiveDistance;
        std::array<double, 3> center = {
            start[0] + d * u[0],
            start[1] + d * u[1],
            start[2] + d * u[2]};
        centers.push_back(center);
    }
    // 如果最后一个中心与end差异较大，则额外添加end以确保完全覆盖
    std::array<double, 3> lastCenter = centers.back();
    double diffToEnd = std::sqrt((lastCenter[0] - end[0]) * (lastCenter[0] - end[0]) +
                                 (lastCenter[1] - end[1]) * (lastCenter[1] - end[1]) +
                                 (lastCenter[2] - end[2]) * (lastCenter[2] - end[2]));
    if (diffToEnd > 1e-8)
    {
        centers.push_back({end[0], end[1], end[2]});
    }

    return centers;
}

std::vector<CubeFrame *> PointNormalProcessor::GetRegionsBoundariesByLevel(int level)
{
    auto kdTree = pointLocator->GetKdTree();
    return kdTree->GetRegionsBoundariesByLevel(level);
}

vtkSmartPointer<vtkIdList> PointNormalProcessor::GetUniquePointsInSpheres(
    const std::vector<std::array<double, 3>> &sphereCenters,
    double sphereRadius) const
{
    std::unordered_set<vtkIdType> uniqueIds;

    // 遍历每个球中心，查找对应半径内的所有点
    for (const auto &center : sphereCenters)
    {
        // center.data() 返回指向数组首地址
        vtkSmartPointer<vtkIdList> sphereIds = FindPointsWithinRadius(sphereRadius, center.data());
        for (vtkIdType i = 0; i < sphereIds->GetNumberOfIds(); i++)
        {
            uniqueIds.insert(sphereIds->GetId(i));
        }
    }

    // 将去重后的点 id 转换为 vtkIdList 返回
    vtkSmartPointer<vtkIdList> resultIds = vtkSmartPointer<vtkIdList>::New();
    for (const auto &id : uniqueIds)
    {
        resultIds->InsertNextId(id);
    }

    return resultIds;
}

double PointNormalProcessor::ComputeProjection(const double v[3], const double u[3]) const
{
    return v[0] * u[0] + v[1] * u[1] + v[2] * u[2];
}

std::array<double, 2> PointNormalProcessor::ComputeBoundingBoxProjectionRange(const double point[3], const double direction[3]) const
{
    double bounds[6];
    processedPolyData->GetBounds(bounds);

    std::array<double, 2> range = {std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()};
    // 定义包围盒8个角点
    double corners[8][3] = {
        {bounds[0], bounds[2], bounds[4]},
        {bounds[1], bounds[2], bounds[4]},
        {bounds[0], bounds[3], bounds[4]},
        {bounds[1], bounds[3], bounds[4]},
        {bounds[0], bounds[2], bounds[5]},
        {bounds[1], bounds[2], bounds[5]},
        {bounds[0], bounds[3], bounds[5]},
        {bounds[1], bounds[3], bounds[5]}};

    for (int i = 0; i < 8; i++)
    {
        double vec[3] = {corners[i][0] - point[0],
                         corners[i][1] - point[1],
                         corners[i][2] - point[2]};
        double proj = ComputeProjection(vec, direction);
        if (proj < range[0])
            range[0] = proj;
        if (proj > range[1])
            range[1] = proj;
    }
    return range;
}

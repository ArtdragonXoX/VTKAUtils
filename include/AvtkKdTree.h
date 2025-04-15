#pragma once

#include <vtkKdTree.h>
#include <vtkKdNode.h>
#include "CubeFrame.h"

class AvtkKdTree : public vtkKdTree
{
public:
    vtkTypeMacro(AvtkKdTree, vtkKdTree);
    static AvtkKdTree *New();

    /**
     * 获取指定层级的所有区域的边界框。
     * @param level 要查询的层级。
     * @return 包含该层级所有区域边界框的CubeFrame指针列表。
     */
    std::vector<CubeFrame *> GetRegionsBoundariesByLevel(int level);

    /**
     * 获取指定层级的区域数量。
     *
     * @param level 要查询的层级。层级从0开始计数。
     * @return 该层级的区域数量。
     *         - 若层级小于0，返回0。
     *         - 若层级超过最大层级，返回最大层级值。
     *         - 否则返回该层级的实际区域数量。
     */
    int GetNumberOfRegionsAtLevel(int level);

    /**
     * 获取KD树的最大层级。
     *
     * @return int 最大层级值，从根节点开始计算层级深度。
     */
    int GetMaxLevel() const;

    /**
     * 计算KD树从指定节点开始的最大层级。
     * @param node 当前处理的KD树节点。
     * @param currentLevel 当前节点所在的层级。
     * @return 返回以node为根节点的子树的最大层级，若节点为空则返回-1。
     */
    int CalculateMaxLevel(vtkKdNode *node, int currentLevel) const;

    /**
     * 根据给定点坐标获取包含该点的区域及其路径上的所有边界框
     * @param x 点的X坐标
     * @param y 点的Y坐标
     * @param z 点的Z坐标
     * @return 包含路径上所有节点边界框的CubeFrame指针列表（按路径顺序），若未找到区域则返回空列表
     */
    std::vector<CubeFrame *> GetRegionBoundsByPoint(double x, double y, double z);

    void FindPointsInArea(double *area, vtkIdList *ids);

    void FindPointsInCuboid(double cuboid[8][3], vtkIdList *ids);

protected:
    /**
     * 统计指定层级下二叉树节点的数量。
     *
     * @param node 要统计的节点的根节点。
     * @param currentLevel 当前节点所在的层级。
     * @param targetLevel 目标层级，统计该层级下的节点数量。
     * @return 返回目标层级下节点的数量。
     */
    int CountNodesAtLevel(vtkKdNode *, int currentLevel, int targetLevel);

    /**
     * 获取指定节点在Kd树中的层级。
     *
     * @param node 目标节点的指针。
     * @return 该节点的层级，根节点的层级为0。
     */
    int GetNodeLevel(vtkKdNode *node) const;

    /**
     * 在Kd树中查找目标节点的层级。
     *
     * @param current 当前遍历的Kd树节点。
     * @param target 需要查找层级的目标节点。
     * @param currentLevel 当前节点的层级值。
     * @return 若找到目标节点，返回其层级；否则返回-1。
     */
    int FindNodeLevel(vtkKdNode *, vtkKdNode *, int) const;

    /**
     * 获取从根节点到指定目标节点的路径。
     *
     * 该函数从Kd树的根节点开始，递归查找并返回从根节点到目标节点的完整路径。
     * 路径以节点指针的向量形式返回，顺序从根节点到目标节点。
     *
     * @param target 目标节点的指针。路径将终止于此节点。
     * @return 包含从根节点到目标节点路径的节点指针向量。若目标节点不存在于树中，可能返回空向量或部分路径。
     */
    std::vector<vtkKdNode *> GetPathFromRootToNode(vtkKdNode *target) const;

    /// \brief 获取从当前节点到目标节点的路径。
    /// \param current 当前节点的起始位置。
    /// \param target 需要查找的目标节点。
    /// \return 返回从current到target的节点路径列表。若路径不存在或current为nullptr，返回空列表。
    std::vector<vtkKdNode *> getPath(vtkKdNode *current, vtkKdNode *target) const;

    vtkPointSet *pointSet;
};

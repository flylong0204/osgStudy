
#ifndef MESH_TO_TRIANGLES_H
#define MESH_TO_TRIANGLES_H 1

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/NodeVisitor>
#include <osg/ComputeBoundsVisitor>
#include <osg/ShapeDrawable>
#include <osg/PrimitiveSet>

/*!
  创建结构体，用于储存三角形顶点和重心信息
*/
struct TriangleSelf
{
public:
	osg::Vec3f _p0;
	osg::Vec3f _p1;
	osg::Vec3f _p2;
	osg::Vec3f _centre;
	TriangleSelf(osg::Vec3f p0, osg::Vec3f p1, osg::Vec3f p2, osg::Vec3f centre)
		: _p0(p0)
		, _p1(p1)
		, _p2(p2)
		, _centre(centre)
	{}
	TriangleSelf()
	{}
};

/*!
  创建结构体，用于储存格子编号和三角形
*/
struct Classification
{
public:
	int _gridIndex;
	std::vector<TriangleSelf> _triangleselfList;
	Classification(int &gridIndex)
	{
		_gridIndex = gridIndex;
	}
};

/*!
  创建nodevisitor，用于遍历节点，得到所有三角形的顶点
*/
class MeshNodeVisitor : public osg::NodeVisitor
{
public:
	
	MeshNodeVisitor();

	/*! 重载相关函数接口 */
	virtual void apply(osg::MatrixTransform &node);
	virtual void apply(osg::Group &group);
	virtual void apply(osg::Geode &geode);

	osg::Vec3Array* getTriPoints();

private:
	/*! 顶点索引 */
	osg::ref_ptr<osg::DrawElementsUInt> triIndex;
	/*! 三角形顶点数据 */
	osg::ref_ptr<osg::Vec3Array> triPoints;
};

/*!
 绘制划分网格，以origin为起点，水平绘制
*/
osg::Group* createGrids(const osg::Vec3f origin, int row, int col, int pace);

/*!
 创建包围盒，获取划分网格的参考位置以及参考大小
*/
osg::Geode* createBoundingBox(osg::Node *node, osg::Vec3f &origin, float &sizeX, float &sizeY);

/*!
  创建函数，用于判断一个点是否在矩形内部
*/
bool isPointinPolygon(osg::Vec2f p, osg::ref_ptr<osg::Vec2Array> pPoly);

#endif 
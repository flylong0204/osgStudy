
#include "Mesh2Triangle.h"
#include <osg/io_utils>
#include <sstream>
#include <iostream>


MeshNodeVisitor::MeshNodeVisitor()
	: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	, triPoints(new osg::Vec3Array)
	, triIndex(new osg::DrawElementsUInt)
{}

void MeshNodeVisitor::apply(osg::MatrixTransform & node)
{
	traverse(node);
}

void MeshNodeVisitor::apply(osg::Group & group)
{
	traverse(group);
}

void MeshNodeVisitor::apply(osg::Geode & geode)
{
	unsigned int cDrawable = geode.getNumDrawables();
	for (unsigned int i = 0; i < cDrawable; i++)
	{
		osg::Geometry* geometry = geode.getDrawable(i)->asGeometry();
		if (!geometry)
		{
			continue;
		}
		for (unsigned int n = 0; n < geometry->getNumPrimitiveSets(); ++n)
		{
			osg::PrimitiveSet* ps = geometry->getPrimitiveSet(n);
			if (!ps)
			{
				continue;
			}
			/*! 获取几何对象的顶点数组 */
			if (osg::PrimitiveSet::TRIANGLES == ps->getMode())
			{
				osg::Vec3Array* pVertex = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
				triIndex = dynamic_cast<osg::DrawElementsUInt*>(ps);
				const unsigned indexNum = triIndex->getNumIndices();
				for (unsigned int m = 0; m<indexNum; m++)
				{
					triPoints->push_back(pVertex->at(triIndex->at(m)));
				}
			}
			else if (osg::PrimitiveSet::TRIANGLE_STRIP == ps->getMode())
			{

			}
			else
			{
				std::cout << "Non-Triangle, PrimitiveSet type is " 
					<< ps->getMode() << std::endl;
			}
		}
	}
	traverse(geode);
}

osg::Vec3Array * MeshNodeVisitor::getTriPoints()
{
	return triPoints.release();
}

osg::Group* createGrids(const osg::Vec3f origin, int row, int col, int pace)
{
	osg::ref_ptr<osg::Group>pGroup = new osg::Group();
	float xorigin = origin.x();
	float yorigin = origin.y();
	float zorigin = origin.z();
	for (int i = 0; i<row; i++)
	{
		for (int j = 0; j<col; j++)
		{
			/*! 四边形的顶点，逆时针顺序 */
			osg::ref_ptr<osg::Vec3Array> vertexs = new osg::Vec3Array;
			vertexs->push_back(osg::Vec3f(xorigin + j * pace, yorigin - i * pace, zorigin));
			vertexs->push_back(osg::Vec3f(xorigin + (j + 1)*pace, yorigin - i * pace, zorigin));
			vertexs->push_back(osg::Vec3f(xorigin + (j + 1)*pace, yorigin - (i + 1)*pace, zorigin));
			vertexs->push_back(osg::Vec3f(xorigin + j * pace, yorigin - (i + 1)*pace, zorigin));

			osg::ref_ptr<osg::Geode> geodePlane = new osg::Geode;
			osg::ref_ptr<osg::Geometry> plane = new osg::Geometry();
			plane->setVertexArray(vertexs);
			plane->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
			osg::ref_ptr<osg::Vec4Array> planeColor = new osg::Vec4Array;
			planeColor->push_back(osg::Vec4(0, 0, 0, 0.1));
			plane->setColorArray(planeColor);
			plane->setColorBinding(osg::Geometry::BIND_OVERALL);
			geodePlane->addDrawable(plane.get());
			geodePlane->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
			int gridIndex = i * col + j;
			std::ostringstream oss;//创建一个流
			oss << gridIndex;//把值传递如流中
			std::string gridName = oss.str();
			geodePlane->setName(gridName);

			osg::ref_ptr<osg::Geode> geodeBound = new osg::Geode;
			osg::ref_ptr<osg::Geometry> bound = new osg::Geometry();
			bound->setVertexArray(vertexs);
			osg::ref_ptr<osg::Vec4Array> boundColor = new osg::Vec4Array;
			boundColor->push_back(osg::Vec4(1, 1, 1, 1));
			bound->setColorArray(boundColor);
			bound->setColorBinding(osg::Geometry::BIND_OVERALL);
			bound->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));
			geodeBound->addDrawable(bound.get());
			geodeBound->getOrCreateStateSet()->setAttributeAndModes(
				new osg::LineWidth(1.0), osg::StateAttribute::ON);

			pGroup->addChild(geodePlane);
			pGroup->addChild(geodeBound);
		}
	}
	return pGroup.release();
}

osg::Geode * createBoundingBox(osg::Node * node, osg::Vec3f & origin, float & sizeX, float & sizeY)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ComputeBoundsVisitor boundvisitor;
	node->accept(boundvisitor);
	osg::BoundingBox bb = boundvisitor.getBoundingBox();

	sizeX = bb.xMax() - bb.xMin();
	sizeY = bb.yMax() - bb.yMin();
	float sizeZ = bb.zMax() - bb.zMin();
	osg::Vec3f pcenter = bb.center();

	/*! 原点取在包围盒的后左下方顶点 */
	origin.set(bb.xMin(), bb.yMax(), bb.zMin());

	//绘制包围盒
	osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(new osg::Box(pcenter, sizeX, sizeY, sizeZ));

	//设置包围盒样式
	osg::StateSet *stateset = drawable->getOrCreateStateSet();
	stateset->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	stateset->setAttribute(new osg::LineWidth(3.0));
	//设置包围盒颜色
	drawable->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));

	geode->addDrawable(drawable);
	return geode.release();
}

bool isPointinPolygon(osg::Vec2f p, osg::ref_ptr<osg::Vec2Array> pPoly)
{
	float xMin = pPoly->at(3).x();//此处顶点的编号与后续本程序中网格顶点顺序有关
	float xMax = pPoly->at(1).x();
	float yMin = pPoly->at(3).y();
	float yMax = pPoly->at(1).y();
	if (xMin <= p.x() && p.x()<xMax&&yMin <= p.y() && p.y()<yMax)
		return true;
	else
		return false;
}
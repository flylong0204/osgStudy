
#include <osg/Node>
#include <osg/Group>
#include <osg/NodeVisitor>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include "EventHandler.h"
#include "Mesh2Triangle.h"
#include "../Util/AxisNode.h"

int main()
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Switch> root = new osg::Switch;
	osg::ref_ptr<osg::Node> mesh = osgDB::readNodeFile("../asserts/cow.osg");

	MeshNodeVisitor mnv;
	mesh->accept(mnv);

	//获取模型三角形信息
	osg::ref_ptr<osg::Vec3Array>pVerts = mnv.getTriPoints();
	std::vector<TriangleSelf> triangleVector;
	for (unsigned int i = 0; i<pVerts->size(); i += 3)
	{
		osg::Vec3f p0 = pVerts->at(i);
		osg::Vec3f p1 = pVerts->at(i + 1);
		osg::Vec3f p2 = pVerts->at(i + 2);
		osg::Vec3f centre = (p0 + p1 + p2) / 3.0;
		TriangleSelf _triangle(p0, p1, p2, centre);
		triangleVector.push_back(_triangle);
	}

	//调用函数createboundingbox（）和CreateGrids（），用于绘制网格
	osg::Vec3f origin;
	float sizeX;
	float sizeY;
	osg::ref_ptr<osg::Node> bbNode = createBoundingBox(mesh, origin, sizeX, sizeY);
	float xOrigin = origin.x();
	float yOrigin = origin.y();
	int rowGrid = int(std::round(sizeX));
	int colGrid = int(std::round(sizeY));
	/*! 步距 */
	float paceGrid = 1.0f;
	osg::ref_ptr<osg::Group> geode = createGrids(origin, rowGrid, colGrid, paceGrid);

	//判断三角形和网格的关系，将三角形进行按网格划分
	std::vector<Classification> classification;
	int totalGrid = rowGrid * colGrid;
	for (int i = 0; i < totalGrid; ++i)
	{
		Classification _classfi(i);
		classification.push_back(_classfi);
	}
	bool isNextTri = false;
	for (unsigned int n = 0, k = 0; n<pVerts->size(); ++k, n += 3)
	{
		for (int i = 0; i<rowGrid; ++i)
		{
			if (isNextTri)
			{
				isNextTri = false;
				break;
			}
			for (int j = 0; j<colGrid; ++j)
			{
				osg::Vec2 p(triangleVector[k]._centre.x(), triangleVector[k]._centre.y());
				osg::ref_ptr<osg::Vec2Array> pPoly = new osg::Vec2Array;
				pPoly->push_back(osg::Vec2f(xOrigin + j * paceGrid, yOrigin - i * paceGrid));
				pPoly->push_back(osg::Vec2f(xOrigin + (j + 1)*paceGrid, yOrigin - i * paceGrid));
				pPoly->push_back(osg::Vec2f(xOrigin + (j + 1)*paceGrid, yOrigin - (i + 1)*paceGrid));
				pPoly->push_back(osg::Vec2f(xOrigin + j * paceGrid, yOrigin - (i + 1)*paceGrid));
				if (isPointinPolygon(p, pPoly))
				{
					int gridIndex = i * colGrid + j;
					classification[gridIndex]._triangleselfList.push_back(triangleVector[k]);
					isNextTri = true;
					break;
				}
			}
		}
	}

	//输出每个格子的三角形（输出为osg格式）
	//for (int m = 0; m<gridNum; m++)
	//{

	//	osg::Group*triGroup = new osg::Group;
	//	for (int n = 0; n<classification[m]._triangleselfList.size(); n++)
	//	{
	//		osg::ref_ptr<osg::Vec3Array>griVertex = new osg::Vec3Array;
	//		griVertex->push_back(classification[m]._triangleselfList[n]._p0);
	//		griVertex->push_back(classification[m]._triangleselfList[n]._p1);
	//		griVertex->push_back(classification[m]._triangleselfList[n]._p2);

	//		osg::ref_ptr<osg::Geometry>triGeometry = new osg::Geometry;
	//		triGeometry->setVertexArray(griVertex);
	//		triGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));

	//		osg::ref_ptr<osg::Geode>triGeode = new osg::Geode;
	//		triGeode->addDrawable(triGeometry);
	//		triGroup->addChild(triGeode.get());
	//	}
	//	std::ostringstream oss;//创建一个流
	//	oss << m;//把值传递如流中
	//	std::string fileName = oss.str() + ".osg";
	//	osg::Node* triNode = dynamic_cast<osg::Node*>(triGroup);//在使用writeNodeFile时，节点始终是Node指针，故需要将其转换成Node
	//	osgDB::writeNodeFile(*triNode, fileName);
	//}

	root->addChild(AxisNode::createAxis());
	root->addChild(mesh);
	root->addChild(bbNode);
	root->addChild(geode.get());
	viewer->setSceneData(root);
	viewer->setUpViewInWindow(100, 100, 1024, 768);
	viewer->addEventHandler(new PickHandler(viewer, classification));
	return viewer->run();
}

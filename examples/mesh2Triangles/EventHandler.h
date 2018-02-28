
#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include "Mesh2Triangle.h"

/*!
  处理GUI事件
*/
class PickHandler : public osgGA::GUIEventHandler
{
public:
	PickHandler(osgViewer::Viewer*viewer, std::vector<Classification> classFications) :mViewer(viewer), lastselect(0)
	{
		mClassifications = classFications;
		pRoot = viewer->getSceneData()->asSwitch();
		pTriGeode = new osg::Geode();
	}//定义构造函数，有两个参数，mViewer和lastselect，分别初始化为viewer和0
	 //在handle中实现的功能有双击鼠标左键调用pick函数
	virtual bool handle(const osgGA::GUIEventAdapter&ea, osgGA::GUIActionAdapter& aa)
	{
		osg::ref_ptr<osgViewer::Viewer>viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if (viewer = NULL)
			return false;

		switch (ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::DOUBLECLICK):
		{
			if (ea.getButton() == 1)
				Pick(ea.getX(), ea.getY());
		}
		return true;
		}
		return false;
	}



private:
	//pick函数用于选择对象
	void Pick(float x, float y)
	{
		osg::ref_ptr<osg::Node>node = new osg::Node;
		osg::ref_ptr<osg::Group>parent = new osg::Group;
		/*** 进行相交测试，几种测试方法见osgUtil::LineSegmentIntersector,结果保留在lsi里面***/
		osgUtil::LineSegmentIntersector* lsi = new osgUtil::LineSegmentIntersector(osgUtil::LineSegmentIntersector::WINDOW, x, y);
		osgUtil::IntersectionVisitor ivsec(lsi);
		mViewer->getCamera()->accept(ivsec);

		if (lsi->containsIntersections())
		{
			if (!(lsi->getFirstIntersection().nodePath.empty()))
			{
				osg::NodePath np = lsi->getFirstIntersection().nodePath;
				osg::ref_ptr<osg::Geode> pGeode = dynamic_cast<osg::Geode*>(np[np.size() - 1]);
				if (pGeode.valid())
				{
					std::string gridName = pGeode->getName();
					int num = atoi(gridName.c_str());
					std::vector<TriangleSelf> triList = mClassifications[num]._triangleselfList;
					//更新三角形
					if (pTriGeode->getNumDrawables()>0)
					{
						pRoot->removeChild(pTriGeode);
						pTriGeode->removeDrawables(0, pTriGeode->getNumDrawables());
					}
					for (int j = 0; j<triList.size(); j++)
					{
						TriangleSelf tri = triList[j];
						osg::ref_ptr<osg::Geometry> pTriGeom = new osg::Geometry;
						osg::ref_ptr<osg::Vec3Array> pTriVerts = new osg::Vec3Array;
						pTriVerts->push_back(osg::Vec3(tri._p0));
						pTriVerts->push_back(osg::Vec3(tri._p1));
						pTriVerts->push_back(osg::Vec3(tri._p2));
						pTriGeom->setVertexArray(pTriVerts);
						pTriGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, pTriVerts->size()));
						pTriGeode->addDrawable(pTriGeom);
					}
					pRoot->addChild(pTriGeode);
				}
			}
		}
		return;
	}

	osgViewer::Viewer *mViewer;
	osg::Node *lastselect;
	std::vector<Classification> mClassifications;
	osg::ref_ptr<osg::Switch> pRoot;
	osg::ref_ptr<osg::Geode> pTriGeode;
};

#endif 
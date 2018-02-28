
#include "PickEventHandler.h"
#include <osg/Group>  
#include <osg/ShapeDrawable>
#include <osg/io_utils>
#include <osgDB/ReadFile>
#include <osgViewer/Renderer>
#include <osgFX/Scribe>
#include <osgUtil/LineSegmentIntersector>  
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/PlaneIntersector>
#include <osgUtil/RayIntersector>
#include <osgGA/AnimationPathManipulator>  
#include "Util.h"

float getRunTime(osg::Vec3 res, osg::Vec3 des)
{
	float xx = (res.x() - des.x())*(res.x() - des.x());
	float yy = (res.y() - des.y())*(res.y() - des.y());
	float zz = (res.z() - des.z())*(res.z() - des.z());
	float distant = sqrt(xx + yy + zz);
	return distant * 0.1;
}

PickEventHandle::PickEventHandle()
	: listPoint(new osg::Vec3Array)
	//, listOrient(new osg::QuatArray)
	, xmouse(0.0f)
	, ymouse(0.0f)
	, eSelTI(LineSegment)
	, useWCoordinate(false)
{
}

osg::AnimationPath * PickEventHandle::createPath()
{
	osg::ref_ptr<osg::AnimationPath>anim = new osg::AnimationPath;
	anim->setLoopMode(osg::AnimationPath::LOOP);

	float time = 0.0;
	float angle = 0.0;
	float roll = 1.57;//osg::inDegrees(90);  

	if (listPoint.valid())
	{
		osg::Vec3Array::iterator iter = listPoint->begin();
		if (!iter->valid())
		{
			return anim.release();
		}
		for (;;) {
			osg::Vec3 pos(*iter);
			iter++;
			if (iter != listPoint->end()) {
				if (iter->x() > pos.x())
				{
					angle = 1.57 - atan((iter->y() - pos.y()) / (iter->x() - pos.x()));
					if (angle<0)
						angle += 1.57;
				}
				else {
					angle = -(1.57 + atan((iter->y() - pos.y()) / (iter->x() - pos.x())));
					if (angle>0) {
						angle = -(1.57 - angle);
					}
				}

				//osg::Quat rotate(osg::Quat(roll, osg::Vec3(1.0, 0, 0))*osg::Quat(-angle, osg::Vec3(0, 0, 1)));
				osg::Quat rotate(osg::Quat(roll, osg::Vec3(0, 0, 1)));

				anim->insert(time, osg::AnimationPath::ControlPoint(pos, rotate));
				time += 10.0;
				//time += getRunTime(pos, *iter);
			}
			else {
				break;
			}
		}
	}


	std::ofstream out("../models/a.path");//把信息保存  
	anim->write(out);
	out.close();
	return anim.release();
}

bool PickEventHandle::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa)
{
	osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (!viewer)
	{
		return false;
	}
	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::MOVE):
	case (osgGA::GUIEventAdapter::PUSH):
		if (true)
		{
			xmouse = ea.getX();
			ymouse = ea.getY();
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::RELEASE):
		//if (true)
		if (xmouse == ea.getX() && ymouse == ea.getY())
		{
			pick(ea, viewer);
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEYDOWN):
		if (true)
		{
			// 启动漫游(可以在这保存下操纵器，漫游完毕可以返回原来状态  
			if (ea.getKey() == 'h' || ea.getKey() == 'H')
			{
				if (viewer)
				{
					osg::ref_ptr<osgGA::AnimationPathManipulator>apm = new osgGA::AnimationPathManipulator;
					apm->setAnimationPath(createPath());
					viewer->setCameraManipulator(apm);
				}
			}
			// 底视图
			else if (ea.getKey() == '1')
			{
				viewBottom(viewer);
				return true;
			}
			// 用已经保存的文件里的信息  
			else if (ea.getKey() == 'g' || ea.getKey() == 'G')
			{
				osg::ref_ptr<osgGA::AnimationPathManipulator>apm = new osgGA::AnimationPathManipulator("../models/a.path");
				viewer->setCameraManipulator(apm);
			}
			// 多面体还是线段求交
			else if (ea.getKey() == 'p' || ea.getKey() == 'P')
			{
				if (eSelTI == Ray)
				{
					eSelTI = Polytope;
					osg::notify(osg::NotifySeverity::NOTICE) << "Polytope intersect" << std::endl;
				}
				else if (eSelTI == Polytope)
				{
					eSelTI = LineSegment;
					osg::notify(osg::NotifySeverity::NOTICE) << "LineSegment intersect" << std::endl;
				}
				else if (eSelTI == LineSegment)
				{
					eSelTI = Plane;
					osg::notify(osg::NotifySeverity::NOTICE) << "Plane intersect" << std::endl;
				}
				else if (eSelTI == Plane)
				{
					eSelTI = Ray;
					osg::notify(osg::NotifySeverity::NOTICE) << "Ray Intersect" << std::endl;
				}
			}
			// 使用坐标类型
			else if (ea.getKey() == 'c' || ea.getKey() == 'C')
			{
				useWCoordinate = !useWCoordinate;
				if (useWCoordinate)
				{
					osg::notify(osg::NotifySeverity::NOTICE) << " window coordinate" << std::endl;
				}
				else
				{
					osg::notify(osg::NotifySeverity::NOTICE) << "projection coordinate" << std::endl;
				}
			}
		}
		break;
	}
	return false;
}

void PickEventHandle::pick(const osgGA::GUIEventAdapter & ea, osgViewer::Viewer * viewer)
{
	osg::Group* root = dynamic_cast<osg::Group*>(viewer->getSceneData());
	if (!root)
	{
		return;
	}
	osg::Node *node = 0;
	osg::Group *parent = 0;

	osg::Matrix vpw = viewer->getCamera()->getViewMatrix() *
		viewer->getCamera()->getProjectionMatrix() *
		viewer->getCamera()->getViewport()->computeWindowMatrix();

	if (eSelTI == Polytope)
	{
		osg::ref_ptr<osgUtil::PolytopeIntersector> picker;
		if (useWCoordinate)
		{
			osg::Viewport *viewport = viewer->getCamera()->getViewport();
			double mx = viewport->x() + (int)((double)viewport->width() * (ea.getXnormalized() * 0.5 + 0.5));
			double my = viewport->y() + (int)((double)viewport->height() * (ea.getYnormalized() * 0.5 + 0.5));
			double offsetx = 5.0f;
			double offsety = 5.0f;
			picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW,
				mx - offsetx, my - offsety, mx + offsetx, my + offsety);
		}
		else
		{
			double mx = ea.getXnormalized();
			double my = ea.getYnormalized();
			double offsetx = 0.05;
			double offsety = 0.05;
			picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::PROJECTION,
				mx - offsetx, my - offsety, mx + offsetx, my + offsety);
		}
		picker->setPrecisionHint(osgUtil::Intersector::USE_DOUBLE_CALCULATIONS);
		//picker->setPrimitiveMask(osgUtil::PolytopeIntersector::ALL_PRIMITIVES);

		osgUtil::IntersectionVisitor iv(picker.get());
		viewer->getCamera()->accept(iv);
		if (picker->containsIntersections())
		{
			osgUtil::PolytopeIntersector::Intersections listIntersection = picker->getIntersections();
			osgUtil::PolytopeIntersector::Intersections::iterator it = listIntersection.begin();
			//for (; it != listIntersection.end(); it++)
			{
				const osg::NodePath& nodePath = it->nodePath;
				/*! node为最后一个节点，倒数第二个就是它的父节点 */
				node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
				parent = (nodePath.size() >= 2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
				for (int i = 0; i < 6; i++)
				{
					//root->addChild(createBox(it->intersectionPoints[i]));
				}
			}
		}
	}
	else if (eSelTI == LineSegment)
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> picker;
		if (useWCoordinate)
		{
			osg::Viewport *viewport = viewer->getCamera()->getViewport();
			float mx = viewport->x() + (int)((float)viewport->width() * (ea.getXnormalized() * 0.5f + 0.5f));
			float my = viewport->y() + (int)((float)viewport->height() * (ea.getYnormalized() * 0.5f + 0.5f));
			picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, mx, my);
		}
		else
		{
			// use non dimensional coordinates - in projection/clip space
			picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::PROJECTION,
				ea.getXnormalized(), ea.getYnormalized());
		}
		picker->setPrecisionHint(osgUtil::Intersector::USE_DOUBLE_CALCULATIONS);

		osgUtil::IntersectionVisitor iv(picker.get());
		viewer->getCamera()->accept(iv);
		if (picker->containsIntersections())
		{
			osgUtil::LineSegmentIntersector::Intersections listIntersection = picker->getIntersections();
			osgUtil::LineSegmentIntersector::Intersections::iterator it = listIntersection.begin();
			//for (; it != listIntersection.end(); it++)
			{
				const osg::NodePath& nodePath = it->nodePath;
				node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
				parent = (nodePath.size() >= 2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
				// 世界坐标和本地坐标是一样的结果，不知道为什么
				osg::Vec3 pos = it->getWorldIntersectPoint();
				listPoint->push_back(pos);
				root->addChild(createBox(pos));
			}
		}
	}
	else if (eSelTI == Plane)
	{

	}
	else if (eSelTI == Ray)
	{

	}
}

void PickEventHandle::toggleScribe(osg::Group * parent, osg::Node * node)
{
	if (!parent || !node)
	{
		return;
	}
	osgFX::Scribe *parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent);
	if (parentAsScribe)
	{
		// 已经选中了，更改状态
		osg::Node::ParentList listParent = parentAsScribe->getParents();
		osg::Node::ParentList::iterator it = listParent.begin();
		for (; it != listParent.end(); it++)
		{
			(*it)->replaceChild(parentAsScribe, node);
		}
	}
	else
	{
		// 未选中状态，高亮
		osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe;
		scribe->addChild(node);
		parent->replaceChild(node, scribe);
	}
}

void PickEventHandle::viewBottom(osgViewer::Viewer * viewer)
{
	const osg::BoundingSphere &bs = viewer->getSceneData()->getBound();
	osg::Matrixd mat;
	mat.makeLookAt(bs.center() - osg::Vec3(0.0f, 0.0f, 3.5f * bs.radius()),
		bs.center(), osg::Vec3(0.0f, 1.0f, 0.0f));
	viewer->getCameraManipulator()->setByInverseMatrix(mat);
}

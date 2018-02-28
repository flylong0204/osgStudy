
#include "TravelManipulator.h"
#include <osgViewer/CompositeViewer>
#include <iostream>
#include <sstream>
#include <osg/io_utils>

/// 关联场景管理器
/// 
TravelManipulator *TravelManipulator::TravelToScence(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	TravelManipulator* camera = new TravelManipulator;

	viewer->setCameraManipulator(camera);

	camera->m_pHostViewer = viewer;

	return camera;
}


// 构造函数
TravelManipulator::TravelManipulator() 
	:m_fMoveSpeed(1.0f)
	, m_bLeftButtonDown(false)
	, m_fpushX(0)
	, m_fAngle(2.5)
	, m_bPeng(true)
	, m_fpushY(0)
{
	m_vPosition = osg::Vec3(-22.0f, -274.0f, 100.0f);
	m_vRotation = osg::Vec3(osg::PI_2, 0.0f, 0.0f);
}

TravelManipulator::~TravelManipulator(void)
{
}

void TravelManipulator::setByMatrix(const osg::Matrix &matrix)
{}

void TravelManipulator::setByInverseMatrix(const osg::Matrix &matrix)
{}

osg::Matrixd TravelManipulator::getMatrix(void)const
{
	osg::Matrixd mat;

	mat.makeRotate(
		m_vRotation._v[0], osg::Vec3(1.0f, 0.0f, 0.0f),
		m_vRotation._v[1], osg::Vec3(0.0f, 1.0f, 0.0f),
		m_vRotation._v[2], osg::Vec3(0.0f, 0.0f, 1.0f)
	);

	return mat * osg::Matrixd::translate(m_vPosition);

}

osg::Matrixd TravelManipulator::getInverseMatrix(void) const
{
	return osg::Matrix::inverse(getMatrix());
}

// 事件处理函数
bool TravelManipulator::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us)
{
	// 获取鼠标位置
	float mouseX = ea.getX();
	float mouseY = ea.getY();

	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
	{
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Space))
		{
			us.requestRedraw();
			us.requestContinuousUpdate(false);

			return true;
		}

		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Home))
		{
			ChangePosition(osg::Vec3(0, 0, m_fMoveSpeed));

			return true;
		}

		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_End))
		{
			ChangePosition(osg::Vec3(0, 0, -m_fMoveSpeed));

			return true;
		}

		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Plus))
		{
			m_fMoveSpeed += 1.0f;

			return true;
		}

		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Minus))
		{
			m_fMoveSpeed -= 0.1f;

			if (m_fMoveSpeed < 1.0f)
			{
				m_fMoveSpeed = 1.0f;
			}

			return true;
		}

		// 前进
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Up) ||
			ea.getKey() == int(osgGA::GUIEventAdapter::KEY_W))
		{
			ChangePosition(osg::Vec3(0, m_fMoveSpeed, 0));
			//ChangePosition(osg::Vec3(0, m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0));
			//ChangePosition(osg::Vec3(m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0, 0));

			return true;
		}

		// 后退
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Down) ||
			ea.getKey() == int(osgGA::GUIEventAdapter::KEY_S))
		{
			ChangePosition(osg::Vec3(0, -m_fMoveSpeed, 0));
			//ChangePosition(osg::Vec3(0, -m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0));
			//ChangePosition(osg::Vec3(-m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0, 0));

			return true;
		}

		// 向左
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_A))
		{
			ChangePosition(osg::Vec3(0, m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0));
			//ChangePosition(osg::Vec3(-m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0, 0));

			return true;
		}

		// 向右
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_D))
		{
			ChangePosition(osg::Vec3(0, -m_fMoveSpeed * cosf(osg::PI_2 + m_vRotation._v[2]), 0));
			//ChangePosition(osg::Vec3(m_fMoveSpeed * sinf(osg::PI_2 + m_vRotation._v[2]), 0, 0));

			return true;
		}

		// 向右转
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Right))
		{
			m_vRotation._v[2] -= osg::DegreesToRadians(m_fAngle);
		}

		// 向左转
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_Left))
		{
			m_vRotation._v[2] += osg::DegreesToRadians(m_fAngle);
		}

		// 改变屏幕角度H键
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_H))
		{
			m_fAngle -= 0.2;

			return true;
		}

		//G键
		if (ea.getKey() == int(osgGA::GUIEventAdapter::KEY_G))
		{
			m_fAngle += 0.2;

			return true;
		}

		return true;
	}

	case(osgGA::GUIEventAdapter::PUSH):
		{
			osgViewer::View* pViewer = dynamic_cast<osgViewer::View*>(&us);
			if (pViewer)
			{
				PickObject(pViewer, ea);
			}
		}
		if (ea.getButton() == 1)
		{
			m_fpushX = mouseX;
			m_fpushY = mouseY;

			m_bLeftButtonDown = true;
		}

		return true;

		// 拖动
	case(osgGA::GUIEventAdapter::DRAG):

		if (m_bLeftButtonDown)
		{
			m_vRotation._v[2] -= osg::DegreesToRadians(m_fAngle * (mouseX - m_fpushX)) / 200;
			m_vRotation._v[0] += osg::DegreesToRadians(1.1 * (mouseY - m_fpushY)) / 200;

			if (m_vRotation._v[0] >= 3.14)
			{
				m_vRotation._v[0] = 3.14;
			}

			if (m_vRotation._v[0] <= 0)
			{
				m_vRotation._v[0] = 0;
			}
		}

		return false;

		// 鼠标释放
	case(osgGA::GUIEventAdapter::RELEASE):

		if (ea.getButton() == 1)
		{
			m_bLeftButtonDown = false;
		}

		return false;

	default:

		return false;
	}

}

// 位置变换函数
void TravelManipulator::ChangePosition(osg::Vec3 delta)
{
	// 碰撞检测
	if (m_bPeng)
	{
		// 得到新的位置
		osg::Vec3 newPos1 = m_vPosition + delta;

		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
			new osgUtil::LineSegmentIntersector(newPos1, m_vPosition);
		osgUtil::IntersectionVisitor iv(intersector.get());
		m_pHostViewer->getSceneData()->accept(iv);
		if (intersector->containsIntersections())
		{
			osg::NodePath &nodePath = iv.getNodePath();
			for (int i = 0; i < nodePath.size(); i++)
			{
				osg::ref_ptr<osg::Node> node = nodePath[i];
				std::cout << " collision node name is " << node->getName() << std::endl;
			}
			//std::cout << "collision" << std::endl;
		}
		else 
		{
			m_vPosition += delta;
			//std::cout << "" << std::endl;
		}
	}
	else
	{
		m_vPosition += delta;
	}
}

void TravelManipulator::PickObject(osgViewer::View * pView, const osgGA::GUIEventAdapter & ea)
{
	osg::ref_ptr<osg::Camera> camera = pView->getCamera();
	osg::ref_ptr<osg::Group> root = dynamic_cast<osg::Group*>(m_pHostViewer->getSceneData());
	std::cout << root->getNumChildren() << std::endl;
	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (pView->computeIntersections(ea, intersections))
	{
		osgUtil::LineSegmentIntersector::Intersections::iterator it;
		std::cout << intersections.size() << std::endl;
		osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
		for (it = intersections.begin(); it != intersections.end(); it++)
		{
			std::ostringstream os;
			if (!it->nodePath.empty())
			{
				//for (int i = 0; i < it->nodePath.size(); i++)
				//{
				//	os << "the " << (i + 1) << "th node name is " <<
				//		it->nodePath[i]->getName() << std::endl;
				//}
			}
			else if (it->drawable.valid())
			{
				os << "drawable name is " << it->drawable->className() << std::endl;
			}
			os << it->getWorldIntersectPoint();
			vertex->push_back(it->getWorldIntersectPoint());
			const osgUtil::LineSegmentIntersector::Intersection::IndexList &listIdx = it->indexList;
			//for (int i = 0; i < listIdx.size(); i++)
			//{
			//	os << "vertex index " << i << " is " << listIdx[i] << std::endl;
			//}
			std::cout << os.str() << std::endl;
		}
		root->addChild(createShape(osg::PrimitiveSet::LINE_STRIP, *vertex));
	}
	//if (picker->containsIntersections())
	//{
	//	// 线段
	//	osgUtil::LineSegmentIntersector::Intersection intersection = picker->getFirstIntersection();
	//	osg::Vec3 posIntersect = intersection.getWorldIntersectPoint();
	//	osg::Vec3f eye;
	//	osg::Vec3f center;
	//	osg::Vec3f up;
	//	//const osg::BoundingSphere &bound = 
	//	camera->getViewMatrixAsLookAt(eye, center, up);
	//	//osg::Vec3 center = bound.center();
	//	std::cout << center.x() << "," << center.y() << "," << center.z() << std::endl;
	//	std::cout << posIntersect.x() << "," << posIntersect.y() << "," << posIntersect.z() << std::endl;

	//	osg::ref_ptr<osg::Group> root = dynamic_cast<osg::Group*>(m_pHostViewer->getSceneData());
	//	root->addChild(createLine(center, posIntersect));

	//	// 有交点
	//	osg::NodePath &nodePath = iv.getNodePath();
	//	std::cout << "nodepath size is " << nodePath.size() << std::endl;
	//	for (int i = 0; i < nodePath.size(); i++)
	//	{
	//		std::cout << "Pick the node name is " << nodePath[i]->getName() << std::endl;
	//	}
	//}
}

// 设置速度
void TravelManipulator::setSpeed(float &sp)
{
	m_fMoveSpeed = sp;
}

osg::ref_ptr<osg::Node> TravelManipulator::createShape(osg::PrimitiveSet::Mode mode, osg::Vec3Array & v)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::DrawElementsUInt> shape = new osg::DrawElementsUInt(mode);
	if (mode == osg::PrimitiveSet::LINE_STRIP)
	{
		for (int i = 0; i < v.size(); i++)
		{
			vertex->push_back(v[i]);
			colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			shape->push_back(i);
		}
	}
	else if (mode == osg::PrimitiveSet::TRIANGLES)
	{
		for (int i = 0; i < v.size(); i += 3)
		{
			vertex->push_back(v[i]);
			vertex->push_back(v[i + 1]);
			vertex->push_back(v[i + 2]);
			colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			shape->push_back(i);
			shape->push_back(i + 1);
			shape->push_back(i + 2);
		}
	}
	else {
		return 0;
	}
	geometry->setVertexArray(vertex.get());
	geometry->setColorArray(colors.get(), osg::Array::BIND_PER_VERTEX);
	geometry->addPrimitiveSet(shape);
	//geometry->setUseVertexArrayObject(true);

	geode->addDrawable(geometry);

	return geode.release();
}

// 获得当前速度
float TravelManipulator::getSpeed()
{
	return m_fMoveSpeed;
}

// 设置起始的位置
void TravelManipulator::SetPosition(osg::Vec3 &position)
{
	m_vPosition = position;
}

// 得到当前所在位置
osg::Vec3 TravelManipulator::GetPosition()
{
	return m_vPosition;
}
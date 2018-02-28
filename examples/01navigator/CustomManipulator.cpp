
#include "CustomManipulator.h"
#include <osg/BoundingBox>
#include <osg/ComputeBoundsVisitor>
#include <osg/io_utils>
#include <sstream>
#include <iostream>

/*!
  默认的是视点位于0，0，5，在Z轴上，距离XY平面5m，
  可根据传进来的boundingbox来设置
  把XY看成地面，视线方向为水平地面，X轴旋转90度
*/
WalkForward::WalkForward(const osg::BoundingBox & bb) 
	: vPosition(osg::Vec3(0.0f, 0.0f, 5.0f))
	, vRotation(osg::Vec3(osg::PI_2, 0.0f, 0.0f))
	, vStep(1.0f)
	, vRotateStep(0.01f)
{
	std::stringstream ss;
	const osg::Vec3 center = bb.center();
	float xSize = bb.xMax() - bb.xMin();
	float ySize = bb.yMax() - bb.yMin();
	float zSize = bb.zMax() - bb.zMin();
	float minSize = xSize < ySize ? (ySize < zSize ? ySize : zSize) : (xSize < zSize ? xSize : zSize);
	ss << xSize << "," << ySize << "," << zSize << "," << minSize << std::endl;
	vStep = minSize / 10.0f;
	vPosition[0] = center.x() + minSize * 0.5;
	vPosition[1] = center.y();
	vPosition[2] = center.z();
	ss << vPosition;
	std::cout << ss.str() << std::endl;
}

void WalkForward::setByMatrix(const osg::Matrixd & matrix)
{
	vPosition = matrix.getTrans();
	vRotation = matrix.getRotate().asVec3();
}

osg::Matrixd WalkForward::getMatrix() const
{
	osg::Matrixd mat;
	mat.makeTranslate(vPosition);
	return osg::Matrixd::rotate(vRotation[0], osg::X_AXIS,
		vRotation[1], osg::Y_AXIS,
		vRotation[2], osg::Z_AXIS) * mat;
}

void WalkForward::setByInverseMatrix(const osg::Matrixd & matrix)
{
	setByMatrix(osg::Matrixd::inverse(matrix));
}

osg::Matrixd WalkForward::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

bool WalkForward::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa)
{
	osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (viewer.valid())
	{
		switch (ea.getEventType())
		{
		case (osgGA::GUIEventAdapter::KEYDOWN):
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
			{
				/*! 向前运动 */
				return move2NextPosition(osg::Vec3(0.0f, vStep, 0.0f), viewer);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
			{
				return move2NextPosition(osg::Vec3(0.0f, -vStep, 0.0f), viewer);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
			{
				return move2NextPosition(osg::Vec3(-vStep, 0.0f, 0.0f), viewer);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
			{
				return move2NextPosition(osg::Vec3(vStep, 0.0f, 0.0f), viewer);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Up)
			{
				/*! up */
				return move2NextPosition(osg::Vec3(0.0f, 0.0f, vStep), viewer);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Down)
			{
				return move2NextPosition(osg::Vec3(0.0f, 0.0f, -vStep), viewer);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_6)
			{
				rotateByZAxis(vRotateStep);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_4)
			{
				rotateByZAxis(-vRotateStep);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_8)
			{
				// 绕X或Y旋转不成功！！！！
				rotateByYAxis(vRotateStep);
			}
			else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_KP_2)
			{
				rotateByYAxis(-vRotateStep);
			}
			break;
		default:
			break;
		}
	}
	return false;
}

bool WalkForward::move2NextPosition(osg::Vec3 delta, osgViewer::Viewer *viewer)
{
	/*! 获取包围盒 */
	osg::ComputeBoundsVisitor bv;
	viewer->getSceneData()->accept(bv);
	osg::BoundingBox &bb = bv.getBoundingBox();
	osg::Vec3 nextPos = vPosition + delta;
	//if (bb.contains(nextPos))
	{
		vPosition += delta;
	}
	return true;
}

bool WalkForward::rotateByZAxis(float delta)
{
	vRotation[2] += delta;
	return false;
}

bool WalkForward::rotateByXAxis(float delta)
{
	vRotation[0] += delta;
	return false;
}

bool WalkForward::rotateByYAxis(float delta)
{
	vRotation[1] += delta;
	return false;
}

WalkForward::~WalkForward()
{
}

#include "RotateGlider.h"
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

CRotateGlider::CRotateGlider()
{}

CRotateGlider::~CRotateGlider()
{}

bool CRotateGlider::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer *vw = dynamic_cast<osgViewer::Viewer *>(&aa);
	if (NULL != vw)
	{
		osg::MatrixTransform * mt = dynamic_cast<osg::MatrixTransform*>(vw->getSceneData());
		if (NULL != mt)
		{
			static double ih = 0;//水平转过的角度,弧度
			static double iv = 0;//垂直转过的角度。弧度
			switch (ea.getEventType())
			{
			case osgGA::GUIEventAdapter::KEYDOWN:
			{
				//左右移动的角度
				if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
				{
					ih -= 0.02;
				}
				else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
				{
					ih += 0.02;
				}

				// 上下当前移动的角度,上下最大不超过90度
				if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
				{
					iv -= 0.2;
					if (iv <= (0 - osg::PI_2))
					{
						iv = 0 - osg::PI_2;
					}
				}
				else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
				{
					iv += 0.2;
					if (iv >= osg::PI_2)
					{
						iv = osg::PI_2;
					}
				}

				// osg::Quat(ih, osg::Vec3(0.0, 0.0, 1.0))为物体绕着Z轴转多少，
				// osg::Quat(iv, osg::Vec3(1.0, 0.0, 0.0))为物体绕着X轴转多少
				// 物体绕着Z轴转，相当于视角左右旋转，由于视口是向着正Y方向，所以物体绕着X轴转相当于视口上线旋转
				mt->setMatrix(osg::Matrix::rotate(osg::Quat(ih, osg::Vec3(0.0, 0.0, 1.0))*osg::Quat(iv, osg::Vec3(1.0, 0.0, 0.0))));
			}
			break;
			default:
				break;
			}
		}

	}
	return false;
}
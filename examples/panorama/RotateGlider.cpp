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
			static double ih = 0;//ˮƽת���ĽǶ�,����
			static double iv = 0;//��ֱת���ĽǶȡ�����
			switch (ea.getEventType())
			{
			case osgGA::GUIEventAdapter::KEYDOWN:
			{
				//�����ƶ��ĽǶ�
				if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
				{
					ih -= 0.02;
				}
				else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
				{
					ih += 0.02;
				}

				// ���µ�ǰ�ƶ��ĽǶ�,������󲻳���90��
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

				// osg::Quat(ih, osg::Vec3(0.0, 0.0, 1.0))Ϊ��������Z��ת���٣�
				// osg::Quat(iv, osg::Vec3(1.0, 0.0, 0.0))Ϊ��������X��ת����
				// ��������Z��ת���൱���ӽ�������ת�������ӿ���������Y����������������X��ת�൱���ӿ�������ת
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
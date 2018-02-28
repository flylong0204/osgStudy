#ifndef __CHUD_PICK_HANDLER_H_
#define __CHUD_PICK_HANDLER_H_

#include <iostream>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osg/Array>
#include <osg/Geode>

// ��Ҫ�ǵ��������ʱ��ʵ���ض������ת
class CHUDPickHandlerSingle : public osgGA::GUIEventHandler
{
public:
	CHUDPickHandlerSingle() : m_pTrackballManipulator(new osgGA::TrackballManipulator) {}

	// ������������
	void createSceneNode(osg::ref_ptr<osgViewer::Viewer> _viewer, osg::ref_ptr<osg::Group> _root);

	// �����ӿ�λ��
	void setHudViewPositionAndCubeWidth(double _x, double _y, double _width, double _height, float _cubeWidth = 80.0f);

private:
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
		osg::Object*, osg::NodeVisitor*);

	// ���������屻�������
	virtual void pick(osgViewer::Viewer* view, const osgGA::GUIEventAdapter& ea);

	// ��ʼ������������Ķ�����Ϣ��������Ϣ
	void initVertexAndNormalData();

	// ���ö�����Ϣ��������Ϣ����һ������������
	osg::Geode* createCustomCubeGeometry();

private:
	enum CUBE_FACE
	{
		CUBE_FRONT = 0,
		CUBE_BACK,
		CUBE_UP,
		CUBE_DOWN,
		CUBE_LEFT,
		CUBE_RIGHT
	};

	// ������Ķ�����Ϣ
	osg::ref_ptr<osg::Vec3Array>                    m_vec3VerCoords;
	// ������ÿ�����������Ϣ
	osg::ref_ptr<osg::Vec2Array>                    m_vec2TexCoords;
	// ÿ��������Ҫ�Ķ������ ��2��ʾm_vec3VerCoords[2] �������
	osg::ref_ptr<osg::Vec4iArray>                   m_vec4FaceVertexs;
	// ׷������ͼ������
	osg::ref_ptr<osgGA::TrackballManipulator>       m_pTrackballManipulator;
	// ���ǵ�Hud���������ʵʱ��ת����
	osg::ref_ptr<osg::Camera>                       m_pHudCamera;
	osg::ref_ptr<osg::Viewport>                     m_pViewport;
	float                                           m_cubeWidth;
};

#endif //__CHUD_PICK_HANDLER_H_
//// CHUDPickHandler.h
#ifndef __CHUD_PICK_HANDLER_H_
#define __CHUD_PICK_HANDLER_H_

#include <iostream>
#include <osgViewer/View>
#include "MyTrackballManipulator.h"

// ���ڱ�ʾ�������ͼ
static      bool        g_MasterViewClicked = true;
// ͨ������������ͼ
static const std::string MASTER_VIEW_NAME = "MASTERVIEW";
static const std::string SLAVE_VIEW_NAME = "SLAVEVIEW";
// ȫ���������Ķ���
static MyTrackballManipulator*myTrackballManipulator = nullptr;
static osgGA::TrackballManipulator* trackballManipulator = nullptr;

class CHUDPickHandler : public osgGA::GUIEventHandler
{
public:
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void initVertexAndNormalData();
	void callHudControllerModule();

	osg::Group* createCustomCubeGeometry();

private:
	osg::Vec3Array*     m_vec3VerCoords;
	osg::Vec2Array*     m_vec2TexCoords;
	osg::Vec3Array*     m_vec3Normalize;
	osg::Vec4iArray*    m_vec4FaceVertexs;
};

#endif //__CHUD_PICK_HANDLER_H_
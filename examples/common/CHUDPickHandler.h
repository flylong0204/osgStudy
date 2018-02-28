//// CHUDPickHandler.h
#ifndef __CHUD_PICK_HANDLER_H_
#define __CHUD_PICK_HANDLER_H_

#include <iostream>
#include <osgViewer/View>
#include "MyTrackballManipulator.h"

// 用于标示点击的视图
static      bool        g_MasterViewClicked = true;
// 通过名字区分视图
static const std::string MASTER_VIEW_NAME = "MASTERVIEW";
static const std::string SLAVE_VIEW_NAME = "SLAVEVIEW";
// 全局漫游器的定义
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
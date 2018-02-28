#ifndef __CHUD_PICK_HANDLER_H_
#define __CHUD_PICK_HANDLER_H_

#include <iostream>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osg/Array>
#include <osg/Geode>

// 主要是点击导航器时，实现特定面的旋转
class CHUDPickHandlerSingle : public osgGA::GUIEventHandler
{
public:
	CHUDPickHandlerSingle() : m_pTrackballManipulator(new osgGA::TrackballManipulator) {}

	// 创建场景构建
	void createSceneNode(osg::ref_ptr<osgViewer::Viewer> _viewer, osg::ref_ptr<osg::Group> _root);

	// 设置视口位置
	void setHudViewPositionAndCubeWidth(double _x, double _y, double _width, double _height, float _cubeWidth = 80.0f);

private:
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
		osg::Object*, osg::NodeVisitor*);

	// 处理立方体被点击的面
	virtual void pick(osgViewer::Viewer* view, const osgGA::GUIEventAdapter& ea);

	// 初始化导航立方体的顶点信息和纹理信息
	void initVertexAndNormalData();

	// 利用顶点信息和纹理信息创建一个导航立方体
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

	// 立方体的顶点信息
	osg::ref_ptr<osg::Vec3Array>                    m_vec3VerCoords;
	// 立方体每个面的纹理信息
	osg::ref_ptr<osg::Vec2Array>                    m_vec2TexCoords;
	// 每个面所需要的顶点序号 如2表示m_vec3VerCoords[2] 这个顶点
	osg::ref_ptr<osg::Vec4iArray>                   m_vec4FaceVertexs;
	// 追踪主视图漫游器
	osg::ref_ptr<osgGA::TrackballManipulator>       m_pTrackballManipulator;
	// 我们的Hud相机，便于实时旋转操作
	osg::ref_ptr<osg::Camera>                       m_pHudCamera;
	osg::ref_ptr<osg::Viewport>                     m_pViewport;
	float                                           m_cubeWidth;
};

#endif //__CHUD_PICK_HANDLER_H_
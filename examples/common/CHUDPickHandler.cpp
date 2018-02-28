
#include "CHUDPickHandler.h"
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/CompositeViewer>
#include <osgUtil/Optimizer>

bool CHUDPickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (view == nullptr)
	{
		return false;
	}

	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::DOUBLECLICK):
	{
		pick(view, ea);
		return false;
	}
	case(osgGA::GUIEventAdapter::PUSH):
	{
		// 用于标示点击的视图，便于对用的漫游器操作
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
		if (view)
		{
			if (view->getName() == MASTER_VIEW_NAME)
			{
				g_MasterViewClicked = true;
			}
			else
			{
				g_MasterViewClicked = false;
			}
		}
		return false;
	}
	case(osgGA::GUIEventAdapter::FRAME):
	{
		if (g_MasterViewClicked)
		{
			myTrackballManipulator->setRotation(trackballManipulator->getRotation());
		}
		else
		{
			trackballManipulator->setRotation(myTrackballManipulator->getRotation());
		}

		return false;
	}
	default:
		return false;
	}
}

// 我们只对导航立方体进行操作
void CHUDPickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& evt)
{
	if (view->getName() == MASTER_VIEW_NAME)
	{
		return;
	}

	osg::Group* group = nullptr;
	osg::Geometry* geom = nullptr;

	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (view->computeIntersections(evt, intersections))
	{
		osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
		const osg::NodePath& nodePath = intersection.nodePath;
		int len = (int)nodePath.size();

		geom = (len >= 1) ? dynamic_cast<osg::Geometry*>(nodePath[len - 1]) : nullptr;
		group = (len >= 2) ? dynamic_cast<osg::Group*>(nodePath[len - 2]) : nullptr;
	}

	if (geom && group)
	{
		const std::string facename = geom->getName();
		std::cout << " face name is " << facename << std::endl;

		int index = ::atoi(facename.c_str());
		double degree = 0.0f;
		double zerodegree = 0.0f;
		osg::Vec3f v3;
		switch (index)
		{
		case 0:
		{
			myTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0f), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case 5:
		{
			myTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0f), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(90.0f), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case 1:
		{
			myTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(180.0f), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case 4:
		{
			myTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(270.0f), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case 2:
		{
			myTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(zerodegree), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case 3:
		{
			myTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(180.0), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		default:
			break;
		}
	}
}

// 这里主要是初始化立方体的顶点信息，纹理坐标信息，还有需要的顶点生成面
void CHUDPickHandler::initVertexAndNormalData()
{
	m_vec3VerCoords = new osg::Vec3Array(8);
	(*m_vec3VerCoords)[0] = osg::Vec3(10.0f, 10.0f, 10.0f);
	(*m_vec3VerCoords)[1] = osg::Vec3(-10.0f, 10.0f, 10.0f);
	(*m_vec3VerCoords)[2] = osg::Vec3(10.0f, -10.0f, 10.0f);
	(*m_vec3VerCoords)[3] = osg::Vec3(-10.0f, -10.0f, 10.0f);
	(*m_vec3VerCoords)[4] = osg::Vec3(10.0f, 10.0f, -10.0f);
	(*m_vec3VerCoords)[5] = osg::Vec3(-10.0f, 10.0f, -10.0f);
	(*m_vec3VerCoords)[6] = osg::Vec3(10.0f, -10.0f, -10.0f);
	(*m_vec3VerCoords)[7] = osg::Vec3(-10.0f, -10.0f, -10.0f);

	m_vec2TexCoords = new osg::Vec2Array(4);
	(*m_vec2TexCoords)[0] = osg::Vec2(0.0f, 0.0f);
	(*m_vec2TexCoords)[1] = osg::Vec2(1.0f, 0.0f);
	(*m_vec2TexCoords)[2] = osg::Vec2(1.0f, 1.0f);
	(*m_vec2TexCoords)[3] = osg::Vec2(0.0f, 1.0f);

	m_vec3Normalize = new osg::Vec3Array(6);

	m_vec4FaceVertexs = new osg::Vec4iArray(6);
	(*m_vec4FaceVertexs)[0] = osg::Vec4i(7, 6, 2, 3);
	(*m_vec4FaceVertexs)[1] = osg::Vec4i(4, 5, 1, 0);
	(*m_vec4FaceVertexs)[2] = osg::Vec4i(3, 2, 0, 1);
	(*m_vec4FaceVertexs)[3] = osg::Vec4i(5, 4, 6, 7);
	(*m_vec4FaceVertexs)[4] = osg::Vec4i(5, 7, 3, 1);
	(*m_vec4FaceVertexs)[5] = osg::Vec4i(6, 4, 0, 2);
}

osg::Group* CHUDPickHandler::createCustomCubeGeometry()
{
	// 初始化立方体信息
	initVertexAndNormalData();

	//
	osg::Group* root = new osg::Group;
	root->setName("root");

	char imagePath[64];
	char faceName[16];
	for (int i = 0; i < 6; i++)
	{
		sprintf(imagePath, "../texture/side%d.png", i + 1);
		sprintf(faceName, "%d", i);
		osg::Image* image = osgDB::readImageFile(imagePath);
		osg::Geometry* geom = new osg::Geometry;
		//   一个面的四个顶点
		osg::Vec3Array* coords = new osg::Vec3Array(4);
		osg::Vec4i v4 = (*m_vec4FaceVertexs)[i];
		(*coords)[0] = (*m_vec3VerCoords)[v4.x()];
		(*coords)[1] = (*m_vec3VerCoords)[v4.y()];
		(*coords)[2] = (*m_vec3VerCoords)[v4.z()];
		(*coords)[3] = (*m_vec3VerCoords)[v4.w()];
		geom->setVertexArray(coords);
		// 面法线
		osg::Vec3Array* norm = new osg::Vec3Array(1);
		(*norm)[0] = ((*coords)[0] - (*coords)[1]) ^ ((*coords)[1] - (*coords)[2]);
		(*norm)[0].normalize();
		(*m_vec3Normalize)[i] = (*norm)[0];
		geom->setNormalArray(norm, osg::Array::BIND_OVERALL);
		// 纹理坐标
		osg::Vec2Array* tcoords = new osg::Vec2Array(4);
		(*tcoords)[0] = osg::Vec2(0.0f, 0.0f);
		(*tcoords)[1] = osg::Vec2(1.0f, 0.0f);
		(*tcoords)[2] = osg::Vec2(1.0f, 1.0f);
		(*tcoords)[3] = osg::Vec2(0.0f, 1.0f);
		geom->setTexCoordArray(0, tcoords);
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, coords->size()));
		geom->setName(faceName);

		// 设置纹理状态
		if (image)
		{
			osg::StateSet* stateset = new osg::StateSet;
			osg::Texture2D* texture = new osg::Texture2D;
			texture->setImage(image);
			stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
			geom->setStateSet(stateset);
		}

		root->addChild(geom);
	}

	return root;
}

void CHUDPickHandler::callHudControllerModule()
{
	// 通过视图管理器管理视图
	osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer;

	//
	osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile("../models/cow.osg");

	//
	osgUtil::Optimizer op;
	op.optimize(cow);

	// 窗口信息
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();
	traits->x = 100;
	traits->y = 100;
	traits->width = 1200;
	traits->height = 900;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	//
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	gc->setClearColor(osg::Vec4(0.2, 0.2, 0.2, 0.3f));
	gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//
	{
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		view->setName(MASTER_VIEW_NAME);
		viewer->addView(view);

		view->setSceneData(cow);
		view->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
		view->getCamera()->setGraphicsContext(gc);

		trackballManipulator = new osgGA::TrackballManipulator;
		view->setCameraManipulator(trackballManipulator);

		// 以下貌似么有用，不确定
		//osg::ref_ptr<CHUDPickHandler> chudPicker = new CHUDPickHandler();
		//view->addEventHandler(chudPicker);
	}

	{
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
		view->setName(SLAVE_VIEW_NAME);
		viewer->addView(view);

		myTrackballManipulator = new MyTrackballManipulator;
		view->setCameraManipulator(myTrackballManipulator);
		osg::ref_ptr<CHUDPickHandler> chudPicker = new CHUDPickHandler();
		view->addEventHandler(chudPicker);

		// 生成自定义几何体
		osg::Group* group = chudPicker->createCustomCubeGeometry();
		view->setSceneData(group);
		view->getCamera()->setViewport(new osg::Viewport(traits->width * 0.8f, traits->height * 0.8f,
			traits->width * 0.2f, traits->height * 0.2f));
		view->getCamera()->setGraphicsContext(gc);
		view->getCamera()->setRenderOrder(osg::Camera::RenderOrder::POST_RENDER);
	}

	viewer->realize();
	viewer->run();
}
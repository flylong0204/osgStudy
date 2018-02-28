
#include "CHUDPickHandlerSingle.h"
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/BlendColor>
#include <osg/BlendFunc>

// 处理输入事件
bool CHUDPickHandlerSingle::handle(const osgGA::GUIEventAdapter& ea,
	osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (viewer == nullptr)
	{
		return false;
	}

	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH):
	{
		// Hud视图点击时进行旋转
		pick(viewer, ea);
		return false;
	}
	case(osgGA::GUIEventAdapter::FRAME):
	{
		// 保证HUD视图可以随主视图旋转
		if (m_pTrackballManipulator && m_pHudCamera)
		{
			osg::Vec3d eye, up, center;
			m_pTrackballManipulator->getTransformation(eye, center, up);
			osg::Vec3 delt = center - eye;
			delt.normalize();

			/* 整个实现逻辑的核心
			保证HUD相机方向和漫游器方向相同 */
			m_pHudCamera->setViewMatrixAsLookAt(
				delt * (-1000.0) * this->m_cubeWidth,
				osg::Vec3(0, 0, 0),
				up);
		}
		return false;
	}

	default:
		return false;
	}
}

void CHUDPickHandlerSingle::pick(osgViewer::Viewer* viewer, const osgGA::GUIEventAdapter& ea)
{
	// 检测碰撞
	osg::ref_ptr<osg::Drawable> drawable = nullptr;
	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (viewer->computeIntersections(ea, intersections))
	{
		// 只取可绘制的几何节点 因为我们检测的是几何形碰撞
		osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
		drawable = intersection.drawable;
	}

	if (drawable)
	{
		const std::string facename = drawable->getName();
		std::cout << "facename is " << facename << std::endl;
		if (facename == "") { return; }

		int index = ::atoi(facename.c_str());
		double zerodegree = 0.0f;
		osg::Vec3f v3;

		// 这里我们操作漫游器进行旋转 因为帧循环时我们的Hud相机可以立刻跟随漫游器进行旋转
		switch (index)
		{
		case CUBE_FRONT: // 前
		{
			m_pTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0f), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case CUBE_BACK: // 后
		{
			m_pTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(180.0f), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case CUBE_UP: // 上
		{
			m_pTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(zerodegree), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case CUBE_DOWN: // 下
		{
			m_pTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(180.0), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case CUBE_LEFT: // 左
		{
			m_pTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0f), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(270.0f), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		case CUBE_RIGHT: // 右
		{
			m_pTrackballManipulator->setRotation(osg::Quat(
				osg::inDegrees(90.0), osg::Vec3(1.0f, zerodegree, zerodegree),
				osg::inDegrees(zerodegree), osg::Vec3(zerodegree, 1.0f, zerodegree),
				osg::inDegrees(90.0f), osg::Vec3(zerodegree, zerodegree, 1.0f)
			));
		}
		break;
		default:
			break;
		}
	}
}

// 暴露接口 设置Hud视口的位置和大小 以及我们导航立方体的大小
void CHUDPickHandlerSingle::setHudViewPositionAndCubeWidth(double _x, double _y, double _width, double _height, float _cubeWidth)
{
	m_pViewport = new osg::Viewport(_x, _y, _width, _height);
	m_cubeWidth = _cubeWidth;
}

// 创建场景信息
void CHUDPickHandlerSingle::createSceneNode(osg::ref_ptr<osgViewer::Viewer> _viewer, osg::ref_ptr<osg::Group> _root)
{
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	unsigned int width, height;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();
	traits->x = 0;
	traits->y = 0;
	traits->width = width;
	traits->height = height;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	//
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	// 操作主相机
	osg::Camera* masterCamera = _viewer->getCamera();
	masterCamera->setGraphicsContext(gc);
	masterCamera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	masterCamera->setDrawBuffer(GL_BACK);
	masterCamera->setReadBuffer(GL_BACK);

	// 设置从属相机
	m_pHudCamera = new osg::Camera;
	m_pHudCamera->setGraphicsContext(gc);
	m_pHudCamera->setAllowEventFocus(true);
	// 投影矩阵的大小范围
	m_pHudCamera->setProjectionMatrixAsOrtho2D(
		-1.0f * traits->height * 0.2f,
		traits->height * 0.2f,
		-1.0f * traits->height * 0.2f,
		traits->height * 0.2f);

	m_pHudCamera->setDrawBuffer(GL_BACK);
	m_pHudCamera->setReadBuffer(GL_BACK);
	m_pHudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	m_pHudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_pHudCamera->addChild(createCustomCubeGeometry());

	// 将相机固定位置
	m_pHudCamera->setViewMatrixAsLookAt(
		osg::Vec3(0, -1000 * m_cubeWidth, 0),
		osg::Vec3(0, 0, 0),
		osg::Vec3(0, 0, 1)
	);

	// 设置HUD视图位置
	m_pHudCamera->setViewport(m_pViewport);

	// 这里使用addChild时，只能旋转导航器，导航器不响应点击事件 还在找原因 addSlave没问题
	_viewer->addSlave(m_pHudCamera, false);
	//_root->addChild(m_pHudCamera);

	// 旋转操作
	_viewer->setCameraManipulator(m_pTrackballManipulator);

	// 点击事件
	_viewer->addEventHandler(this);

}

// 初始化我们绘制立方体所需要的顶点 纹理坐标 以及决定一个面所需要的顶点下标
void CHUDPickHandlerSingle::initVertexAndNormalData()
{
	// 立方体的顶点信息
	m_vec3VerCoords = new osg::Vec3Array(8);
	(*m_vec3VerCoords)[0] = osg::Vec3(m_cubeWidth, m_cubeWidth, m_cubeWidth);
	(*m_vec3VerCoords)[1] = osg::Vec3(-1.0f * m_cubeWidth, m_cubeWidth, m_cubeWidth);
	(*m_vec3VerCoords)[2] = osg::Vec3(m_cubeWidth, -1.0f * m_cubeWidth, m_cubeWidth);
	(*m_vec3VerCoords)[3] = osg::Vec3(-1.0f * m_cubeWidth, -1.0f * m_cubeWidth, m_cubeWidth);
	(*m_vec3VerCoords)[4] = osg::Vec3(m_cubeWidth, m_cubeWidth, -1.0f * m_cubeWidth);
	(*m_vec3VerCoords)[5] = osg::Vec3(-1.0f * m_cubeWidth, m_cubeWidth, -1.0f * m_cubeWidth);
	(*m_vec3VerCoords)[6] = osg::Vec3(m_cubeWidth, -1.0f * m_cubeWidth, -1.0f * m_cubeWidth);
	(*m_vec3VerCoords)[7] = osg::Vec3(-1.0f * m_cubeWidth, -1.0f * m_cubeWidth, -1.0f * m_cubeWidth);

	// 立方体的纹理坐标
	m_vec2TexCoords = new osg::Vec2Array(4);
	(*m_vec2TexCoords)[0] = osg::Vec2(0.0f, 0.0f);
	(*m_vec2TexCoords)[1] = osg::Vec2(1.0f, 0.0f);
	(*m_vec2TexCoords)[2] = osg::Vec2(1.0f, 1.0f);
	(*m_vec2TexCoords)[3] = osg::Vec2(0.0f, 1.0f);

	// 组成面的顶点
	m_vec4FaceVertexs = new osg::Vec4iArray(6);
	(*m_vec4FaceVertexs)[0] = osg::Vec4i(7, 6, 2, 3);
	(*m_vec4FaceVertexs)[1] = osg::Vec4i(4, 5, 1, 0);
	(*m_vec4FaceVertexs)[2] = osg::Vec4i(3, 2, 0, 1);
	(*m_vec4FaceVertexs)[3] = osg::Vec4i(5, 4, 6, 7);
	(*m_vec4FaceVertexs)[4] = osg::Vec4i(5, 7, 3, 1);
	(*m_vec4FaceVertexs)[5] = osg::Vec4i(6, 4, 0, 2);
}

// 创建导航器立方体
osg::Geode* CHUDPickHandlerSingle::createCustomCubeGeometry()
{
	initVertexAndNormalData();

	//
	osg::Geode* geode = new osg::Geode;
	char imagePath[64];
	char faceName[16];
	for (int i = 0; i < 6; i++)
	{
		sprintf(imagePath, "../textures/side%d.png", i + 1);
		sprintf(faceName, "%d", i);
		osg::Image* image = osgDB::readImageFile(imagePath);
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

		// 顶点
		osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array(4);
		osg::Vec4i v4 = (*m_vec4FaceVertexs)[i];
		(*coords)[0] = (*m_vec3VerCoords)[v4.x()];
		(*coords)[1] = (*m_vec3VerCoords)[v4.y()];
		(*coords)[2] = (*m_vec3VerCoords)[v4.z()];
		(*coords)[3] = (*m_vec3VerCoords)[v4.w()];
		geom->setVertexArray(coords);

		// 法线
		osg::ref_ptr<osg::Vec3Array> norm = new osg::Vec3Array(1);
		(*norm)[0] = ((*coords)[0] - (*coords)[1]) ^ ((*coords)[1] - (*coords)[2]);
		(*norm)[0].normalize();
		geom->setNormalArray(norm, osg::Array::BIND_OVERALL);

		// 纹理坐标
		osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array(4);
		(*tcoords)[0] = osg::Vec2(0.0f, 0.0f);
		(*tcoords)[1] = osg::Vec2(1.0f, 0.0f);
		(*tcoords)[2] = osg::Vec2(1.0f, 1.0f);
		(*tcoords)[3] = osg::Vec2(0.0f, 1.0f);
		geom->setTexCoordArray(0, tcoords);
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, coords->size()));
		geom->setName(faceName);

		if (image)
		{
			osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
			texture->setImage(image);
			stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
			geom->setStateSet(stateset);
		}

		geode->addChild(geom);
	}

	// 创建一个纹理地图
	{
		osg::Image* image = osgDB::readImageFile("../textures/plate.PNG");
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

		// 顶点
		osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array(4);
		float planeWidth = 1.5f * m_cubeWidth;
		(*coords)[0] = osg::Vec3(-1.0f * planeWidth, planeWidth, -1.01f * m_cubeWidth);
		(*coords)[1] = osg::Vec3(planeWidth, planeWidth, -1.01f * m_cubeWidth);
		(*coords)[2] = osg::Vec3(planeWidth, -1.0f * planeWidth, -1.01f * m_cubeWidth);
		(*coords)[3] = osg::Vec3(-1.0f * planeWidth, -1.0f * planeWidth, -1.01f * m_cubeWidth);
		geom->setVertexArray(coords);

		// 法线
		osg::ref_ptr<osg::Vec3Array> norm = new osg::Vec3Array(1);
		(*norm)[0] = ((*coords)[1] - (*coords)[0]) ^ ((*coords)[2] - (*coords)[1]);
		(*norm)[0].normalize();
		geom->setNormalArray(norm, osg::Array::BIND_OVERALL);

		// 纹理坐标
		osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array(4);
		(*tcoords)[0] = osg::Vec2(0.0f, 0.0f);
		(*tcoords)[1] = osg::Vec2(1.0f, 0.0f);
		(*tcoords)[2] = osg::Vec2(1.0f, 1.0f);
		(*tcoords)[3] = osg::Vec2(0.0f, 1.0f);
		geom->setTexCoordArray(0, tcoords);
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, coords->size()));
		geom->setName("3");
		if (image)
		{
			osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
			osg::ref_ptr<osg::BlendColor> bc = new osg::BlendColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));
			osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc();

			texture->setImage(image);

			// 添加纹理效果
			stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
			stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			stateset->setAttributeAndModes(bf, osg::StateAttribute::ON);
			stateset->setAttributeAndModes(bc, osg::StateAttribute::ON);

			bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
			bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
			bc->setConstantColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.5f));

			geom->setStateSet(stateset);
		}
		geode->addChild(geom);
	}

	return geode;
}
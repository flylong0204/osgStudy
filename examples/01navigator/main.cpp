/*!
  漫游器 真的普通模型的导航

*/

#include "PickEventHandler.h"
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osg/io_utils>
#include <osg/ComputeBoundsVisitor>
#include "Util.h"
#include "CustomManipulator.h"
#include "../../Util/SupportedOGLVersion.h"
#include "../../Util/Axis.h"

int main(int argc, char *argv[])
{
	osg::ref_ptr<osgViewer::Viewer>viewer = new osgViewer::Viewer;

	osg::Camera* camera = viewer->getCamera();
	//viewer->getCamera()->setClearColor(osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
	
	// 设置图形上下文特性
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 100;
	traits->y = 100;
	traits->width = 1024;
	traits->height = 768;
	traits->windowDecoration = true; // 窗口扩展
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->red = 8;
	traits->green = 8;
	traits->blue = 8;
	traits->alpha = 8;
	traits->samples = 16;
	traits->sampleBuffers = 1;
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	camera->setGraphicsContext(gc.get());
	camera->setViewport(0, 0, 1024, 768);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(Util::Axis::createAxis(1.0f, 1.0f, 1.0f));
	osg::ref_ptr<osg::Node> fileNode = osgDB::readNodeFile(
		"..\\models\\ceep.ive"
		//"..\\models\\town.osgt"
	);
	osg::ComputeBoundsVisitor bv;
	fileNode->accept(bv);
	const osg::BoundingBox &bbs = bv.getBoundingBox();
	//std::stringstream ss;
	//for (int i = 0; i < 8; i++)
	//{
	//	ss << bbs.corner(i) << ",";
	//}
	//std::cout << ss.str() << std::endl;
    // 设置属性	
	osg::StateSet *stateSet = fileNode->getOrCreateStateSet();
	//stateSet->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	if (true) 
	{
		stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	else 
	{
		stateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		stateSet->setMode(GL_LIGHT0, osg::StateAttribute::ON);
		// 场景灯光
		osg::ref_ptr<osg::Light> light = new osg::Light;
		light->setLightNum(0);
		light->setDirection(osg::Vec3(0.0f, 0.0f, 5.0f));
		light->setPosition(osg::Vec4(
			bbs.center().x(),
			bbs.center().y(),
			bbs.center().z(), 1.0f));
		light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		light->setConstantAttenuation(1.0f);
		light->setLinearAttenuation(0.0f);
		light->setQuadraticAttenuation(0.0f);
		// 创建光源
		osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource;
		lightSource->setLight(light.get());
		root->addChild(lightSource);
	}
	root->addChild(fileNode);
	viewer->setSceneData(root.get());

	viewer->addEventHandler(new osgViewer::StatsHandler());
	viewer->addEventHandler(new osgViewer::WindowSizeHandler());
	viewer->addEventHandler(new PickEventHandle());
	viewer->addEventHandler(new osgViewer::ScreenCaptureHandler);

	osg::ref_ptr<SupportedOpenGLVersion> testOGLVersion = new SupportedOpenGLVersion;
	viewer->setRealizeOperation(testOGLVersion.get());
	viewer->realize();
	if (testOGLVersion->mSupported)
	{
		std::cout << "OpenGL Version " << testOGLVersion->mVersion << std::endl;
	}
	else
	{
		std::cout << testOGLVersion->mMessage << std::endl;
	}

	viewer->setCameraManipulator(new WalkForward(bbs));
	viewer->run();

	return 0;
}
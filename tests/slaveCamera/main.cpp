
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/GraphicsContext>
#include <iostream>

void addWindow(osgViewer::Viewer &viewer, int x, int y, int width, int height,
	float xTranslate, float yTranslate)
{
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = x;
	traits->y = y;
	traits->width = width;
	traits->height = height;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(gc.get());
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	/*!
	  新建的camera需要设置一下，如果使用RTT时可能会出错，因为会读取ReadBuffer，
	  默认是没有设置的，
	*/
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);

	viewer.addSlave(camera.get(), osg::Matrix::translate(xTranslate, yTranslate, 0), osg::Matrix());
}

int main(int argc, char** argv)
{

	osgViewer::Viewer viewer;

	addWindow(viewer, 100, 100, 800, 640, 50, 50);
	addWindow(viewer, 100, 100, 800, 640, 100, 100);

	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.setUpViewInWindow(100, 100, 1024, 768);
	viewer.realize();

	viewer.run();

	return 0;
}
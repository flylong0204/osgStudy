
/*!
   创建子视景器
*/
#ifndef MEIJIE_UTIL_H
#define MEIJIE_UTIL_H 1

#include <osg/Node>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/CompositeViewer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/CameraManipulator>

class Util
{
public:
	static void createView(osgViewer::CompositeViewer *viewer, osg::ref_ptr<osg::Group> &root, osg::ref_ptr<osg::GraphicsContext> &gc,
		osgGA::CameraManipulator *manipulator, int x, int y, int width, int height)
	{
		double left;
		double right;
		double top;
		double bottom;
		double near;
		double far;
		double aspectRatio;
		double frusht;
		double fruswid;
		double fudge;

		osgViewer::View *view = new osgViewer::View;

		viewer->addView(view);
		view->setCameraManipulator(manipulator);
		view->setSceneData(root.get());
		view->getCamera()->setViewport(new osg::Viewport(x, y, width, height));

		view->getCamera()->getProjectionMatrixAsFrustum(
			left, right, bottom, top, near, far);
		if (true)
		{
			aspectRatio = width / height;
			frusht = top - bottom;
			fruswid = right - left;
			fudge = frusht * aspectRatio / fruswid;
			right = right * fudge;
			left = left * fudge;
			view->getCamera()->setProjectionMatrixAsFrustum(left, right,
				bottom, top, near, far);
		}

		//
		view->getCamera()->setGraphicsContext(gc.get());

		view->addEventHandler(new osgViewer::StatsHandler);
		view->addEventHandler(new osgViewer::WindowSizeHandler);
	}

};

#endif // !MEIJIE_UTIL_H
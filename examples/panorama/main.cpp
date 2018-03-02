
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include "RotateGlider.h"
#include <atomic>

/*!
  这里的全景器很假，使用一个球体来模拟，让人在球中心，水平方向旋转还算可以，
  上下看时就会发现球的两极点，封边也有瑕疵
*/

osg::ref_ptr<osg::Node> createSphere()
{
	osg::ref_ptr<osg::Group> root = new osg::Group;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 5.0f)));

	//给球体贴图
	osg::ref_ptr<osg::Texture2D> tex2D = new osg::Texture2D;
	tex2D->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("../textures/panorama-NSW-Australia.jpg");
	tex2D->setImage(image.get());

	osg::ref_ptr<osg::TexEnv> texEnv = new osg::TexEnv;
	texEnv->setMode(osg::TexEnv::DECAL);
	osg::StateSet* stateSet = geode->getOrCreateStateSet();
	stateSet->setTextureAttribute(0, texEnv.get());
	stateSet->setTextureAttributeAndModes(0, tex2D.get());

	root->addChild(geode.get());

	return root.release();
}

int main(int argc, char *argv[])
{
	osgViewer::Viewer viewer;

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
	mt->addChild(createSphere());

	viewer.setSceneData(mt);

	viewer.addEventHandler(new CRotateGlider);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);

	osg::Camera* camera = viewer.getCamera();
	osg::Vec3 eye(0.0f, 0.0f, 0.0f);
	osg::Vec3 cente(0.0f, 1.0f, 0.0f);
	osg::Vec3 up(0.0f, 0.0f, 1.0f);
	camera->setViewMatrixAsLookAt(eye, cente, up);

	double nleft, nright, nbottom, ntop, nNear, nFar;
	camera->getProjectionMatrixAsFrustum(nleft, nright, nbottom, ntop, nNear, nFar);
	//camera->setProjectionMatrixAsFrustum(nleft, nright, nbottom, ntop, nNear / 2, nFar);

	viewer.realize();

	// run函数后如果没有设置操作器会自动加载一个TrackballManipulator操作器，
	// 而TrackballManipulator会重新设置相机相关的位置属性
	// 所以使用循环来处理帧渲染
	if (false)
	{
		viewer.run();
	}
	else
	{
		while (!viewer.done())
		{
			viewer.frame();
		}
	}

	return 0;
}

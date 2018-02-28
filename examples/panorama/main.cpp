//https://www.qxqzx.com/contents/507.html

#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osgGA/TrackballManipulator>
#include "RotateGlider.h"
#include <atomic>

osg::ref_ptr<osg::Node> createSphere()
{
	osg::ref_ptr<osg::Group> _root = new osg::Group;

	//新建一个半径为5的球体
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 5.0f)));

	//给球体贴图
	osg::ref_ptr<osg::Texture2D> qjTexture = new osg::Texture2D;
	qjTexture->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr<osg::Image> qjImage = osgDB::readImageFile("../textures/panorama-NSW-Australia.jpg");
	qjTexture->setImage(qjImage.get());

	osg::ref_ptr<osg::TexEnv> sphereOneTexEnv = new osg::TexEnv;
	sphereOneTexEnv->setMode(osg::TexEnv::DECAL);
	osg::ref_ptr<osg::StateSet> sphereOneStateSet = new osg::StateSet;
	sphereOneStateSet->setAttribute(sphereOneTexEnv.get());
	sphereOneStateSet->setAttributeAndModes(qjTexture.get());
	geode->setStateSet(sphereOneStateSet.get());

	_root->addChild(geode);

	return _root;
}

void main()
{
	osgViewer::Viewer viewer;

	//新建一个矩阵变化节点，把贴好图的球体放到里面去，以便于后面对物体进行旋转调整
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
	mt->addChild(createSphere());

	viewer.setSceneData(mt);

	//增加操作器
	viewer.addEventHandler(new CRotateGlider);

	//获取相机，并修改相机属性
	osg::ref_ptr<osg::Camera> camera = viewer.getCamera();

	//设置相机位置为0,0,0的原点，相机对准的位置为Y轴正方形，相机顶部垂直的位置Z轴正方形
	//http://blog.csdn.net/ivan_ljf/article/details/8764737
	osg::Vec3f eye(0.0f, 0.0f, 0.0f), cente(0.0f, 1.0f, 0.0f), up(0.0f, 0.0f, 1.0f);
	camera->setViewMatrixAsLookAt(eye, cente, up);

	//获取相机默认的近裁面和远裁面
	osg::Matrixd mx = camera->getProjectionMatrix();
	double nleft, nright, nbottom, ntop, nNear, nFar;
	camera->getProjectionMatrixAsFrustum(nleft, nright, nbottom, ntop, nNear, nFar);

	//近裁面距离相机的距离减少为之前的一般，让相机看到更大的场景
	camera->setProjectionMatrixAsFrustum(nleft, nright, nbottom, ntop, nNear / 2, 10);

	viewer.realize();

	//调用run函数后如果没有设置操作器会自动加载一个TrackballManipulator操作器，而TrackballManipulator会重新设置相机相关的位置属性
	//所以这里不调用run，而是自己写了一个循环来处理帧渲染
	//viewer.run();

	while (!viewer.done())
	{
		viewer.frame();
	}
}

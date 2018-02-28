/// \brief 测试单视图的导航器
#include "../common/CHUDPickHandlerSingle.h"
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

// 模型参数， 导航器模型大小
int main(int argc, char** argv)
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile("../models/cow.osg");

	CHUDPickHandlerSingle* cHudPickHandler = new CHUDPickHandlerSingle();

	// 设置HUD视图的位置和导航立方体的大小
	cHudPickHandler->setHudViewPositionAndCubeWidth(200.0, 20.0, 300.0, 300.0);

	// 添加Hud视图
	cHudPickHandler->createSceneNode(viewer, root);

	// 添加主场景模型
	root->addChild(cow);

	osgUtil::Optimizer op;
	op.optimize(root);
	viewer->setSceneData(root);

	viewer->realize();
	viewer->run();

	return 0;
}

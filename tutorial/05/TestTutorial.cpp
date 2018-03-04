#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "../../Util/Axis.h"

int main()
{
   osgViewer::Viewer viewer;

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../nps/T72-tank/");
   osgDB::setDataFilePathList(pathList);
   
   osg::Node* tankNode = 
	   //osgDB::readNodeFile("t72-tank_des.flt");
       osgDB::readNodeFile("NewT72.osg");

   osg::Group* root = new osg::Group();
   osg::PositionAttitudeTransform* tankXform = new osg::PositionAttitudeTransform();

   root->addChild(tankXform);
   tankXform->addChild(tankNode);

   tankXform->setPosition( osg::Vec3(1.5, 0, 0));

   root->addChild(Util::Axis::createAxis(1.0f, 1.0f, 1.0f));

   viewer.addEventHandler(new osgViewer::StatsHandler);

   viewer.setUpViewInWindow(100, 100, 1024, 768);
   viewer.setSceneData( root );
   viewer.realize();

   viewer.run();

   return 0;
}
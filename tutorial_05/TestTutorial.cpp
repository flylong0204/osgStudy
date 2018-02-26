#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
//#include <osgProducer/Viewer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

int main()
{
   osg::Node* tankNode = NULL;
   osg::Group* root = NULL;
   //osgProducer::Viewer viewer;
   osgViewer::Viewer viewer;
   osg::Vec3 tankPosit; 
   osg::PositionAttitudeTransform* tankXform;

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../asserts/nps/T72-tank/");
   osgDB::setDataFilePathList(pathList);
   
   tankNode = osgDB::readNodeFile("t72-tank_des.flt");
   //tankNode = osgDB::readNodeFile("NewT72.osg");

   root = new osg::Group();
   tankXform = new osg::PositionAttitudeTransform();

   root->addChild(tankXform);
   tankXform->addChild(tankNode);

   tankPosit.set(150,0,0); 
   tankXform->setPosition( tankPosit );

   viewer.addEventHandler(new osgViewer::StatsHandler);

   //viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
   viewer.setUpViewInWindow(100, 100, 1024, 768);
   viewer.setSceneData( root );
   viewer.realize();

   //while( !viewer.done() )
   //{
   //   viewer.sync();
   //   viewer.update();
   //   viewer.frame();
   //}
   viewer.run();

   return 0;
}
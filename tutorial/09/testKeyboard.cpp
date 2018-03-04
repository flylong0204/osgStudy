#include "../Util/KeyboardHandler.h"
#include "../Util/findNodeVisitor.h"

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Transform>
#include <osgSim/DOFTransform>
#include <osg/PositionAttitudeTransform>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

class tankDataType : public osg::Referenced
{
public:
   tankDataType(osg::Node*n); 
   void updateTurretRotation();
   void updateGunElevation();
   void setRotate(bool r) {rotate = r;}
protected:
   osgSim::DOFTransform* tankTurretNode;
   osgSim::DOFTransform* tankGunNode;
   double rotation;
   double elevation;
   bool rotate;
};

void tankDataType::updateTurretRotation()
{   
   if (rotate)
   {
      rotation += 0.01;
      tankTurretNode->setCurrentHPR( osg::Vec3(rotation,0,0) );
   }
}

void tankDataType::updateGunElevation()
{
   elevation += 0.01;
   tankGunNode->setCurrentHPR( osg::Vec3(0,elevation,0) );
   if (elevation > .5)
      elevation = 0;
}

tankDataType::tankDataType(osg::Node* n)
{
   rotation = 0;
   elevation = 0;
   rotate = false;

   findNodeVisitor findNode("turret"); 
   n->accept(findNode);
   tankTurretNode = 
      dynamic_cast <osgSim::DOFTransform*> (findNode.getFirst());

   findNodeVisitor findGun("gun"); 
   n->accept(findGun);
   tankGunNode = 
      dynamic_cast< osgSim::DOFTransform*> (findGun.getFirst());
}

tankDataType* tankOneData = NULL;

void stopTurretRotation()
{
   tankOneData->setRotate(false);
}

void rotateTurretLeft()
{
   tankOneData->setRotate(true);
}
void toggleAA()
{
   std::cout << "code to toggle AntiAliasing goes here" << std::endl;
}

class tankNodeCallback : public osg::NodeCallback 
{
public:
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osg::ref_ptr<tankDataType> tankData = 
         dynamic_cast<tankDataType*> (node->getUserData() );
      if(tankData != NULL)
      {
         tankData->updateTurretRotation();
         tankData->updateGunElevation();
      }
      traverse(node, nv); 
   }
};

int main( int argc, char **argv )
{
   osg::Node* tankNode = NULL;
   osg::Group* root = new osg::Group();
   osgViewer::Viewer viewer;

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../nps/T72-tank/");
   osgDB::setDataFilePathList(pathList);

   tankNode = osgDB::readNodeFile("t72-tank_des.flt");
   root->addChild(tankNode);

   tankOneData = new tankDataType(tankNode);

   tankNode->setUserData( tankOneData );
   tankNode->setUpdateCallback( new tankNodeCallback );

   // add a viewport to the viewer and attach the scene graph.
   viewer.setSceneData( root );

   keyboardEventHandler* keh = new keyboardEventHandler();
   viewer.addEventHandler(keh);

   keh->addFunction('a', toggleAA);
   keh->addFunction('k', keyboardEventHandler::KEY_UP, stopTurretRotation);
   keh->addFunction('i', keyboardEventHandler::KEY_DOWN, rotateTurretLeft);

   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);
   viewer.setSceneData( root );
   viewer.realize();

   viewer.run();

   return 0;
}

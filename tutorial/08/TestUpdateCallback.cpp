#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include "osgDB/FileUtils"
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>

#include "../Util/findNodeVisitor.h"

class tankDataType : public osg::Referenced
{
public:
   tankDataType(osg::Node*n); 
   void updateTurretRotation();
   void updateGunElevation();
protected:
   osgSim::DOFTransform* tankTurretNode;
   osgSim::DOFTransform* tankGunNode;
   double rotation;
   double elevation;
};

void tankDataType::updateTurretRotation()
{   
   rotation += 0.01;
   tankTurretNode->setCurrentHPR( osg::Vec3(rotation,0,0) );
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

   findNodeVisitor findNode("turret"); 
   n->accept(findNode);
   tankTurretNode = 
      dynamic_cast <osgSim::DOFTransform*> (findNode.getFirst());

   findNodeVisitor findGun("gun"); 
   n->accept(findGun);
   tankGunNode = 
      dynamic_cast< osgSim::DOFTransform*> (findGun.getFirst());
}

class tankNodeCallback : public osg::NodeCallback 
{
public:
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osg::ref_ptr<tankDataType> tankData = 
         dynamic_cast<tankDataType*> (node->getUserData() ); //node¼´ÎªtankNode
      if(tankData != NULL)
      {
         tankData->updateTurretRotation();
         tankData->updateGunElevation();
      }
      traverse(node, nv); 
   }
};

int main()
{
   osg::Node* tankNode = NULL;
   osg::Group* root = new osg::Group();
   osgViewer::Viewer viewer;

   tankNode = osgDB::readNodeFile("../nps/T72-tank/t72-tank_des.flt");
   osgDB::FilePathList pathlist = osgDB::getDataFilePathList();
   pathlist.push_back("../nps/T72-tank/");   
   osgDB::setDataFilePathList(pathlist);
   root->addChild(tankNode);

   tankDataType* tankData = new tankDataType(tankNode);

   tankNode->setUserData( tankData );
   tankNode->setUpdateCallback(new tankNodeCallback);

   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);
   viewer.setSceneData( root );
   viewer.realize();

   viewer.run();

   return 0;
}
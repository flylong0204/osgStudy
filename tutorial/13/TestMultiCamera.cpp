#include <osgViewer/CompositeViewer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgSim/MultiSwitch>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>

#include "../Util/findNodeVisitor.h"
#include "../Util/FollowNodeManip.h"
#include "../Util/Util.h"

enum cameraIndex
{
   DRIVER_CAMERA,
   GUNNER_LEFT_CAMERA,
   GUNNER_CENTER_CAMERA,
   GUNNER_RIGHT_CAMERA
};

bool ceateView(osgViewer::CompositeViewer *viewer)
{
	return true;
}

// Setup a scene with a damaged tank near our own tank.
// Return (via reference argument) a handle to a tank we'll
// eventually control and a handle to the root node.  
// Function returns false if it can't load
// models or arguments are not NULL, true otherwise.
bool setupScene(osg::ref_ptr<osg::Group> &rootNode, osg::ref_ptr<osg::Group> &ownTank)
{
	if (rootNode.get() || ownTank.get())
		return false;

	rootNode = new osg::Group();

	osgDB::FilePathList pathList = osgDB::getDataFilePathList();
	pathList.push_back("../nps/T72-tank/");
	pathList.push_back("../nps/JoeDirt/");
	osgDB::setDataFilePathList(pathList);

	osg::ref_ptr<osg::Node> terrainNode = osgDB::readNodeFile("JoeDirt.flt");
	if (!terrainNode)
	{
		std::cout << " no terrain! " << std::endl;
		return false;
	}
	rootNode->addChild(terrainNode.get());

	ownTank = (osg::Group*) osgDB::readNodeFile("T72-tank_des.flt");
	if (!ownTank)
	{
		std::cout << "no Tank" << std::endl;
		return false;
	}
	osg::PositionAttitudeTransform* ownTankPAT = new osg::PositionAttitudeTransform();
	ownTankPAT->setPosition(osg::Vec3(100, 100, 8));
	rootNode->addChild(ownTankPAT);
	ownTankPAT->addChild(ownTank.get());

	osg::ref_ptr<osg::Node> damagedTank = osgDB::readNodeFile("T72-tank_des.flt");
	if (!damagedTank)
	{
		std::cout << "no Tank" << std::endl;
		return false;
	}

	osg::PositionAttitudeTransform* damagedTankPAT = new osg::PositionAttitudeTransform();
	damagedTankPAT->setPosition(osg::Vec3(90, 110, 8));
	rootNode->addChild(damagedTankPAT);
	damagedTankPAT->addChild(damagedTank.get());

	findNodeVisitor findSwitch("sw1");
	damagedTank->accept(findSwitch);
	osgSim::MultiSwitch* damagedTankSwitch = dynamic_cast <osgSim::MultiSwitch*> (findSwitch.getFirst());
	if (!damagedTankSwitch)
		return -1;
	damagedTankSwitch->setSingleChildOn(0, false);
	//damagedTankSwitch->setSingleChildOn(0,true);

	return true;
}

int main( int argc, char **argv )
{
   osg::ref_ptr<osg::Group> rootNode; 
   osg::ref_ptr<osg::Group> ownTank;

   // build scene with terrain and two tanks
   if (!setupScene(rootNode,ownTank))
   {
      std::cout<< "problem setting up scene" << std::endl;
      return -1;
   }

   osgViewer::CompositeViewer viewer;

   osg::GraphicsContext::WindowingSystemInterface *wsi = osg::GraphicsContext::getWindowingSystemInterface();
   if (!wsi)
   {
	   osg::notify(osg::NOTICE) << "Error, no WindowingSystemInterface available, cannot create windows"
		   << std::endl;
	   return 1;
   }

   unsigned int width;
   unsigned int height;
   wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
   //
   osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
   traits->x = 0;
   traits->y = 0;
   traits->width = width;
   traits->height = height;
   traits->windowDecoration = true;
   traits->doubleBuffer = true;
   traits->sharedContext = 0;
   osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
   if (gc.valid())
   {
	   osg::notify(osg::INFO) << "GraphicsWindow has been created successfully." << std::endl;
	   gc->setClearColor(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	   gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }
   else
   {
	   osg::notify(osg::INFO) << "GraphicsWindow has NOT been created successfully." << std::endl;
	   return 2;
   }



   // Get a handle to our tank's turret node
   findNodeVisitor findTurret("turret");
   ownTank->accept(findTurret);
   osgSim::DOFTransform* turretXForm = dynamic_cast<osgSim::DOFTransform*>(findTurret.getFirst());

   // Declare an offset for the gunner and attach it to the turret
   osg::PositionAttitudeTransform* gunnerXForm = new osg::PositionAttitudeTransform();
   gunnerXForm->setPosition( osg::Vec3(0,-1.5,3.0) );
   turretXForm->addChild(gunnerXForm);

   // Declare a tranform accumulator to retreive world coords of 
   // the center gunner. Associate tranform accumulator with the offset
   // for the gunner.
   transformAccumulator* gunnerWorldCoords = new transformAccumulator();
   gunnerWorldCoords->attachToGroup(gunnerXForm);

   // Declare and set a tranform for the left gunner's view 
   // (this will be a 45 degree rotation relative to the up axis.)
   // Attach this to the gunner transform.
   osg::PositionAttitudeTransform* leftGunnerPAT = new osg::PositionAttitudeTransform();
   leftGunnerPAT->setAttitude( osg::Quat( osg::DegreesToRadians(45.0) , osg::Vec3(0,0,1) ));
   gunnerXForm->addChild(leftGunnerPAT);

   // Declare a transform accumulator to retrieve world coordinates 
   // for the left gunner. Associate this accumulator with the left
   // gunner's transform.
   transformAccumulator* leftGunnerWC = new transformAccumulator();
   leftGunnerWC->attachToGroup(leftGunnerPAT);

   // repeat this process for the right gunner, the offset will be 
   // a -45 degree rotation relative to the up axis.
   osg::PositionAttitudeTransform* rightGunnerPAT = new osg::PositionAttitudeTransform();
   rightGunnerPAT->setAttitude(osg::Quat( osg::DegreesToRadians(
	   -45.0
	   //10.0
   ) , osg::Vec3(0,0,1) ));
   transformAccumulator* rightGunnerWC = new transformAccumulator();
   gunnerXForm->addChild(rightGunnerPAT);
   rightGunnerWC->attachToGroup(rightGunnerPAT);

   // Declare and initialize a transform for the driver,
   // add this to the tank node.
   osg::PositionAttitudeTransform* driverOffsetPAT = 
      new osg::PositionAttitudeTransform();
   driverOffsetPAT->setPosition(osg::Vec3(0,-15,4));
   driverOffsetPAT->setAttitude( 
      osg::Quat( osg::DegreesToRadians(-5.0f), osg::Vec3(1,0,0) ) );
   ownTank->addChild(driverOffsetPAT);

   // Declare a transform accumulator to retrieve world coordinates 
   // of the driver transform.
   transformAccumulator *driverWorldCoords = new transformAccumulator;
   driverWorldCoords->attachToGroup(driverOffsetPAT);

   osg::ref_ptr<followNodeMatrixManipulator> mainManipulator = new followNodeMatrixManipulator(driverWorldCoords);
   osg::ref_ptr<followNodeMatrixManipulator> leftManipulator = new followNodeMatrixManipulator(leftGunnerWC);
   osg::ref_ptr<followNodeMatrixManipulator> centerManipulator = new followNodeMatrixManipulator(gunnerWorldCoords);
   osg::ref_ptr<followNodeMatrixManipulator> rightManipulator = new followNodeMatrixManipulator(rightGunnerWC);

   Util::createView(&viewer, rootNode, gc, mainManipulator, 0, 0, width, height / 2);

   Util::createView(&viewer, rootNode, gc, leftManipulator, 0, height / 2, width / 3, height / 2);
   Util::createView(&viewer, rootNode, gc, centerManipulator, width/3, height / 2, width / 3, height / 2);
   Util::createView(&viewer, rootNode, gc, rightManipulator, 2*width/3, height / 2, width / 3, height / 2);

   osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keySwitch = new osgGA::KeySwitchMatrixManipulator;
   keySwitch->addMatrixManipulator('m', "main", mainManipulator.get());
   keySwitch->addMatrixManipulator('n', "left", leftManipulator.get());
   keySwitch->addMatrixManipulator('b', "center", centerManipulator.get());
   keySwitch->addMatrixManipulator('v', "right", rightManipulator.get());

   viewer.setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

   while( !viewer.done() )
   {
	  leftManipulator->setByInverseMatrix( 
         osg::Matrix(leftGunnerWC->getMatrix().ptr() )*
            osg::Matrix::rotate( -osg::PI/2.0, osg::Vec3(1, 0, 0 ) ));

	  centerManipulator->setByInverseMatrix( 
         osg::Matrix(gunnerWorldCoords->getMatrix().ptr() )*
            osg::Matrix::rotate( -osg::PI/2.0, osg::Vec3(1, 0, 0 ) ));

	  rightManipulator->setByInverseMatrix( 
         osg::Matrix(rightGunnerWC->getMatrix().ptr() )*
            osg::Matrix::rotate( -osg::PI/2.0, osg::Vec3(1, 0, 0 ) ));

      // fire off the cull and draw traversals of the scene.
      viewer.frame();
   }

   return 0;
}

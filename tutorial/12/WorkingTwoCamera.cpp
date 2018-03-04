#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>

#include "../Util/TransformAccumulator.h"
#include "../Util/Util.h"

class circleAimlessly : public osg::NodeCallback 
{
public:
   circleAimlessly(): _angle(0.0) {}
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osg::MatrixTransform *tx = dynamic_cast<osg::MatrixTransform *>(node);
      if( tx != NULL )
      {
         _angle += osg::PI/180.0;
         tx->setMatrix(osg::Matrix::translate(100.0, 0.0, 20.0) * osg::Matrix::rotate(_angle, 0, 0, 1 ) );
      }
      traverse(node, nv);
   }
private:
   float _angle;
};

// Setup a scene, return (via reference arguments) handles
// to the root node and the tank model.  
// Attach an update callback to the tank transform so the tank
// circles above our terrain model.
// Function returns false if it can't load
//  models or arguments are not NULL, true otherwise.
bool setupScene(osg::ref_ptr<osg::Group> &rootNode,osg::ref_ptr<osg::Group> &ownTank )
{
   if (rootNode.get() || ownTank.get())
      return false;

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../nps/T72-tank/");
   pathList.push_back("../nps/JoeDirt/");
   osgDB::setDataFilePathList(pathList);

   rootNode = new osg::Group();
   osg::ref_ptr<osg::Node> terrainNode = osgDB::readNodeFile("JoeDirt.flt");
   if (!terrainNode)
   {
      std::cout << " no terrain! " << std::endl;
      return false;
   }
   rootNode->addChild(terrainNode.get());

   ownTank = (osg::Group*)(osgDB::readNodeFile("T72-tank_des.flt"));
   if( ! ownTank)
   {
      std::cout << "no Tank" << std::endl;
      return false;
   }
   osg::MatrixTransform * tankXForm = new osg::MatrixTransform();
   tankXForm->setUpdateCallback(new circleAimlessly());
   rootNode->addChild(tankXForm);
   tankXForm->addChild(ownTank.get());
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

   osg::PositionAttitudeTransform * followerOffset = new osg::PositionAttitudeTransform();
   followerOffset->setPosition(osg::Vec3(0.0, -25.0, 10));
   followerOffset->setAttitude(osg::Quat(osg::DegreesToRadians(-15.0), osg::Vec3(1, 0, 0)));

   ownTank.get()->addChild(followerOffset);

   transformAccumulator* tankFollowerWorldCoords = new transformAccumulator();
   tankFollowerWorldCoords->attachToGroup(followerOffset);

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

   osgGA::TrackballManipulator *tankManipulator = new osgGA::TrackballManipulator;
   osgGA::TrackballManipulator *mainManipulator = new osgGA::TrackballManipulator;
   osgGA::TrackballManipulator *thirdManipulator = new osgGA::TrackballManipulator;


   Util::createView(&viewer, rootNode, gc, mainManipulator, 0, 0, width, height / 2);

   Util::createView(&viewer, rootNode, gc, tankManipulator, 0, height / 2, width / 2, height / 2);

   Util::createView(&viewer, rootNode, gc, thirdManipulator, width / 2, height / 2, width / 2, height / 2);

   viewer.setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

   while( !viewer.done() )
   {
	   tankManipulator->setByInverseMatrix(osg::Matrix(tankFollowerWorldCoords->getMatrix()) *
		   osg::Matrix::rotate(-osg::PI / 2.0f, osg::Vec3(1.0f, 0.0f, 0.0f)));

	  osg::Matrix temp=tankFollowerWorldCoords->getMatrix() * osg::Matrix::rotate(
		  osg::DegreesToRadians(10.0),osg::Vec3(1,0,0),
		  osg::DegreesToRadians(0.0),osg::Vec3(0,1,0),
		  osg::DegreesToRadians(-16.0),osg::Vec3(0,0,1)
		  );
	  thirdManipulator->setByInverseMatrix(temp * osg::Matrix::rotate(-osg::PI / 2.0f, osg::Vec3(1.0f, 0.0f, 0.0f)));

      // fire off the cull and draw traversals of the scene.
      viewer.frame();
   }

   return 0;
}
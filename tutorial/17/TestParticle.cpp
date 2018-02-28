#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile> 
#include <osgDB/FileUtils>
#include <osg/Group>
#include <osg/Geode>

#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/AccelOperator>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/Interpolator>

#include <osg/NodeCallback>

class orbit : public osg::NodeCallback 
{
public:
   orbit(): _angle(0.0) {}
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osg::MatrixTransform *tx = dynamic_cast<osg::MatrixTransform *>(node);
      if( tx != NULL )
      {
         _angle += osg::PI/(180.0*5.0);
         tx->setMatrix( osg::Matrix::translate( 50.0, 0.0, 8.0) * 
            osg::Matrix::rotate( _angle, 0, 0, 1 ) );
      }
      traverse(node, nv);
   }
private:
   float _angle;
};
   
int main(int argc, char **argv)
{
   osg::Group* rootNode = new osg::Group();
   osg::Node* terrainNode = new osg::Node();
   osgViewer::Viewer viewer;
   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../asserts/nps/T72-tank/");
   pathList.push_back("../asserts/nps/JoeDirt/");
   pathList.push_back("../asserts/nps/");
   osgDB::setDataFilePathList(pathList);

   terrainNode = osgDB::readNodeFile("JoeDirt.flt");
   if (! terrainNode)
   {
      std::cout << "Couldn't load models, quitting." << std::endl;
      return -1;
   }
   rootNode->addChild(terrainNode);

   osg::Node* tankNode = osgDB::readNodeFile("T72-tank_des.flt");
   if ( ! tankNode)
   {
      std::cout << "no tank" << std::endl;
      return -1;
   }

   // Create and initialize a particle system
   osgParticle::ParticleSystem *dustParticleSystem = new osgParticle::ParticleSystem;

   // Set the attributes 'texture', 'emmisive' and 'lighting'
   dustParticleSystem->setDefaultAttributes("dust2.rgb",false, false);

   // Since the particle system is derived from the class Drawable, we can create
   // add it to the scene as a child of a geode 
   osg::Geode *geode = new osg::Geode; 
   rootNode->addChild(geode);
   geode->addDrawable(dustParticleSystem);

   // Add an 'updater' to help per-frame management
   osgParticle::ParticleSystemUpdater *dustSystemUpdater = new osgParticle::ParticleSystemUpdater;
   // Associate this updater with our particle system 
   dustSystemUpdater->addParticleSystem(dustParticleSystem);
   // add the updater node to the scene graph
   rootNode->addChild(dustSystemUpdater);

   // Create a partical to be used by our particle system and define a few
   // of its properties
   osgParticle::Particle smokeParticle; 
   smokeParticle.setSizeRange(osgParticle::rangef(0.01,20.0)); // meters
   smokeParticle.setLifeTime(4); // seconds
   smokeParticle.setMass(0.01); // in kilograms
   // Make this our particle system's default particle 
   dustParticleSystem->setDefaultParticleTemplate(smokeParticle);

   // Create a modular emitter (this contains default counter, placer and shooter.)
   osgParticle::ModularEmitter *emitter = new osgParticle::ModularEmitter;
   // Associate this emitter with the particle system 
   emitter->setParticleSystem(dustParticleSystem);

   // Get a handle to the emitter's counter and adjust the number of new particles
   // that will be added each frame
   osgParticle::RandomRateCounter *dustRate = static_cast<osgParticle::RandomRateCounter *>(emitter->getCounter());
   dustRate->setRateRange(5, 10); // generate 5 to 10 particles per second

   // To customize the placer, create and initialize a multi-segment placer 
   osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
   // Add vertices to our placer. This defines line seqments along which our particles will
   // originate. (If we co-locate a tank and this emitter, this will result in dust particles
   // originating from a line extending below and behind the tank model.) 
   lineSegment->addVertex(0,0,-2);
   lineSegment->addVertex(0,-2,-2);
   lineSegment->addVertex(0,-16,0);
   // Use this placer for our emitter 
   emitter->setPlacer(lineSegment);

   // To customize the shooter, create and initialize a radial shooter 
   osgParticle::RadialShooter* smokeShooter = new osgParticle::RadialShooter();
   // Set properties of this shooter 
   smokeShooter->setThetaRange(0.0, 3.14159/2); // radians, relative to Z axis.
   smokeShooter->setInitialSpeedRange(50,100); // meters/second
   // Use this shooter for our emitter
   emitter->setShooter(smokeShooter);


   osg::MatrixTransform * tankTransform = new osg::MatrixTransform(); 
   tankTransform->setUpdateCallback( new orbit() ); // Callback that drives in a circle 

   // Add the emitter and the tank as children of this transform 
   tankTransform->addChild(emitter);
   tankTransform->addChild(tankNode);
   rootNode->addChild(tankTransform);

   // Create a modular program and attach it to our particle system
   osgParticle::ModularProgram *moveDustInAir = new osgParticle::ModularProgram;
   moveDustInAir->setParticleSystem(dustParticleSystem);

   // Create an operator that simulates gravity, adjust it and add it to our program
   osgParticle::AccelOperator *accelUp = new osgParticle::AccelOperator;
   accelUp->setToGravity(-1); // scale factor for normal acceleration due to gravity. 
   moveDustInAir->addOperator(accelUp);

   // Add an operator to our program to calculate the friction of air.
   osgParticle::FluidFrictionOperator *airFriction = new osgParticle::FluidFrictionOperator;
   airFriction->setFluidToAir();
   //airFriction->setFluidDensity(1.2929/*air*/*5.0f);
   moveDustInAir->addOperator(airFriction);

   // Finally, add the program to the scene 
   rootNode->addChild(moveDustInAir);


   // add a viewport to the viewer and attach the scene graph.
   viewer.setSceneData(rootNode);

   // create the windows and run the threads.
   viewer.realize();

   viewer.run();

   return 0;
}
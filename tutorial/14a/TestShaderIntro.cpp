#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Shader>
#include <osg/Program>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <iostream>

bool loadShaderSource(osg::Shader* obj, const std::string& fileName );

int main( int argc, char **argv )
{
   osg::Group* rootNode = new osg::Group();
   osg::Node* tankNode = NULL;
   osg::Node* terrainNode = NULL;
   osg::PositionAttitudeTransform* tankXform = new osg::PositionAttitudeTransform();

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../asserts/nps/T72-tank/");
   pathList.push_back("../asserts/nps/JoeDirt/");
   osgDB::setDataFilePathList(pathList);

   tankNode = osgDB::readNodeFile("T72-tank_des.flt");
   terrainNode = osgDB::readNodeFile("JoeDirt.flt");
   if (! (tankNode && terrainNode))
   {
      std::cout << "Couldn't load models, quitting." << std::endl;
      return -1;
   }
   rootNode->addChild(terrainNode);
   rootNode->addChild(tankXform);
   tankXform->addChild(tankNode);

   tankXform->setPosition(osg::Vec3(10,10,8));
   tankXform->setAttitude(osg::Quat(osg::DegreesToRadians(-45.0),osg::Vec3(0,0,1) ) );
   
   // attach an "empty" ProgramObject to the rootNode, which will act as
   // the default StateAttribute. An empty ProgramObject (ie without any
   // attached ShaderObjects) is a special case, which means to use the
   // OpenGL 1.x "fixed functionality" rendering pipeline.
   osg::StateSet* rootState = rootNode->getOrCreateStateSet();
   osg::Program* defaultProgramObject = new osg::Program;
   rootState->setAttributeAndModes(defaultProgramObject, osg::StateAttribute::ON);

   osg::StateSet* brickState = tankNode->getOrCreateStateSet();

   osg::Program* brickProgramObject = new osg::Program;
   osg::Shader* brickVertexObject = new osg::Shader( osg::Shader::VERTEX );
   osg::Shader* brickFragmentObject = new osg::Shader( osg::Shader::FRAGMENT );
   brickProgramObject->addShader( brickVertexObject );
   brickProgramObject->addShader( brickFragmentObject );   
   loadShaderSource( brickVertexObject, "../asserts/nps/shaders/brick.vert" );
   loadShaderSource( brickFragmentObject, "../asserts/nps/shaders/brick.frag" );

   brickState->setAttributeAndModes(brickProgramObject, osg::StateAttribute::ON);

   osgViewer::Viewer viewer;
   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);

   viewer.setSceneData( rootNode );
   viewer.realize();

   viewer.run();

   return 0;
}

bool loadShaderSource(osg::Shader* obj, const std::string& fileName )
{
   std::string fqFileName = osgDB::findDataFile(fileName);
   if( fqFileName.length() == 0 )
   {
      std::cout << "File \"" << fileName << "\" not found." << std::endl;
      return false;
   }
   bool success = obj->loadShaderSourceFromFile( fqFileName.c_str());
   if ( !success  )
   {
      std::cout << "Couldn't load file: " << fileName << std::endl;
      return false;
   }
   else
   {
      return true;
   }
}

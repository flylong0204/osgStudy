#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Program>
#include <osg/Shader>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <iostream>
#include <osg/Texture2D>

bool loadShaderSource(osg::Shader* obj, const std::string& fileName );

int main( int argc, char **argv )
{
   osg::Group* rootNode = new osg::Group();
   osg::Node* tankNode = NULL;
   osg::Node* terrainNode = NULL;
   osg::PositionAttitudeTransform* tankXform = 
      new osg::PositionAttitudeTransform();

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../asserts/nps/T72-tank/");
   pathList.push_back("../asserts/nps/JoeDirt/");
   pathList.push_back("../asserts/nps/");
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

   tankXform->setPosition( osg::Vec3(10,10,8) );
   tankXform->setAttitude( 
      osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,0,1) ) );

   // attach an "empty" ProgramObject to the rootNode, which will act as
   // the default StateAttribute. An empty ProgramObject (ie without any
   // attached ShaderObjects) is a special case, which means to use the
   // OpenGL 1.x "fixed functionality" rendering pipeline.
   osg::StateSet* rootState = rootNode->getOrCreateStateSet();
   osg::Program* defaultProgramObject = new osg::Program;
   rootState->setAttributeAndModes(defaultProgramObject, osg::StateAttribute::ON);
   
   osg::StateSet* bumpMapState = tankNode->getOrCreateStateSet();

   osg::Program* bumpMapProgramObject = new osg::Program;

   osg::Shader* brickVertexObject =  new osg::Shader( osg::Shader::VERTEX );
   osg::Shader* brickFragmentObject = new osg::Shader( osg::Shader::FRAGMENT );

   bool ok = loadShaderSource( brickVertexObject, "../asserts/nps/shaders/bumpmap.vert" )
          && loadShaderSource( brickFragmentObject, "../asserts/nps/shaders/bumpmap.frag" ) ;

   if(!ok)
   {
      std::cout << "Couldn't load shaders" << std::endl;
      return -1;
   }

   bumpMapProgramObject->addShader( brickFragmentObject );
   bumpMapProgramObject->addShader( brickVertexObject );

   //bumpMapProgramObject->setUniform("LightPosition", osg::Vec3(0,0,1));
   //bumpMapProgramObject->setSampler("normalMap",1);
   //bumpMapProgramObject->setSampler("baseTexture",0);

   osg::Uniform* lightPosU = new osg::Uniform("LightPosition",osg::Vec3(0,0,1));
   osg::Uniform* normalMapU = new osg::Uniform("normalMap",1);
   osg::Uniform* baseTextureU = new osg::Uniform("baseTexture",0);

   bumpMapState->addUniform(lightPosU);
   bumpMapState->addUniform(normalMapU);
   bumpMapState->addUniform(baseTextureU);
   
   bumpMapState->setAttributeAndModes(bumpMapProgramObject, osg::StateAttribute::ON);

   osg::Texture2D* tankBodyNormalMap = new osg::Texture2D;
   // protect from being optimized away as static state:
   tankBodyNormalMap->setDataVariance(osg::Object::DYNAMIC); 
   osg::Image* tankBody = osgDB::readImageFile("TankBump.bmp");
   if (!tankBody)
   {
      std::cout << " couldn't find texture, quiting." << std::endl;
      return -1;
   }

   tankBodyNormalMap->setImage(tankBody);
   bumpMapState->setTextureAttributeAndModes(1,tankBodyNormalMap,osg::StateAttribute::ON);

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

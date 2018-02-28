#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
//#include <osgProducer/Viewer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgText/Font>
#include <osgText/Text>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Geometry>

int main()
{
   // Group to serve as root of scene graph:
   osg::Group* root = NULL;
   // Node to contain geometry for a tank model:
   osg::Node* tankNode = NULL;
   // Node to contain geometry for terrain model:
   osg::Node* terrainNode = NULL;
   // Matrix transform node for positioning tank model:
   osg::PositionAttitudeTransform* tankXform;
   // Position of tank:
   osg::Vec3 tankPosit;
   // A geometry node for our HUD:
   osg::Geode* HUDGeode = new osg::Geode();
   // Text instance for HUD:
   osgText::Text* textOne = new osgText::Text();
   // Text for label that will follow the tank:
   osgText::Text* tankLabel = new osgText::Text();
   // Projection node for defining view frustrum for HUD:
   osg::Projection* HUDProjectionMatrix = new osg::Projection;
   // Viewer instance to handle scene rendering:
   //osgProducer::Viewer viewer;
   osgViewer::Viewer viewer;

   // Initialize root of scene:
   root = new osg::Group();

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../asserts/nps/T72-tank/");
   pathList.push_back("../asserts/nps/JoeDirt/");
   pathList.push_back("../asserts/nps/");

   osgDB::setDataFilePathList(pathList);

   // Load models from files and assign to nodes:
   tankNode = osgDB::readNodeFile("t72-tank_des.flt");
   terrainNode = osgDB::readNodeFile("JoeDirt.flt");

   // Initialize tranform to be used for positioning the tank
   // assign values for the transform
   tankXform = new osg::PositionAttitudeTransform();
   tankPosit.set(5,5,8);
   tankXform->setPosition( tankPosit );

   // Build the scene - add the terrain node directly to the root,
   // connect the tank node to the root via the transform node:
   root->addChild(terrainNode);
   root->addChild(tankXform);
   tankXform->addChild(tankNode);

   // Initialize the projection matrix for viewing everything we
   // will add as descendents of this node. Use screen coordinates
   // to define the horizontal and vertical extent of the projection
   // matrix. Positions described under this node will equate to
   // pixel coordinates.
   HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,1024,0,768));
  
   // For the HUD model view matrix use an identity matrix:
   osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
   HUDModelViewMatrix->setMatrix(osg::Matrix::identity());

   // Make sure the model view matrix is not affected by any transforms
   // above it in the scene graph:
   HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

   // Add the HUD projection matrix as a child of the root node
   // and the HUD model view matrix as a child of the projection matrix
   // Anything under this node will be view using this projection matrix
   // and positioned with this model view matrix.
   root->addChild(HUDProjectionMatrix);
   HUDProjectionMatrix->addChild(HUDModelViewMatrix);

   // Add the Geometry node to contain HUD geometry as a child of the
   // HUD model view matrix.
   // (See figure "n")
   HUDModelViewMatrix->addChild( HUDGeode );

   // Add the text (Text class is derived from drawable) to the geode:
   HUDGeode->addDrawable( textOne );

   // Set up the parameters for the text we'll add to the HUD:
   textOne->setCharacterSize(25);
   textOne->setFont("../asserts/fonts/simfang.ttf");
   textOne->setText("Not so good");
   textOne->setAxisAlignment(osgText::Text::SCREEN);
   textOne->setPosition( osg::Vec3(360,365,-1.5) );
   textOne->setColor( osg::Vec4(199, 77, 15, 1) );

   // Set up the parameters for the text label for the tank
   // align text with tank's SCREEN.
   // (for Onder: use XZ_PLANE to align text with tank's XZ plane.)
   tankLabel->setCharacterSize(5);
   tankLabel->setFont("../asserts/fonts/times.ttf");
   tankLabel->setText("tank #1");
   tankLabel->setAxisAlignment(osgText::Text::XZ_PLANE);

   // Set the text to render with alignment anchor and bounding box around it:
   tankLabel->setDrawMode(osgText::Text::TEXT |
      osgText::Text::ALIGNMENT |
      osgText::Text::BOUNDINGBOX);
   tankLabel->setAlignment(osgText::Text::CENTER_TOP);
   tankLabel->setPosition( osg::Vec3(0,0,8) );//hyy 相对坦克移动，升高8个单位
   tankLabel->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );

   // Declare a geode to contain the tank's text label:
   osg::Geode* tankLabelGeode = new osg::Geode();

   // Add the tank label to the scene using a geode:
   tankLabelGeode->addDrawable(tankLabel);
   tankXform->addChild(tankLabelGeode);


   // Set up geometry for the HUD and add it to the HUD
   // geometry node, see tutorial "n" for details:
   osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();

   osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;

   HUDBackgroundVertices->push_back( osg::Vec3( 0, 0,-1) );
   HUDBackgroundVertices->push_back( osg::Vec3(1024, 0,-1) );
   HUDBackgroundVertices->push_back( osg::Vec3(1024,200,-1) );
   HUDBackgroundVertices->push_back( osg::Vec3( 0,200,-1) );

   osg::DrawElementsUInt* HUDBackgroundIndices =
      new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
   HUDBackgroundIndices->push_back(0);
   HUDBackgroundIndices->push_back(1);
   HUDBackgroundIndices->push_back(2);
   HUDBackgroundIndices->push_back(3);

   osg::Vec4Array* HUDcolors = new osg::Vec4Array;
   HUDcolors->push_back(osg::Vec4(0.8f,0.8f,0.8f,0.8f));

   osg::Vec2Array* texcoords = new osg::Vec2Array(4);
   (*texcoords)[0].set(0.0f,0.0f);
   (*texcoords)[1].set(1.0f,0.0f);
   (*texcoords)[2].set(1.0f,1.0f);
   (*texcoords)[3].set(0.0f,1.0f);
   HUDBackgroundGeometry->setTexCoordArray(0,texcoords);
   osg::Texture2D* HUDTexture = new osg::Texture2D;
   HUDTexture->setDataVariance(osg::Object::DYNAMIC);
   osg::Image* hudImage;
   hudImage = osgDB::readImageFile("HUDBack2.tga");
   HUDTexture->setImage(hudImage);
   osg::Vec3Array* HUDnormals = new osg::Vec3Array;
   HUDnormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
   HUDBackgroundGeometry->setNormalArray(HUDnormals);
   HUDBackgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

   HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
   HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
   HUDBackgroundGeometry->setColorArray(HUDcolors);
   HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   HUDGeode->addDrawable(HUDBackgroundGeometry);

   // Create and set up a state set using the texture from above:
   osg::StateSet* HUDStateSet = new osg::StateSet();
   HUDGeode->setStateSet(HUDStateSet);
   HUDStateSet->setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON);

   // For this state set, turn blending on (so alpha texture looks right)
   HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);

   // Disable depth testing so geometry is draw regardless of depth values
   // of geometry already draw.
   HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

   // Need to make sure this geometry is draw last. RenderBins are handled
   // in numerical order so set bin number to 11
   HUDStateSet->setRenderBinDetails( 99, "RenderBin");

   //viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);
   viewer.setSceneData( root );

   viewer.realize();
   viewer.run();

   return 0;
} 

/*
  This is the old version....

  #include <osg/PositionAttitudeTransform>
  #include <osg/Group>
  #include <osg/Node>
  #include <osgDB/ReadFile> 
  #include <osgDB/FileUtils>
  #include <osgProducer/Viewer>

  #include <osgText/Font>
  #include <osgText/Text>
  #include <osg/MatrixTransform>
  #include <osg/Geode>
  #include <osg/Projection>
  #include <osg/ShapeDrawable>
  #include <osg/Geometry>

  int main()
  {
  osg::Group* root = NULL; 
  osg::Node* tankNode = NULL; 
  osg::Node* terrainNode = NULL;
  osg::PositionAttitudeTransform* tankXform;
  osg::Vec3 tankPosit; 
  osg::Geode* HUDGeode = new osg::Geode();
  osgText::Text* textOne = new osgText::Text();
  osgText::Text* tankLabel = new osgText::Text();
  osg::Projection* HUDProjectionMatrix = new osg::Projection;
  osgProducer::Viewer viewer;

  root = new osg::Group();

  osgDB::FilePathList pathList = osgDB::getDataFilePathList();
  pathList.push_back("C:\\Projects\\OpenSceneGraph\\OpenSceneGraph-Data\\NPSData\\Models\\T72-Tank\\");
  pathList.push_back("C:\\Projects\\OpenSceneGraph\\OpenSceneGraph-Data\\NPSData\\Models\\JoeDirt\\");
  osgDB::setDataFilePathList(pathList);

  tankNode = osgDB::readNodeFile("t72-tank_des.flt");
  terrainNode = osgDB::readNodeFile("JoeDirt.flt");

  tankXform = new osg::PositionAttitudeTransform();
  tankPosit.set(5,5,8);
  tankXform->setPosition( tankPosit ); 
  root->addChild(terrainNode);
  root->addChild(tankXform);
  tankXform->addChild(tankNode);

  HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,1024,0,768));

  osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
  HUDModelViewMatrix->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);
  HUDModelViewMatrix->setMatrix(osg::Matrix::identity());

  root->addChild(HUDProjectionMatrix);
  HUDProjectionMatrix->addChild(HUDModelViewMatrix);
  HUDModelViewMatrix->addChild( HUDGeode );
  HUDGeode->addDrawable( textOne );

  textOne->setCharacterSize(25);
  textOne->setFont("C:/WINDOWS/Fonts/impact.ttf");
  textOne->setText("Not so good");
  textOne->setAxisAlignment(osgText::Text::SCREEN);
  textOne->setPosition( osg::Vec3(242,165,1) );
  textOne->setColor( osg::Vec4(1,0,0,1.0f) );

  tankLabel->setCharacterSize(2);
  tankLabel->setFont("/fonts/arial.ttf");
  tankLabel->setText("Tank #1");
  tankLabel->setAxisAlignment(osgText::Text::SCREEN);
  tankLabel->setDrawMode(osgText::Text::TEXT |
  osgText::Text::ALIGNMENT | 
  osgText::Text::BOUNDINGBOX);

  tankLabel->setAlignment(osgText::Text::CENTER_TOP);
  tankLabel->setPosition( osg::Vec3(0,0,4) );
  tankLabel->setColor( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

  osg::Geode* tankLabelGeode = new osg::Geode();
  tankLabelGeode->addDrawable(tankLabel);
  tankXform->addChild(tankLabelGeode);

  osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();

  osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;
  HUDBackgroundVertices->push_back( osg::Vec3(   0,  0,.8) );
  HUDBackgroundVertices->push_back( osg::Vec3(1024,  0,.8) );
  HUDBackgroundVertices->push_back( osg::Vec3(1024,200,.8) );
  HUDBackgroundVertices->push_back( osg::Vec3(   0,200,.8) );

  osg::DrawElementsUInt* HUDBackgroundIndices = 
  new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
  HUDBackgroundIndices->push_back(0);
  HUDBackgroundIndices->push_back(1);
  HUDBackgroundIndices->push_back(2);
  HUDBackgroundIndices->push_back(3);

  osg::Vec4Array* HUDcolors = new osg::Vec4Array;
  HUDcolors->push_back(osg::Vec4(0.8f,0.8f,0.8f,0.5f));

  osg::Vec2Array* texcoords = new osg::Vec2Array(4);
  (*texcoords)[0].set(0.0f,0.0f);
  (*texcoords)[1].set(1.0f,0.0f);
  (*texcoords)[2].set(1.0f,1.0f);
  (*texcoords)[3].set(0.0f,1.0f);
  HUDBackgroundGeometry->setTexCoordArray(0,texcoords);

  // set up the texture state.    
  osg::Texture2D* HUDTexture = new osg::Texture2D;
  // protect from being optimized away as static state.
  HUDTexture->setDataVariance(osg::Object::DYNAMIC); 

  //pathList.push_back("C:\\Projects\\OpenSceneGraph\\OpenSceneGraph-Data\\NPSData\\textures\\");
  //osgDB::setDataFilePathList(pathList);

  HUDTexture->setImage(osgDB::readImageFile("C:\\Projects\\OpenSceneGraph\\OpenSceneGraph-Data\\NPSData\\textures\\HUDBackground.tga"));

  osg::Vec3Array* HUDnormals = new osg::Vec3Array;
  HUDnormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
  HUDBackgroundGeometry->setNormalArray(HUDnormals);
  HUDBackgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

  HUDGeode->addDrawable(HUDBackgroundGeometry);
  HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
  HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
  HUDBackgroundGeometry->setColorArray(HUDcolors);
  HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

  osg::StateSet* HUDStateSet = new osg::StateSet(); 
  HUDGeode->setStateSet(HUDStateSet);
  HUDStateSet->setRenderBinDetails( 11, "DepthSortedBin");
  HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
  HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
  HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
  HUDStateSet->setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON);
  osgUtil::RenderBin* HUDBin = 
  new osgUtil::RenderBin(osgUtil::RenderBin::SORT_BACK_TO_FRONT);

  viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
  viewer.setSceneData( root );

  viewer.realize();

  while( !viewer.done() )
  {
  viewer.sync();
  viewer.update();
  viewer.frame();
  }
  }
  */
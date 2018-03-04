#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
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
   // A geometry node for our HUD:
   osg::Geode* HUDGeode = new osg::Geode();
   // Text instance for HUD:
   osgText::Text* textOne = new osgText::Text();
   // Text for label that will follow the tank:
   osgText::Text* tankLabel = new osgText::Text();
   // Projection node for defining view frustrum for HUD:
   osg::Projection* HUDProjectionMatrix = new osg::Projection;
   
   osgViewer::Viewer viewer;
   osg::Group* root = new osg::Group();

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../nps/T72-tank/");
   pathList.push_back("../nps/JoeDirt/");
   pathList.push_back("../nps/");
   osgDB::setDataFilePathList(pathList);

   // Load models from files and assign to nodes:
   osg::Node* tankNode = osgDB::readNodeFile("t72-tank_des.flt");
   osg::Node* terrainNode = osgDB::readNodeFile("JoeDirt.flt");

   // Initialize tranform to be used for positioning the tank
   // assign values for the transform
   osg::PositionAttitudeTransform* tankXform = new osg::PositionAttitudeTransform();
   tankXform->setPosition( osg::Vec3(5, 5, 8));

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
   textOne->setFont("../fonts/simfang.ttf");
   textOne->setText("Not so good");
   textOne->setAxisAlignment(osgText::Text::SCREEN);
   textOne->setPosition( osg::Vec3(360,365,-1.5) );
   textOne->setColor( osg::Vec4(199, 77, 15, 1) );

   // Set up the parameters for the text label for the tank
   // align text with tank's SCREEN.
   // (for Onder: use XZ_PLANE to align text with tank's XZ plane.)
   tankLabel->setCharacterSize(5);
   tankLabel->setFont("../fonts/times.ttf");
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
   osg::Image* hudImage = osgDB::readImageFile("KLN89FaceB.tga");
   HUDTexture->setImage(hudImage);
   osg::Vec3Array* HUDnormals = new osg::Vec3Array;
   HUDnormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
   HUDBackgroundGeometry->setNormalArray(HUDnormals, osg::Array::BIND_OVERALL);

   HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
   HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);
   HUDBackgroundGeometry->setColorArray(HUDcolors, osg::Array::BIND_OVERALL);

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
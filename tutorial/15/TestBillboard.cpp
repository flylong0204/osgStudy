#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Billboard>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>

osg::Drawable* createShrub(const float & scale, osg::StateSet* bbState)
{
   float width = 1.5f;
   float height = 3.0f;

   width *= scale;
   height *= scale;

   osg::Geometry* shrubQuad = new osg::Geometry;

   osg::Vec3Array* shrubVerts = new osg::Vec3Array(4);
   (*shrubVerts)[0] = osg::Vec3(-width/2.0f, 0, 0);
   (*shrubVerts)[1] = osg::Vec3( width/2.0f, 0, 0);
   (*shrubVerts)[2] = osg::Vec3( width/2.0f, 0, height );
   (*shrubVerts)[3] = osg::Vec3(-width/2.0f, 0, height );

   shrubQuad->setVertexArray(shrubVerts);

   osg::Vec2Array* shrubTexCoords = new osg::Vec2Array(4);
   (*shrubTexCoords)[0].set(0.0f,0.0f);
   (*shrubTexCoords)[1].set(1.0f,0.0f);
   (*shrubTexCoords)[2].set(1.0f,1.0f);
   (*shrubTexCoords)[3].set(0.0f,1.0f);
   shrubQuad->setTexCoordArray(0,shrubTexCoords);

   shrubQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

   shrubQuad->setStateSet(bbState); 

   return shrubQuad;
}

int main( int argc, char **argv )
{
   osgViewer::Viewer viewer;

   osg::Group* rootNode = new osg::Group();
   osg::Node* tankNode = NULL;
   osg::Node* terrainNode = NULL;
   osg::PositionAttitudeTransform* tankXform = new osg::PositionAttitudeTransform();

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
   tankXform->setAttitude( osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,0,1) ) );

   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);

   viewer.setSceneData(rootNode);
   viewer.realize();

   osg::Billboard* shrubBillBoard = new osg::Billboard();
   rootNode->addChild(shrubBillBoard);

   shrubBillBoard->setMode(osg::Billboard::AXIAL_ROT);
   shrubBillBoard->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
   shrubBillBoard->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));

   osg::Texture2D *ocotilloTexture = new osg::Texture2D;
   ocotilloTexture->setImage(osgDB::readImageFile("Ocotillo.bmp"));

   osg::StateSet* billBoardStateSet = new osg::StateSet;

   billBoardStateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
   billBoardStateSet->setTextureAttributeAndModes(0, ocotilloTexture, osg::StateAttribute::ON );
   billBoardStateSet->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );

   osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
   alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
   
   billBoardStateSet->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );

   osg::Drawable* shrub1Drawable = createShrub( 1.0f, billBoardStateSet);
   osg::Drawable* shrub2Drawable = createShrub( 2.0f, billBoardStateSet);
   osg::Drawable* shrub3Drawable = createShrub( 1.2f, billBoardStateSet);
   osg::Drawable* shrub4Drawable = createShrub( 0.8f, billBoardStateSet);
   osg::Drawable* shrub5Drawable = createShrub( 0.5f, billBoardStateSet);
   osg::Drawable* shrub6Drawable = createShrub( 1.6f, billBoardStateSet);

   shrubBillBoard->addDrawable( shrub1Drawable , osg::Vec3(12,3,8) );
   shrubBillBoard->addDrawable( shrub2Drawable , osg::Vec3(10,18,8));
   shrubBillBoard->addDrawable( shrub3Drawable , osg::Vec3(6,10,8));
   shrubBillBoard->addDrawable( shrub4Drawable , osg::Vec3(8,3,8) );
   shrubBillBoard->addDrawable( shrub5Drawable , osg::Vec3(12,18,8));
   shrubBillBoard->addDrawable( shrub6Drawable , osg::Vec3(6,5,8));

   viewer.run();

   return 0;
}

#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Billboard>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>
#include <osgDB/FileUtils>
#include <osgUtil/IntersectionVisitor>
#include <ctime>    
#include <cstdlib> 

#define NUMBER_O_SHRUBS 1000

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
   (*shrubVerts)[2] = osg::Vec3( width/2.0f, 0, height);
   (*shrubVerts)[3] = osg::Vec3(-width/2.0f, 0, height);

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
   pathList.push_back("../nps/T72-tank/");
   pathList.push_back("../nps/JoeDirt/");
   pathList.push_back("../nps/");
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
   double tankXPosition = 5.0;
   double tankYPosition = -3.0;

   osg::ref_ptr<osgUtil::LineSegmentIntersector> tankPicker = new osgUtil::LineSegmentIntersector(
	   osg::Vec3(tankXPosition, tankYPosition, 999),
      osg::Vec3(tankXPosition, tankYPosition, -999) );
   
   osgUtil::IntersectionVisitor ivTankElevation(tankPicker.get());
   terrainNode->accept(ivTankElevation);

   if (!tankPicker->containsIntersections())
   {
      std::cout << " couldn't place tank on terrain" << std::endl;
      return -1;
   }
   osgUtil::LineSegmentIntersector::Intersections listIntersection = tankPicker->getIntersections();
   osgUtil::LineSegmentIntersector::Intersections::iterator it = listIntersection.begin();
   osg::Vec3 terrainHeight = it->getWorldIntersectPoint();//获得交点坐标

   tankXform->setPosition( terrainHeight );
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

   osg::AlphaFunc* alphaFunction = new osg::AlphaFunc;
   alphaFunction->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
   billBoardStateSet->setAttributeAndModes( alphaFunction, osg::StateAttribute::ON );

   srand(time(0));  // Initialize random number generator.

   osgUtil::LineSegmentIntersector* terrainIsect[NUMBER_O_SHRUBS];

   /*! 随机构造线段 */
   int randomX, randomY;
   for (int i=0; i< NUMBER_O_SHRUBS; i++ )
   {  

	  int x=rand();
	  int y=rand();
      randomX = ( x % 200) + 1;
      randomY = ( y % 200) + 1;
      terrainIsect[i] = new osgUtil::LineSegmentIntersector(//垂直于地面的射线求交点坐标
         osg::Vec3(randomX, randomY, 999) ,
         osg::Vec3(randomX, randomY, -999) );
   }

   osg::Drawable* shrubDrawable[NUMBER_O_SHRUBS];
   int numToadd=0;
   for (int j = 0 ; j < NUMBER_O_SHRUBS; j ++)
   {
      float randomScale = ((rand() % 15) + 10 ) / 10.0;
      shrubDrawable[j] = createShrub( randomScale, billBoardStateSet);
	  osgUtil::IntersectionVisitor ivNode(terrainIsect[j]);
	  terrainNode->accept(ivNode);
      if (terrainIsect[j]->containsIntersections())
      { 
		  osgUtil::LineSegmentIntersector::Intersections listIntersection2 = terrainIsect[j]->getIntersections();
		  osgUtil::LineSegmentIntersector::Intersections::iterator it2 = listIntersection2.begin();
         osg::Vec3 shrubPosition = it2->getWorldIntersectPoint();

		 shrubBillBoard->addDrawable( shrubDrawable[j] , shrubPosition );
		 numToadd++;
      }
   }
   std::cout<<"一共添加了: "<<numToadd<<"个BillBoard!"<<std::endl;

   viewer.run();

   return 0;
}

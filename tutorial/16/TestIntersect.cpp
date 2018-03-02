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
   double tankXPosition = 5.0;
   double tankYPosition = -3.0;

   osgUtil::LineSegmentIntersector *tankLocationSegment = new osgUtil::LineSegmentIntersector(
	   osg::Vec3(tankXPosition, tankYPosition, 999),
      osg::Vec3(tankXPosition, tankYPosition, -999) );

   osgUtil::IntersectionVisitor findTankElevationVisitor(tankLocationSegment);
   terrainNode->accept(findTankElevationVisitor);

   osgUtil::IntersectionVisitor tankElevationLocatorHits;
   tankElevationLocatorHits = findTankElevationVisitor.getIntersector();
   if ( tankElevationLocatorHits.getint.empty() )
   {
      std::cout << " couldn't place tank on terrain" << std::endl;
      return -1;
   }
   heightTestResults = tankElevationLocatorHits.getfir.front();
   osg::Vec3 terrainHeight = heightTestResults.getWorldIntersectPoint();//��ý�������

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

   osgUtil::IntersectionVisitor isectVisitor;
   osgUtil::LineSegmentIntersector* terrainIsect[NUMBER_O_SHRUBS];

   int randomX, randomY;

   std::cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&�������Ϊ��"<<std::endl;
   for (int i=0; i< NUMBER_O_SHRUBS; i++ )
   {  

	  int x=rand();
	  int y=rand();
	  std::cout<<x<<","<<y<<std::endl;
      randomX = ( x % 200) + 1;
      randomY = ( y % 200) + 1;
      std::cout << randomX <<", " << randomY << std::endl;
      terrainIsect[i] = new osgUtil::LineSegmentIntersector(//��ֱ�ڵ���������󽻵�����
         osg::Vec3(randomX, randomY, 999) ,
         osg::Vec3(randomX, randomY, -999) );
      //isectVisitor.addLineSegment(terrainIsect[i]);
   }
   terrainNode->accept(isectVisitor);
   std::cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&�������Ϊ��"<<std::endl;

   osg::Drawable* shrubDrawable[NUMBER_O_SHRUBS];
   int numToadd=0;
   for (int j = 0 ; j < NUMBER_O_SHRUBS; j ++)
   {
      float randomScale = ((rand() % 15) + 10 ) / 10.0;
	  std::cout<<randomScale<<std::endl;
      shrubDrawable[j] = createShrub( randomScale, billBoardStateSet);
	  isectVisitor->accept(te)
      osgUtil::IntersectionVisitor hitList = isectVisitor.getHitList(terrainIsect[j]);
      if (! hitList.empty() )
      {    
         osgUtil::Hit firstHit = hitList.front();
         osg::Vec3 shrubPosition = firstHit.getWorldIntersectPoint();

         // osg::Vec3d shrubPosition = isectVisitor.getHitList(terrainIsect[j]).front().getWorldIntersectPoint();
         shrubBillBoard->addDrawable( shrubDrawable[j] , shrubPosition );
		 std::cout<<"������������������������������BillBoard�ɹ�!"<<std::endl;
		 numToadd++;
      }
	  else
	  {
		  std::cout<<"�޽���!"<<std::endl;
	  }
   }
   std::cout<<"һ��������: "<<numToadd<<"��BillBoard!"<<std::endl;

   viewer.run();

   return 0;
}
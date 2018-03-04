#include <osg/NodeCallback>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile> 
#include <osgDB/FileUtils>

#include "../Util/KeyboardHandler.h"

bool manuallyPlaceCamera = true;

void toggleView()
{
	std::cout << "toggle view" << std::endl;
	if (!manuallyPlaceCamera)
	{
		manuallyPlaceCamera = true;
	}
	else
	{
		manuallyPlaceCamera = false;
	}
}

int main()
{
   osg::Node* groundNode = NULL;
   osg::Node* tankNode = NULL;
   osg::Group* root = new osg::Group();
   osgViewer::Viewer viewer;
   osg::PositionAttitudeTransform* tankXform;

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../nps/T72-tank/");
   pathList.push_back("../nps/JoeDirt/");
   osgDB::setDataFilePathList(pathList);

   groundNode = osgDB::readNodeFile("JoeDirt.flt");
   tankNode = osgDB::readNodeFile("T72-tank_des.flt");

   // Create green Irish sky
   osg::ClearNode* backdrop = new osg::ClearNode;
   backdrop->setClearColor(osg::Vec4(0.0f,0.8f,0.0f,1.0f));
   root->addChild(backdrop);
   root->addChild(groundNode);

   tankXform = new osg::PositionAttitudeTransform();
   root->addChild(tankXform);
   tankXform->addChild(tankNode);

   tankXform->setPosition( osg::Vec3(10,10,8) );
   tankXform->setAttitude( osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,0,1) ) );

   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);
   viewer.setSceneData( root );
   viewer.realize();

   osg::Matrixd myCameraMatrix;
   osg::Matrixd cameraRotation;
   osg::Matrixd cameraTrans;
   cameraRotation.makeRotate(
      osg::DegreesToRadians(-20.0), osg::Vec3(0,1,0), // roll
      osg::DegreesToRadians(-15.0), osg::Vec3(1,0,0) , // pitch
      osg::DegreesToRadians( 10.0), osg::Vec3(0,0,1) ); // heading
   cameraRotation.makeIdentity();

   // 60 meters behind and 7 meters above the tank model 相对：10-(-50)=60
   cameraTrans.makeTranslate( 10,-50,15 );

   myCameraMatrix = cameraRotation * cameraTrans;

   keyboardEventHandler* keh = new keyboardEventHandler();
   keh->addFunction('v',toggleView);
   viewer.addEventHandler(keh); 

   viewer.realize();

   osg::Camera* mainCamera = viewer.getCamera();

   while( !viewer.done() )
   {
	  /************************************************************************/
	  // 通常osgGA::MatrixManipulator矩阵（osgProducer::Viewer中使用）使用的坐标
	  // 系为Z轴向上。但是Producer和osg::Matrix（也就是上文所创建的）使用Y轴向上
	  // 的坐标系系统。因此，在获得逆矩阵之后，我们需要将其从Y轴向上旋转到Z轴向
	  // 上的形式。这一要求可以通过沿X轴旋转-90度来实现。                                                                     */
	  /************************************************************************/

      if (manuallyPlaceCamera)	//为true（程序初始状态）: 每次都是由计算出的矩阵设置相机的方位
      {
         osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);
		 mainCamera->setViewMatrix(i * osg::Matrix::rotate(-osg::PI/2.0f,osg::Vec3(1.0f, 0.0f, 0.0f)));
      }

      viewer.frame();
   }

   return 0;
}
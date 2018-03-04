// Tutorial_11.3.cpp : Defines the entry point for the console application.
//参考osgChina.org   如何获取节点在世界坐标的位置   一节实现的 贺毅岳 08-5-21

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
 
#include <osg/MatrixTransform>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osg/PositionAttitudeTransform>
#include <osg/Transform>
#include <osgText/Text>
#include <osgText/Font> 

#include "../Util/KeyboardHandler.h"

bool manuallyPlaceCamera = true;

void toggleView()
{
	if (! manuallyPlaceCamera)
		manuallyPlaceCamera = true;
	else
		manuallyPlaceCamera = false;
}

class getWorldCoordOfNodeVisitor : public osg::NodeVisitor 
{
public:
	getWorldCoordOfNodeVisitor():
	  osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), done(false)
	  {
		  wcMatrix= new osg::Matrixd();
	  }
	  virtual void apply(osg::Node &node)
	  {
		  if (!done)
		  {
			  if ( 0 == node.getNumParents() ) //到达根节点，此时节点路径也已记录完整
			  {
				  wcMatrix->set( osg::computeLocalToWorld(this->getNodePath()) );
				  done = true;
			  }
			  traverse(node);
		  }
	  }
	  osg::Matrixd* giveUpDaMat() 
	  {
		  return wcMatrix;
	  }
private:
	bool done;
	osg::Matrix* wcMatrix;
};

//获得场景中节点的坐标
osg::Matrixd* getWorldCoords( osg::Node* node) 
{
	getWorldCoordOfNodeVisitor* ncv = new getWorldCoordOfNodeVisitor();
	if (node && ncv)
	{
		node->accept(*ncv);
		return ncv->giveUpDaMat();
	}
	else
	{
		return NULL;
	}
} 

bool addTextLabel(osg::Group* g, std::string s)
{
	if (!g)
	{
		return false;
	}
	osg::Geode* textLabelGeode = new osg::Geode();
	osgText::Text* textOne = new osgText::Text();
	g->addChild(textLabelGeode);
	textLabelGeode->addDrawable(textOne);

	textOne->setCharacterSize(1);
	textOne->setFont("../fonts/simfang.ttf");
	textOne->setText(s);
	textOne->setAxisAlignment(osgText::Text::XZ_PLANE);
	textOne->setColor( osg::Vec4(.5,.5,.25,1.0f) );
	textOne->setPosition( osg::Vec3(0,-5,4) );
	//textOne->setDrawMode(osgText::Text::TEXT |
	//                          osgText::Text::ALIGNMENT | 
	//                             osgText::Text::BOUNDINGBOX);
	textOne->setAlignment(osgText::Text::CENTER_TOP);
	return true;
}

class circleAimlessly : public osg::NodeCallback
{
public:
	circleAimlessly(): angle(0.0) {}
	 
	virtual void operator () (osg::Node * node, osg::NodeVisitor* nv)
	{
		osg::PositionAttitudeTransform* pat = dynamic_cast <osg::PositionAttitudeTransform*> (node);
		if (pat != NULL)
		{
			angle += osg::DegreesToRadians(1.0f); 
			pat->setPosition( osg::Vec3(cosf(angle)*100.0f, sinf(angle)*100.0f, 20.0f));
			pat->setAttitude(osg::Quat(angle, osg::Vec3(0,0,1)) );
		}
		
		traverse(node, nv);
	}
private:
	float angle;
};



int main()
{
	osg::Node* groundNode = NULL;
	osg::Node* tankNode = NULL;
	osg::Group* root = new osg::Group();
	osgViewer::Viewer viewer;
	
	osg::Matrixd  myCameraMatrix;	

	osgDB::FilePathList pathList = osgDB::getDataFilePathList();
	pathList.push_back("../nps/T72-tank/");
	pathList.push_back("../nps/JoeDirt/");
	osgDB::setDataFilePathList(pathList);

	groundNode = osgDB::readNodeFile("JoeDirt.flt");
	tankNode = osgDB::readNodeFile("T72-tank_des.flt");	 

	osg::PositionAttitudeTransform* tankPAT = new osg::PositionAttitudeTransform();
	tankPAT->setUpdateCallback(new circleAimlessly);
	root->addChild(tankPAT);
	tankPAT->addChild(tankNode);
	addTextLabel(tankPAT,"Follow Me!");

	osg::PositionAttitudeTransform* tankXform = new osg::PositionAttitudeTransform();
	//tankXform->setUpdateCallback(new circleAimlessly);
	root->addChild(groundNode);
	root->addChild(tankXform);
	tankXform->addChild(tankNode);
	addTextLabel(tankXform,"Follow Me!");

	tankXform->setPosition( osg::Vec3(10,10,8) );
	//tankXform->setAttitude( osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,0,1) ) );

	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.setSceneData( root );
	viewer.realize();

	//osg::Matrixd myCameraMatrix;
	//osg::Matrixd cameraRotation;
	//osg::Matrixd cameraTrans;
	////cameraRotation.makeRotate(
	////   osg::DegreesToRadians(-20.0), osg::Vec3(0,1,0), // roll
	////   osg::DegreesToRadians(-15.0), osg::Vec3(1,0,0) , // pitch
	////   osg::DegreesToRadians( 10.0), osg::Vec3(0,0,1) ); // heading
	//cameraRotation.makeIdentity();

	//// 60 meters behind and 7 meters above the tank model 相对：10-(-50)=60
	//cameraTrans.makeTranslate( 10,-50,15 );

	//myCameraMatrix = cameraRotation * cameraTrans;	

	keyboardEventHandler* keh = new keyboardEventHandler();
	keh->addFunction('v',toggleView);
	viewer.addEventHandler(keh); 

	osg::Camera *mainCamera = viewer.getCamera();
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
			myCameraMatrix = *( getWorldCoords(tankXform));
			osg::Matrix	cameraTrans,cameraRotation;
			cameraTrans.makeTranslate( 0,-50,5 );			
			cameraRotation.makeRotate(
			   osg::DegreesToRadians(-10.0), osg::Vec3(0,1,0), // roll
			   osg::DegreesToRadians(-5.0), osg::Vec3(1,0,0) , // pitch
			   osg::DegreesToRadians(-10.0), osg::Vec3(0,0,1) ); // heading
	    	myCameraMatrix = myCameraMatrix  * cameraRotation * cameraTrans;
		//	myCameraMatrix = cameraTrans * cameraRotation  * myCameraMatrix ;
			osg::Matrixd i = myCameraMatrix.inverse(myCameraMatrix);
			mainCamera->setViewMatrix(i * osg::Matrix::rotate(-osg::PI / 2.0f, osg::Vec3(1.0f, 0.0f, 0.0f)));
		}
		viewer.frame();
	}

	return 0;
}



 
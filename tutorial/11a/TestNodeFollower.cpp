#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osg/PositionAttitudeTransform>
#include <osg/Transform>
#include <osgText/Text>
#include <osgText/Font>

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
   //textOne->setColor( osg::Vec4(.5,.5,.25,1.0f) );
   textOne->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
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
      // What happens is this line is commented out???
      traverse(node, nv);
   }
private:
      float angle;
};

// class to allow access to matrix that represents accumlation of 
//  matrices above specified node of scene graph.
struct updateAccumulatedMatrix : public osg::NodeCallback
{
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      matrix = osg::computeWorldToLocal(nv->getNodePath() );
      traverse(node,nv);
   }
   osg::Matrix matrix;
};

struct transformAccumulator
{
   transformAccumulator();
   bool attachToGroup(osg::Group* g);
   osg::Matrix getMatrix();
protected:
   osg::ref_ptr<osg::Group> parent;
   osg::Node* node;
   updateAccumulatedMatrix* mpcb;
};

osg::Matrix transformAccumulator::getMatrix()
{
   return mpcb->matrix;
}
transformAccumulator::transformAccumulator()
{
   parent = NULL;
   node = new osg::Node;
   mpcb = new updateAccumulatedMatrix();
   node->setUpdateCallback(mpcb);
}

bool transformAccumulator::attachToGroup(osg::Group* g)
{
   bool success = false;
   //hyy: 以下代码的意思是从原来的父节点下删除 node，防止node(相机)为有多个父节点的情况
   if (parent != NULL)
   {
	  std::cout<<"parent != NULL"<<std::endl;
      int n = parent->getNumChildren();
      for (int i = 0; i < n; i++)
      {
         if (node == parent->getChild(i) )
         {
            parent->removeChild(i,1);
            success = true;
         }
      }
      if (! success)
      {
         return success;
      }
   }
   g->addChild(node);
   return true;
}

class followNodeMatrixManipulator : public osgGA::CameraManipulator
{
public:
   followNodeMatrixManipulator( transformAccumulator* ta) {worldCoordinatesOfNode = ta; theMatrix = osg::Matrixd::identity();}
   bool handle (const osgGA::GUIEventAdapter&ea, osgGA::GUIActionAdapter&aa);
   void updateTheMatrix();
   virtual void setByMatrix(const osg::Matrixd& mat); 
   virtual void setByInverseMatrix(const osg::Matrixd&mat);
   virtual osg::Matrixd getInverseMatrix() const;
   virtual osg::Matrixd getMatrix() const;
protected:
   ~followNodeMatrixManipulator() {}
   transformAccumulator* worldCoordinatesOfNode;
   osg::Matrixd theMatrix;//相机位置矩阵
};

void followNodeMatrixManipulator::setByMatrix(const osg::Matrixd& mat)
{
   theMatrix = mat;
}
void followNodeMatrixManipulator::setByInverseMatrix(const osg::Matrixd& mat)
{
   theMatrix = mat.inverse(mat);
}
void followNodeMatrixManipulator::updateTheMatrix()
{
   theMatrix = worldCoordinatesOfNode->getMatrix();
}

osg::Matrixd followNodeMatrixManipulator::getMatrix() const
{
	return theMatrix;
}

//这个是最重要的方法，这个方法每帧会被调用，它返回当前的视图矩阵。
osg::Matrixd followNodeMatrixManipulator::getInverseMatrix() const
{
   osg::Matrixd m;
   //return osg::Matrixd::inverse(matrix * osg::Matrixd::translate(_vPosition));//QS 73页
   //m = theMatrix * osg::Matrixd::rotate(-M_PI/2.0, osg::Vec3(1,0,0) );
   m =theMatrix * osg::Matrixd::rotate(-osg::PI/2,osg::Vec3(1,0,0));//// 将矩阵从Y轴向上旋转到Z轴向上
   return m;
}

bool followNodeMatrixManipulator::handle(const osgGA::GUIEventAdapter&ea, osgGA::GUIActionAdapter&aa)
{
   switch(ea.getEventType())
   {
   case (osgGA::GUIEventAdapter::FRAME):
      {
         updateTheMatrix();
         return false;
      }
   }
   return false;
}

int main( int argc, char **argv )
{
   osgViewer::Viewer viewer;
   osg::Group* rootNode = new osg::Group;

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../nps/T72-tank/");
   pathList.push_back("../nps/JoeDirt/");
   osgDB::setDataFilePathList(pathList);

   osg::Node* terrainNode = osgDB::readNodeFile("JoeDirt.flt");
   if (!terrainNode)
   {
      std::cout << " no terrain! " << std::endl;
      return NULL;
   }
   rootNode->addChild(terrainNode);

   osg::Node* tankNode = osgDB::readNodeFile("T72-tank_des.flt");
   if( ! tankNode)
   {
      std::cout << "no Tank" << std::endl;
      return NULL;
   }

   viewer.setSceneData( rootNode );
   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);

   // Create and set up a transform for updating the tank's
   // position.  (For now, this will move in a circle.)
   //osg::MatrixTransform* tankPAT = new osg::MatrixTransform();
   osg::PositionAttitudeTransform* tankPAT = new osg::PositionAttitudeTransform();

   tankPAT->setUpdateCallback(new circleAimlessly);
   rootNode->addChild(tankPAT);
   tankPAT->addChild(tankNode);
   addTextLabel(tankPAT,"Follow Me!");

   // Declare and set up a transform to 'follow' the tank node.
   osg::PositionAttitudeTransform *followerPAT = new osg::PositionAttitudeTransform();
   followerPAT->setPosition( osg::Vec3(0,-22,4) );
   followerPAT->setAttitude( osg::Quat( osg::DegreesToRadians(-10.0f) , 
      osg::Vec3(1,0,0) ));

   // Add this as a child of the tank's transform
   tankPAT->addChild(followerPAT);

   // create the windows and run the threads.
   viewer.realize();

   transformAccumulator* tankWorldCoords = new transformAccumulator();
   tankWorldCoords->attachToGroup(followerPAT);
   followNodeMatrixManipulator* followTank = new followNodeMatrixManipulator(tankWorldCoords);
   osgGA::KeySwitchMatrixManipulator *ksmm = new osgGA::KeySwitchMatrixManipulator;// viewer.getKeySwitchMatrixManipulator();
   if (!ksmm)
      return -1;
   // add the tank follower matrix manipulator. Selecting the 'f' key 
   // with switch the viewer to this manipulator.
   ksmm->addMatrixManipulator('m',"tankFollower",followTank);
   ksmm->selectMatrixManipulator(ksmm->getNumMatrixManipulators()-1);

   viewer.realize();
   viewer.run();

   return 0;
}
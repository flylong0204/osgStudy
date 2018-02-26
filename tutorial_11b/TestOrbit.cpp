#include <osg/NodeCallback>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile> 
#include <osgDB/FileUtils>

#include "../Util/KeyboardHandler.h"

class orbit : public osg::NodeCallback 
{
public:
   orbit(): heading(osg::PI/2.0) {}  

   osg::Matrix getWCMatrix(){ return worldCoordMatrix;} 

   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osg::MatrixTransform *tx = dynamic_cast<osg::MatrixTransform *>(node);
      if( tx != NULL )
      {
         heading +=  osg::PI/180.0;
		 //std::cout<<heading<<std::endl;
         osg::Matrixd orbitRotation;
         orbitRotation.makeRotate(  
            osg::DegreesToRadians(-10.0), osg::Vec3(0,1,0),  // roll
            osg::DegreesToRadians(-20.0), osg::Vec3(1,0,0) , // pitch
            heading, osg::Vec3(0, 0, 1) );                   // heading
         osg::Matrixd orbitTranslation;
         orbitTranslation.makeTranslate( 0,-42, 4 );
         tx->setMatrix ( orbitTranslation * orbitRotation);
		 worldCoordMatrix = osg::computeWorldToLocal( nv->getNodePath() );
      }

      traverse(node, nv);
   }
private:
   osg::Matrix worldCoordMatrix;//一直保存供设置camera用
   float heading;
};

bool useTankOrbiterView = true;

void toggleTankOrbiterView()
{
   if (! useTankOrbiterView)
      useTankOrbiterView = true;
   else
      useTankOrbiterView = false;
}



#include <osg/TextureCubeMap>
#include <osg/Depth>
#include <osg/ShapeDrawable>
#include <osg/TexEnv>
#include <osg/TexMat>
#include <osg/TexGen>

osg::TextureCubeMap* readCubeMap()
{
	osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;
	//#define CUBEMAP_FILENAME(face) "nvlobby_" #face ".png"
	//#define CUBEMAP_FILENAME(face) "Cubemap_axis/" #face ".png"
#define CUBEMAP_FILENAME(face) "../asserts/nps/skyBmp/" #face ".bmp"
	/*osgDB::FilePathList pathList = osgDB::getDataFilePathList();
	pathList.push_back("..\\data\\T72-tank\\");
	osgDB::setDataFilePathList(pathList);*/

	osg::Image* imagePosX = osgDB::readImageFile(CUBEMAP_FILENAME(front));
	osg::Image* imageNegX = osgDB::readImageFile(CUBEMAP_FILENAME(left));
	osg::Image* imagePosY = osgDB::readImageFile(CUBEMAP_FILENAME(down));
	osg::Image* imageNegY = osgDB::readImageFile(CUBEMAP_FILENAME(right));
	osg::Image* imagePosZ = osgDB::readImageFile(CUBEMAP_FILENAME(up));
	osg::Image* imageNegZ = osgDB::readImageFile(CUBEMAP_FILENAME(back));

	if (imagePosX && imageNegX && imagePosY  && imageNegY && imagePosZ && imageNegZ )
	{
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
		cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
		cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

		cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

		cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	}

	return cubemap;
}


// Update texture matrix for cubemaps
struct TexMatCallback : public osg::NodeCallback
{
public:

	TexMatCallback(osg::TexMat& tm) :
	  _texMat(tm)
	  {
	  }

	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	  {
		  osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		  if (cv)
		  {
			  const osg::Matrix* MV = cv->getModelViewMatrix();
			  const osg::Matrix R = osg::Matrix::rotate( osg::DegreesToRadians(112.0f), 0.0f,0.0f,1.0f)*
				  osg::Matrix::rotate( osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);

			  const osg::Quat q = MV->getRotate();
			  const osg::Matrix C = osg::Matrix::rotate( q.inverse() );

			  _texMat.setMatrix( C*R );
		  }

		  traverse(node,nv);
	  }

public:
	osg::TexMat& _texMat;
};


class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
	/** Get the transformation matrix which moves from local coords to world coords.*/
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(eyePointLocal));
		}
		return true;
	}

	/** Get the transformation matrix which moves from world coords to local coords.*/
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal));
		}
		return true;
	}
};


osg::Node* createSkyBox()
{
	osg::StateSet* stateset = new osg::StateSet();

	osg::TexEnv* te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

	osg::TexGen *tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

	osg::TexMat *tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm);

	osg::TextureCubeMap* skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

	// clear the depth to the far plane.
	 osg::Depth* depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0,1.0);   
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

//	stateset->setRenderBinDetails(-1,"RenderBin");

	osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),1));

	osg::Geode* geode = new osg::Geode;
	geode->setCullingActive(false);
	geode->setStateSet( stateset );
	geode->addDrawable(drawable);


	osg::Transform* transform = new MoveEarthySkyWithEyePointTransform;
	transform->setCullingActive(false);
	transform->addChild(geode);

	osg::ClearNode* clearNode = new osg::ClearNode;
	//   clearNode->setRequiresClear(false);
	clearNode->setCullCallback(new TexMatCallback(*tm));
	clearNode->addChild(transform);

	return clearNode;
}


int main()
{
   osg::Node* groundNode = NULL;
   osg::Node* tankNode = NULL;
   osg::Group* root = NULL;
   osgViewer::Viewer viewer;
   osg::PositionAttitudeTransform* tankXform = NULL;

   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("../asserts/nps/T72-tank/");
   pathList.push_back("../asserts/nps/JoeDirt/");;
   osgDB::setDataFilePathList(pathList);

   groundNode = osgDB::readNodeFile("JoeDirt.flt");
   tankNode = osgDB::readNodeFile("T72-tank_des.flt");

   root = new osg::Group();

   // Create green Irish sky
   osg::ClearNode* backdrop =dynamic_cast<osg::ClearNode*>( createSkyBox());   
   root->addChild(backdrop);

   tankXform = new osg::PositionAttitudeTransform();

   root->addChild(groundNode);

   root->addChild(tankXform);
   tankXform->addChild(tankNode);

   tankXform->setPosition( osg::Vec3(10,10,8) );
   tankXform->setAttitude( 
      osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,0,1) ) );

   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.addEventHandler(new osgViewer::WindowSizeHandler);

   viewer.setSceneData( root );
   viewer.realize();

   osg::MatrixTransform* orbitTankXForm = new osg::MatrixTransform();
   orbit* tankOrbitCallback = new orbit();
   orbitTankXForm->setUpdateCallback( tankOrbitCallback );
   tankXform->addChild(orbitTankXForm);

   keyboardEventHandler* keh = new keyboardEventHandler();
   keh->addFunction('v',toggleTankOrbiterView);
   viewer.addEventHandler(keh); 

   osg::Camera *mainCamera = viewer.getCamera();

   while( !viewer.done() )
   {
      if (useTankOrbiterView)
      {
		  mainCamera->setViewMatrix(osg::Matrix(tankOrbitCallback->getWCMatrix() *
			  osg::Matrix::rotate(-osg::PI / 2.0f, osg::Vec3(1.0f, 0.0f, 0.0f))));
      }

      viewer.frame();
   }
}
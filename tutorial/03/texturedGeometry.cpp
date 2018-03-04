#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile> 
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>

#include "../../Util/Axis.h"

osg::Geode* createPyramid()
{
   osg::ref_ptr<osg::Geode> pyramidGeode = new osg::Geode();
   osg::ref_ptr<osg::Geometry> pyramidGeometry = new osg::Geometry();
    
   // Specify the vertices:
   osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array;
   pyramidVertices->push_back( osg::Vec3(0, 0, 0) ); // front left 
   pyramidVertices->push_back( osg::Vec3(2, 0, 0) ); // front right 
   pyramidVertices->push_back( osg::Vec3(2, 2, 0) ); // back right 
   pyramidVertices->push_back( osg::Vec3( 0,2, 0) ); // back left 
   pyramidVertices->push_back( osg::Vec3( 1, 1,2) ); // peak 
   pyramidGeometry->setVertexArray( pyramidVertices.get() );

   /*! 底部 */
   osg::ref_ptr<osg::DrawElementsUInt> pyramidBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
   pyramidBase->push_back(3);
   pyramidBase->push_back(2);
   pyramidBase->push_back(1);
   pyramidBase->push_back(0);
   pyramidGeometry->addPrimitiveSet(pyramidBase.get());

   /*! 前面 */
   osg::ref_ptr<osg::DrawElementsUInt> pyramidFaceOne = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceOne->push_back(0);
   pyramidFaceOne->push_back(1);
   pyramidFaceOne->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceOne.get());
   /*! 右面 */
   osg::ref_ptr<osg::DrawElementsUInt> pyramidFaceTwo = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceTwo->push_back(1);
   pyramidFaceTwo->push_back(2);
   pyramidFaceTwo->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceTwo.get());
   /*! 后面 */
   osg::ref_ptr<osg::DrawElementsUInt> pyramidFaceThree = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceThree->push_back(2);
   pyramidFaceThree->push_back(3);
   pyramidFaceThree->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceThree.get());
   /*! 左面 */
   osg::ref_ptr<osg::DrawElementsUInt> pyramidFaceFour = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceFour->push_back(3);
   pyramidFaceFour->push_back(0);
   pyramidFaceFour->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceFour.get());
   /*! 设置每个顶点的颜色值 */
   osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
   colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
   colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
   colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
   colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white
   colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
   pyramidGeometry->setColorArray(colors.get(), osg::Array::BIND_PER_VERTEX);
  
   pyramidGeode->addDrawable(pyramidGeometry.get());

   /*! 
     设置纹理坐标值
   */
   // Since the mapping from vertices to texture coordinates is 1:1, 
   //  we don't need to use an index array to map vertices to texture
   //  coordinates. We can do it directly with the 'setTexCoordArray' 
   //  method of the Geometry class. 
   //  This method takes a variable that is an array of two dimensional
   //  vectors (osg::Vec2). This variable needs to have the same
   //  number of elements as our Geometry has vertices. Each array element
   //  defines the texture coordinate for the cooresponding vertex in the
   //  vertex array.
   osg::Vec2Array* texcoords = new osg::Vec2Array(5);
   (*texcoords)[0].set(0.00f,0.0f); // tex coord for vertex 0 
   (*texcoords)[1].set(0.25f,0.0f); // tex coord for vertex 1 
   (*texcoords)[2].set(0.50f,0.0f); //  ""
   (*texcoords)[3].set(0.75f,0.0f); //  "" 
   (*texcoords)[4].set(0.50f,1.0f); //  ""
   pyramidGeometry->setTexCoordArray(0,texcoords);

   return pyramidGeode.release();
}

#include <osg/TextureCubeMap>
#include <osg/Depth>
#include <osg/ShapeDrawable>
#include <osg/TexEnv>
#include <osg/TexMat>
#include <osg/TexGen>

osg::TextureCubeMap* readCubeMap()
{
	osg::ref_ptr<osg::TextureCubeMap> cubemap = new osg::TextureCubeMap;
#define CUBEMAP_FILENAME(face) "../nps/skyBmp/" #face ".bmp"

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

	return cubemap.release();
}


// Update texture matrix for cubemaps
struct TexMatCallback : public osg::NodeCallback
{
public:
	TexMatCallback(osg::TexMat& tm) 
		: _texMat(tm)
	  {}

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
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	
	osg::StateSet* stateset = geode->getOrCreateStateSet();

	osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv;
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te.get(), osg::StateAttribute::ON );

	osg::ref_ptr<osg::TexGen> tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0, tg.get(), osg::StateAttribute::ON);

	osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm.get());

	osg::TextureCubeMap* skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

	// clear the depth to the far plane.
	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0,1.0);   
	stateset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON );

	stateset->setRenderBinDetails(-1,"RenderBin");

	geode->setCullingActive(false);
	geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 1)));


	osg::ref_ptr<osg::Transform> transform = new MoveEarthySkyWithEyePointTransform;
	transform->setCullingActive(false);
	transform->addChild(geode);

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	//   clearNode->setRequiresClear(false);
	clearNode->setCullCallback(new TexMatCallback(*tm));
	clearNode->addChild(transform.get());

	return clearNode.release();
}


osg::Node* createLights(osg::ref_ptr<osg::StateSet> ssRoot)
{
	/*!
	  很不明白这里，明明开启两个灯光后，就会出现一个错误
	  Warning: detected OpenGL error 'invalid value' at after RenderBin::draw(..)
	*/
	ssRoot->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ssRoot->setMode(GL_LIGHT0, osg::StateAttribute::ON);
	ssRoot->setMode(GL_LIGHT1, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Group> lightGroup = new osg::Group;
	// create a directional light.
	osg::ref_ptr<osg::Light> myLight = new osg::Light;
	myLight->setLightNum(0);
	myLight->setPosition(osg::Vec4(0.0,0.0,100.0,0.0f));
	myLight->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	myLight->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

	osg::ref_ptr<osg::LightSource> lightS = new osg::LightSource;    
	lightS->setLight(myLight.get());
	lightS->setLocalStateSetModes(osg::StateAttribute::ON); 

	lightS->setStateSetModes(*ssRoot,osg::StateAttribute::ON);

	lightGroup->addChild(lightS);

	return lightGroup.release();
}

int main()
{
	osgViewer::Viewer viewer;

   osg::Group* root = new osg::Group();

   osg::Node* sky=createSkyBox();
   root->addChild(sky);

   osg::Geode* pyramidGeode = createPyramid();
   root->addChild(pyramidGeode);

   root->addChild(createLights(root->getOrCreateStateSet()));  
   
   root->addChild(Util::Axis::createAxis(1.0f, 1.0f, 1.0f));


   /*!
     给节点添加一个纹理状态对象
   */
   // Create a new StateSet with default settings: 
   osg::StateSet* stateOne = pyramidGeode->getOrCreateStateSet();
   osg::Texture2D* KLN89FaceTexture = new osg::Texture2D;
   KLN89FaceTexture->setDataVariance(osg::Object::DYNAMIC);
   // load an image by reading a file: 
   osg::Image* klnFace = osgDB::readImageFile("../nps/KLN89FaceB.tga");
   if (!klnFace)
   {
	   std::cout << " couldn't find texture, quiting." << std::endl;
	   return -1;
   }
   // Assign the texture to the image we read from file: 
   KLN89FaceTexture->setImage(klnFace);

   stateOne->setTextureAttributeAndModes
	   (0,KLN89FaceTexture,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

   viewer.addEventHandler(new osgViewer::StatsHandler);
   viewer.setUpViewInWindow(100, 100, 800, 600);
   viewer.setSceneData( root );

   viewer.realize();
   viewer.run();

   return 0;
}
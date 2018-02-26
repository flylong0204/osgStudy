#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile> 
//#include <osgProducer/Viewer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/PositionAttitudeTransform>

#include "../Util/Axis.h"

osg::Geode* createPyramid()
{
   osg::Geode* pyramidGeode = new osg::Geode();
   osg::Geometry* pyramidGeometry = new osg::Geometry();
   pyramidGeode->addDrawable(pyramidGeometry); 
   // Specify the vertices:
   osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
   pyramidVertices->push_back( osg::Vec3(0, 0, 0) ); // front left 
   pyramidVertices->push_back( osg::Vec3(2, 0, 0) ); // front right 
   pyramidVertices->push_back( osg::Vec3(2, 2, 0) ); // back right 
   pyramidVertices->push_back( osg::Vec3( 0,2, 0) ); // back left 
   pyramidVertices->push_back( osg::Vec3( 1, 1,2) ); // peak
   
   //Associate this set of vertices with the geometry associated with the
   //geode we added to the scene.
   pyramidGeometry->setVertexArray( pyramidVertices );

   osg::DrawElementsUInt* pyramidBase = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
   pyramidBase->push_back(3);
   pyramidBase->push_back(2);
   pyramidBase->push_back(1);
   pyramidBase->push_back(0);
   pyramidGeometry->addPrimitiveSet(pyramidBase);

   //Repeat the same for each of the four sides. Again, vertices are 
   //specified in counter-clockwise order. 

   osg::DrawElementsUInt* pyramidFaceOne = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceOne->push_back(0);
   pyramidFaceOne->push_back(1);
   pyramidFaceOne->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

   osg::DrawElementsUInt* pyramidFaceTwo = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceTwo->push_back(1);
   pyramidFaceTwo->push_back(2);
   pyramidFaceTwo->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

   osg::DrawElementsUInt* pyramidFaceThree = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceThree->push_back(2);
   pyramidFaceThree->push_back(3);
   pyramidFaceThree->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

   osg::DrawElementsUInt* pyramidFaceFour = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceFour->push_back(3);
   pyramidFaceFour->push_back(0);
   pyramidFaceFour->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceFour);
   
   osg::Vec4Array* colors = new osg::Vec4Array;
   colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
   colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
   colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
   colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white
   osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> 
      *colorIndexArray;colorIndexArray = new 
      osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
   colorIndexArray->push_back(0); // vertex 0 assigned color array element 0
   colorIndexArray->push_back(1); // vertex 1 assigned color array element 1
   colorIndexArray->push_back(2); // vertex 2 assigned color array element 2
   colorIndexArray->push_back(3); // vertex 3 assigned color array element 3
   colorIndexArray->push_back(0); // vertex 4 assigned color array element 0
   pyramidGeometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
   //pyramidGeometry->setColorIndices(colorIndexArray);
   //pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

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
   return pyramidGeode;
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
	stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON );

	osg::TexGen *tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

	osg::TexMat *tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm);

	osg::TextureCubeMap* skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

	// clear the depth to the far plane.
	osg::Depth* depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0,1.0);   
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

	stateset->setRenderBinDetails(-1,"RenderBin");

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


osg::Node* createLights(osg::StateSet* rootStateSet)
{
	osg::Group* lightGroup = new osg::Group;

	// create a positional light.
	osg::Light* myLight1 = new osg::Light;
	myLight1->setLightNum(0);
	myLight1->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));    //最后一个参数如果不为0,则为方向性光源
	myLight1->setAmbient(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	myLight1->setDiffuse(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	myLight1->setSpotCutoff(1);
	myLight1->setSpotExponent(150.0f);
	myLight1->setDirection(osg::Vec3(0.0f,1.0f,0.0f));

	osg::LightSource* lightS1 = new osg::LightSource;    
	lightS1->setLight(myLight1);
	lightS1->setLocalStateSetModes(osg::StateAttribute::ON); 
	lightS1->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
	//lightS1->setUpdateCallback(new CLightUpdateCallback(tracker.get()));
	//lightGroup->addChild(lightS1);

	// create a directional light.
	osg::Light* myLight2 = new osg::Light;
	myLight2->setLightNum(1);
	myLight2->setPosition(osg::Vec4(0.0,0.0,100.0,0.0f));
	myLight2->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	myLight2->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

	osg::LightSource* lightS2 = new osg::LightSource;    
	lightS2->setLight(myLight2);
	lightS2->setLocalStateSetModes(osg::StateAttribute::ON); 

	lightS2->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);

	lightGroup->addChild(lightS2);

	return lightGroup;
}

int main()
{
   //osgProducer::Viewer viewer;
	osgViewer::Viewer viewer;

   // Declare a group to act as root node of a scene:
   osg::Group* root = new osg::Group();

   osg::Node* sky=createSkyBox();
   root->addChild(sky);
   osg::Geode* pyramidGeode = createPyramid();

   root->addChild(createLights(root->getOrCreateStateSet()));
   
   root->addChild(pyramidGeode);
   root->addChild(Axis::createAxis(1.0f, 1.0f, 1.0f));

   osg::Texture2D* KLN89FaceTexture = new osg::Texture2D;
   // protect from being optimized away as static state:
   KLN89FaceTexture->setDataVariance(osg::Object::DYNAMIC); 

   // load an image by reading a file: 
   osg::Image* klnFace = osgDB::readImageFile("../asserts/nps/KLN89FaceB.tga");
   if (!klnFace)
   {
      std::cout << " couldn't find texture, quiting." << std::endl;
      return -1;
   }

   // Assign the texture to the image we read from file: 
   KLN89FaceTexture->setImage(klnFace);

   // Create a new StateSet with default settings: 
   osg::StateSet* stateOne = new osg::StateSet();

   // Assign texture unit 0 of our new StateSet to the texture 
   //爓e just created and enable the texture.
   stateOne->setTextureAttributeAndModes
	   (0,KLN89FaceTexture,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

   // Associate this state set with the Geode that contains
   //the pyramid: 
   pyramidGeode->setStateSet(stateOne);
   
   //The final step is to set up and enter a simulation loop.
   //viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
   viewer.setUpViewInWindow(100, 100, 800, 600);
   viewer.setSceneData( root );

   viewer.realize();

   viewer.addEventHandler(new osgViewer::StatsHandler);
   //while( !viewer.done() )
   //{
   //   viewer.sync();
   //   viewer.update();
   //   viewer.frame();
   //}
   viewer.run();

   return 0;
}
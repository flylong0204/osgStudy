#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Notify>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>

#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/ShapeDrawable>

#include "../../Util/Axis.h"

osg::Geode* createPyramid()
{
   osg::Geode* pyramidGeode = new osg::Geode();
   osg::Geometry* pyramidGeometry = new osg::Geometry();
   pyramidGeode->addDrawable(pyramidGeometry); 

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
      new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 3);
   pyramidBase->push_back(3);
   pyramidBase->push_back(2);
   pyramidBase->push_back(1);
   pyramidBase->push_back(0);
   pyramidGeometry->addPrimitiveSet(pyramidBase);

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

   /*! 设置每个顶点的颜色值 */
   osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
   colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); //index 0 red
   colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); //index 1 green
   colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); //index 2 blue
   colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)); //index 3 white
   colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
   pyramidGeometry->setColorArray(colors.get(), osg::Array::BIND_PER_VERTEX);


   osg::Vec2Array* texcoords = new osg::Vec2Array(5);
   (*texcoords)[0].set(0.00f,0.0f);
   (*texcoords)[1].set(0.25f,0.0f);
   (*texcoords)[2].set(0.50f,0.0f);
   (*texcoords)[3].set(0.75f,0.0f);
   (*texcoords)[4].set(0.50f,1.0f);

   pyramidGeometry->setTexCoordArray(0,texcoords);
   return pyramidGeode;
}


int main()
{
	osgViewer::Viewer viewer;

   // Declare a group to act as root node of a scene:
   osg::Group* root = new osg::Group();

   /*! 单位立方体 */
   // Declare a box class (derived from shape class) instance
   // This constructor takes an osg::Vec3 to define the center
   //  and a float to define the height, width and depth.
   //  (an overloaded constructor allows you to specify unique
   //   height, width and height values.)
   osg::Box* unitCube = new osg::Box( osg::Vec3(0,0,0), 1.0f);
   unitCube->setDataVariance(osg::Object::DYNAMIC);

   // Declare an instance of the shape drawable class and initialize 
   //  it with the unitCube shape we created above.
   //  This class is derived from 'drawable' so instances of this
   //  class can be added to Geode instances.
   osg::ShapeDrawable* unitCubeDrawable = new osg::ShapeDrawable(unitCube);

   // Declare a instance of the geode class: 
   osg::Geode* unitCubeGeode = new osg::Geode();
   unitCubeGeode->addDrawable(unitCubeDrawable);

   // Add the goede to the scene:
   root->addChild(unitCubeGeode);

   /*! 单位球体 */
   osg::Sphere* unitSphere = new osg::Sphere( osg::Vec3(0,0,0), 1.0);
   osg::ShapeDrawable* unitSphereDrawable = new osg::ShapeDrawable(unitSphere);
   unitSphereDrawable->setColor( osg::Vec4(0.1, 0.1, 0.1, 0.1) );

   osg::PositionAttitudeTransform* unitSphereXForm = new osg::PositionAttitudeTransform();
   unitSphereXForm->setPosition(osg::Vec3(3.0,0,0));

   osg::Geode* unitSphereGeode = new osg::Geode();
   root->addChild(unitSphereXForm);
   unitSphereXForm->addChild(unitSphereGeode);
   unitSphereGeode->addDrawable(unitSphereDrawable);

   /*! 四面体金字塔 */
   osg::Geode* pyramidGeode = createPyramid();
   
   osg::PositionAttitudeTransform* pyramidXForm = 
      new osg::PositionAttitudeTransform();
   pyramidXForm->setPosition(osg::Vec3(0,-3.0,0));
   root->addChild(pyramidXForm);
   pyramidXForm->addChild(pyramidGeode);

   /*! 加载纹理图片 */
   osgDB::FilePathList pathList = osgDB::getDataFilePathList();
   pathList.push_back("F:/osg/OpenSceneGraphData/Images");
   osgDB::setDataFilePathList(pathList);

   osg::ref_ptr<osg::Texture2D> KLN89FaceTexture = new osg::Texture2D;
   //osg::setNotifyLevel(osg::DEBUG_INFO);
   // protect from being optimized away as static state:
   KLN89FaceTexture->setDataVariance(osg::Object::DYNAMIC); 
   osg::ref_ptr<osg::Image> klnFace = osgDB::readImageFile(
	   //"skymap.jpg"
	   "../nps/KLN89FaceB.tga"
   );
   if (!klnFace)
   {
      std::cout << " couldn't load texture, quitting." << std::endl;
      return -1;
   }
   KLN89FaceTexture->setImage(klnFace.get());

   /*! 纹理的环境模式为 blend */
   osg::StateSet* blendStateSet = new osg::StateSet();
   osg::TexEnv* blendTexEnv = new osg::TexEnv;
   blendTexEnv->setMode(osg::TexEnv::BLEND);
   blendStateSet->setTextureAttributeAndModes(0,KLN89FaceTexture.get(),osg::StateAttribute::ON);
   blendStateSet->setTextureAttribute(0,blendTexEnv, osg::StateAttribute::ON);

   /*! 纹理的环境模式为 DECAL*/
   osg::StateSet* decalStateSet = new osg::StateSet();
   osg::TexEnv* decalTexEnv = new osg::TexEnv();
   decalTexEnv->setMode(osg::TexEnv::DECAL);
   decalStateSet->setTextureAttributeAndModes(0,KLN89FaceTexture.get(),osg::StateAttribute::ON);
   decalStateSet->setTextureAttribute(0,decalTexEnv, osg::StateAttribute::ON);

   //osg::StateSet *ssPyramid = pyramidGeode->getOrCreateStateSet();
   //ssPyramid->setTextureAttributeAndModes(0, KLN89FaceTexture.get(), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);

   root->setStateSet(blendStateSet);
   //unitCubeGeode->setStateSet(blendStateSet);
   unitSphereGeode->setStateSet(decalStateSet);

   root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

   root->addChild(Util::Axis::createAxis(5.0f, 5.0f, 5.0f));

   viewer.setUpViewInWindow(100, 100, 1024, 768);
   viewer.setSceneData( root );

   viewer.realize();
   viewer.run();

   return 0;
}

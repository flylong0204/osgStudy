
/*!
   ´´½¨×ø±êÖá
*/
#ifndef MEIJIE_AXIS_H
#define MEIJIE_AXIS_H 1

#include <osg/Node>
#include <osg/Geometry>
#include <osg/Geode>

class Axis
{
public:
	static osg::Node* createAxis(float ux, float uy, float uz)
	{
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry());

		osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array());
		vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
		vertices->push_back(osg::Vec3(ux, 0.0, 0.0));
		vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
		vertices->push_back(osg::Vec3(0.0, uy, 0.0));
		vertices->push_back(osg::Vec3(0.0, 0.0, 0.0));
		vertices->push_back(osg::Vec3(0.0, 0.0, uz));
		geometry->setVertexArray(vertices.get());

		osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array());
		colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));  // x -- red
		colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // y - green
		colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
		colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); // z -blue
		colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
		geometry->setColorArray(colors.get(), osg::Array::BIND_PER_VERTEX);
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));

		geode->addDrawable(geometry.get());
		geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
		return geode.release();
	}

};

#endif // !MEIJIE_AXIS_H
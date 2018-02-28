
#include "Util.h"

#include <osg/Geometry>
#include <osg/ShapeDrawable>

osg::Geode * createBox(osg::Vec3 center)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(new osg::Box(center, 0.5f, 0.5f, 0.5f));
	sd->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	geode->addDrawable(sd);
	return geode.release();
}

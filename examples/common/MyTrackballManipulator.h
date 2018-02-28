#ifndef __MY_TRACKBALL_MANIPULATOR_H_
#define __MY_TRACKBALL_MANIPULATOR_H_

#include <osgGA/TrackballManipulator>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>

class MyTrackballManipulator : public osgGA::TrackballManipulator
{
public:
	virtual bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);
	virtual bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy);
	virtual bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy);
	virtual bool performMouseDeltaMovement(const float dx, const float dy);
	virtual void applyAnimationStep(const double currentProgress, const double prevProgress);
};

#endif
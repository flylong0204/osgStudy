#include "MyTrackballManipulator.h"

bool MyTrackballManipulator::handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	return true;
}

// 重写旋转事件
bool MyTrackballManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	if (getVerticalAxisFixed())
		rotateWithFixedVertical(dx, dy);
	else
		rotateTrackball(_ga_t0->getXnormalized(), _ga_t0->getYnormalized(),
			_ga_t1->getXnormalized(), _ga_t1->getYnormalized(),
			getThrowScale(eventTimeDelta));
	return true;
}

bool MyTrackballManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	return true;
}

bool MyTrackballManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
	return true;
}

bool MyTrackballManipulator::performMouseDeltaMovement(const float dx, const float dy)
{
	return true;
}

void MyTrackballManipulator::applyAnimationStep(const double currentProgress, const double prevProgress)
{
}
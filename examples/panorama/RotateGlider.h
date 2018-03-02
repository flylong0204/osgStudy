
#ifndef  ROTATE_GLIDER_H
#define ROTATE_GLIDER_H 1

#include <osgGA/GUIEventHandler>

class CRotateGlider : public osgGA::GUIEventHandler
{
public:
	CRotateGlider();
	~CRotateGlider();

public:
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};

#endif // ! ROTATE_GLIDER_H
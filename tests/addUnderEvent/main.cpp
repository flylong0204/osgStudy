
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/GUIEventHandler>
#include <osgDB/ReadFile>
#include <iostream>
#include "../../Util/Axis.h"
#include "../../Util/Shape.h"


class keyboardHandler : public osgGA::GUIEventHandler
{
public:
	keyboardHandler()
	{}

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *obj, osg::NodeVisitor *nv)
	{
		switch (ea.getEventType())
		{
		case (osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey() == 'a')
			{
				osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
				if (viewer)
				{
					osg::Group* root = dynamic_cast<osg::Group*>(viewer->getSceneData());
					if (root)
					{
						std::srand(time(NULL));
						float x = 0.5 + std::rand()%100/100.0f;
						float y = 0.5 + std::rand() % 100 / 100.0f;
						float z = 0.5 + std::rand() % 100 / 100.0f;
						root->addChild(Util::Shape::createBox(osg::Vec3(x, y, z)));
						return true;
					}
				}
			}
		}
			break;
		case (osgGA::GUIEventAdapter::FRAME):
			break;
		case (osgGA::GUIEventAdapter::USER):
			break;
		}
		return false;
	}
private:
};

int main( int argc, char** argv )
{
	osgViewer::Viewer viewer;

	viewer.addEventHandler(new keyboardHandler);
	viewer.setUpViewInWindow(100, 100, 1024, 768);
	viewer.realize();

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(osgDB::readNodeFile("../models/cow.osg"));
	root->addChild(Util::Axis::createAxis(100.0f, 100.0f, 100.0f));
	viewer.setSceneData(root.get());
	viewer.run();

	return 0;
}

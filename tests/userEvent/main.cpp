
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/GUIEventHandler>
#include <iostream>
struct TimerInfo : public osg::Referenced
{
	TimerInfo(unsigned int c) : count(c)
	{}
	unsigned int count;
};

class TimerHandler : public osgGA::GUIEventHandler
{
public:
	TimerHandler(TimerInfo *ti)
		: timerInfo(ti)
	{}

	bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *obj, osg::NodeVisitor *nv)
	{
		switch (ea.getEventType())
		{
		case (osgGA::GUIEventAdapter::FRAME):
		{
			osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
			if (viewer && (timerInfo->count % 100 == 0))
			{
				viewer->getEventQueue()->userEvent(timerInfo);
			}
			timerInfo->count++;
		}
			break;
		case (osgGA::GUIEventAdapter::USER):
		{
			const TimerInfo *ti = dynamic_cast<const TimerInfo*>(ea.getUserData());
			std::cout << "the " << int(ti->count/100) << "th 100 frame " << std::endl;
		}
			break;
		}
		return false;
	}
private:
	osg::ref_ptr<TimerInfo> timerInfo;
};

int main( int argc, char** argv )
{
	osgViewer::Viewer viewer;

	viewer.addEventHandler(new TimerHandler(new TimerInfo(0)));
	viewer.setUpViewInWindow(100, 100, 1024, 768);
	viewer.realize();

	viewer.run();

	return 0;
}

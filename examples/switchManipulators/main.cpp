
#include <osg/MatrixTransform>

#include <osgDB/ReadFile>

#include <osgGA/NodeTrackerManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/UFOManipulator>
#include <osgGA/SphericalManipulator>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#define OSG_WIDTH 1280
#define OSG_HEIGHT 960

osg::AnimationPath* createAnimationPath(const osg::Vec3 &center, float radius, double loopTime)
{
	osg::ref_ptr<osg::AnimationPath> aniPath = new osg::AnimationPath;
	aniPath->setLoopMode(osg::AnimationPath::LOOP);
	
	const int numSamples = 40;

	float yaw = 0.0f;
	float yaw_delta = 2.0f * osg::PI / ((float)numSamples - 1.0f);
	float roll = osg::inDegrees(30.0f);

	double time = 0.0f;
	double time_delta = loopTime / (double)numSamples;

	for (int i = 0; i < numSamples; i++)
	{
		osg::Vec3 pos(sin(yaw)*radius, cos(yaw)*radius, 0);

		osg::Quat rotation(osg::Quat(roll, osg::Vec3(0.0f, 1.0f, 0.0f)) 
			//* osg::Quat(-(yaw + osg::inDegrees(90.0)), osg::Vec3(0.0f, 0.0f, 1.0f))
		);
		aniPath->insert(time, osg::AnimationPath::ControlPoint(pos, rotation));
		yaw += yaw_delta;
		time += time_delta;
	}

	return aniPath.release();
}
osg::AnimationPath* createAnimationPath(const osg::Vec3 &sPos, const osg::Vec3 &ePos,
	double sTime, double eTime)
{
	osg::ref_ptr<osg::AnimationPath> aniPath = new osg::AnimationPath;
	aniPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
	aniPath->insert(sTime, osg::AnimationPath::ControlPoint(sPos));
	aniPath->insert(eTime, osg::AnimationPath::ControlPoint(ePos));
	return aniPath.release();
}

osgGA::NodeTrackerManipulator* viewerManipulator_NodeTrack(osg::Group &root)
{
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	osg::Node*glider = osgDB::readNodeFile("../models/glider.osg");
	mt->addChild(glider);

	osg::ref_ptr<osg::AnimationPath> path = createAnimationPath(
		//osg::Vec3(0.0f, 0.0f, 0.0f), 10.0f, 3.0f
		osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(1000, 0, 0), 0.0, 1000.0
	);
	mt->setUpdateCallback(new osg::AnimationPathCallback(path.get()));
	
	osg::ref_ptr<osgGA::NodeTrackerManipulator> tm = new osgGA::NodeTrackerManipulator;
	tm->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
	tm->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
	tm->setTrackNode(glider);

	root.addChild(mt);

	return tm.release();
}

osgGA::AnimationPathManipulator* viewerManipulator_AnimationPath(osg::Group &root)
{
	osg::ref_ptr<osg::AnimationPath> path = createAnimationPath(
		//osg::Vec3(0.0f, 0.0f, 0.0f), 10.0f, 3.0f
		osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(100.0f, 0.0f, 0.0f), 0.0, 1000.0
	);

	osg::MatrixTransform* mt = new osg::MatrixTransform;
	mt->addChild(osgDB::readNodeFile("../models/glider.osg"));
		
	mt->setEventCallback(new osg::AnimationPathCallback(path.get()));

	root.addChild(mt);

	osg::ref_ptr<osgGA::AnimationPathManipulator> pAn = new osgGA::AnimationPathManipulator(path.get());
	return pAn.release();
}

int main(int argc, char*argv[])
{
	osgViewer::Viewer viewer;
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);

	osg::Group *root = new osg::Group;
	root->addChild(osgDB::readNodeFile("../models/cow.osg"));

	root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	root->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

	viewer.setSceneData(root);

	osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator =
		new osgGA::KeySwitchMatrixManipulator;

	osgGA::NodeTrackerManipulator *ntm = viewerManipulator_NodeTrack((*root));
	keyswitchManipulator->addMatrixManipulator('1', "Node Track", ntm);
	keyswitchManipulator->addMatrixManipulator('2', "Track Ball", new osgGA::TrackballManipulator);
	keyswitchManipulator->addMatrixManipulator('3', "Flight", new osgGA::FlightManipulator);
	keyswitchManipulator->addMatrixManipulator('4', "Drive", new osgGA::DriveManipulator);
	keyswitchManipulator->addMatrixManipulator('5', "Terrain", new osgGA::TerrainManipulator);
	keyswitchManipulator->addMatrixManipulator('6', "FirstPersonalShooting", new osgGA::FirstPersonManipulator);
	keyswitchManipulator->addMatrixManipulator('7', "UFO", new osgGA::UFOManipulator);
	osgGA::AnimationPathManipulator *apm = viewerManipulator_AnimationPath((*root));
	keyswitchManipulator->addMatrixManipulator('8', "AnimationPath", apm);
	
	viewer.setCameraManipulator(keyswitchManipulator.get());
	viewer.setUpViewInWindow(100, 100, OSG_WIDTH, OSG_HEIGHT);
	viewer.run();

	return 0;
}
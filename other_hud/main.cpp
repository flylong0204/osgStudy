#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Depth>
#include <osg/Camera>
#include <osgDB/ReadFile> 
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgText/Text>
#include <osgGA/GUIEventHandler>
#include <osg/io_utils>
#include "../Util/Axis.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

/*!
   HUD(Head Up Display)，新添一个摄像机来拍摄文字，且正交投影到屏幕的指定位置
   不接收事件，最后渲染，一直显示
*/
osg::Node* createHUDText(osgText::Text* text)
{
	// for scene node
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(text);
	// set stateset
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON); // make sure alpha texture display ok

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, WIN_WIDTH, 0, WIN_HEIGHT));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearColor(osg::Vec4(0.5f,0.5f, 0.5f, 1.0f));
	camera->setClearMask(GL_DEPTH_BUFFER_BIT); 
	camera->setAllowEventFocus(false);
	camera->setRenderOrder(osg::Camera::POST_RENDER);

	camera->addChild(geode);

	return camera.release();
}

/*!
   添加一个获取动态的文字信息
*/
class DynHudText : public osgGA::GUIEventHandler
{
public:
	DynHudText(osgText::Text *text) : mText(text)
	{}
	~DynHudText()
	{}

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
	{
		if (ea.getEventType() == osgGA::GUIEventAdapter::FRAME)
		{
			osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
			if (viewer)
			{
				viewer->getCamera()->getViewMatrixAsLookAt(mEye, mCenter, mUp);
				updateText(viewer, ea);
			}
		}
		return false;
	}

	void updateText(osgViewer::Viewer *viewer, const osgGA::GUIEventAdapter &ea)
	{
		std::stringstream ss;
		ss << "Eye(" << mEye << ")";
		if (mText.get())
		{
			mText->setText(ss.str());
		}
	}
private:
	osg::Vec3 mEye; //! 视点坐标
	osg::Vec3 mCenter;
	osg::Vec3 mUp;
	osg::ref_ptr<osgText::Text> mText;
};

class AxisCamera : public osg::Camera
{
public:
	AxisCamera()
	{
		addChild(Axis::createAxis(1.0f, 1.0f, 1.0f));
	}

	AxisCamera(AxisCamera const& copy, osg::CopyOp copyOp = osg::CopyOp::SHALLOW_COPY)
		: Camera(copy, copyOp)
		, mCamera(copy.mCamera)
	{}
	
	META_Node(osg, AxisCamera);

	void setMainCamera(osg::Camera *camera)
	{
		mCamera = camera;
	}

	virtual void traverse(osg::NodeVisitor &nv)
	{
		double fovy;
		double aspectRatio;
		double near;
		double far;
		mCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, near, far);
		// 固定投影矩阵的，避免缩放影响
		this->setProjectionMatrixAsOrtho(-10.0f * aspectRatio, 10.0f * aspectRatio, -10.0f, 10.0f, 2.0f, -2.0f);
		osg::Vec3 trans(8.5f * aspectRatio, -8.5f, -8.5f);
		// 保持axis与主相机的视图矩阵一直
		if (mCamera.valid() && nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
		{
			osg::Matrix matrix = mCamera->getViewMatrix();
			matrix.setTrans(trans);
			this->setViewMatrix(matrix);
		}
		osg::Camera::traverse(nv);
	}

protected:
	virtual ~AxisCamera() {}
	osg::observer_ptr<osg::Camera> mCamera;
};

int main()
{
	osgViewer::Viewer viewer;
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../asserts/cow.osg");
	root->addChild(model);
	// font ttf
	std::string ttf = "../asserts/fonts/simfang.ttf";
	if (true)
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(ttf);
		// position
		// y^
		//  |
		//  |____> x
		//  0
		osg::Vec3 position(5.0f, 10.0f, 0.0f);
		text->setPosition(position);
		text->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		text->setText(L"English 中文 ");

		root->addChild(createHUDText(text.get()));
	}
	if (true)
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setFont(ttf);
		text->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);
		osg::Vec3 position(5.0f, 40.0f, 0.0f);
		text->setPosition(position);
		text->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));		
		osg::ref_ptr<DynHudText> dynText = new DynHudText(text.get());
		root->addChild(createHUDText(text.get()));
		viewer.addEventHandler(dynText.get());
	}
	if (true)
	{
		osg::Camera *mainCamera = viewer.getCamera();
		osg::ref_ptr<AxisCamera> axisCamera = new AxisCamera;
		axisCamera->setMainCamera(mainCamera);
		axisCamera->setRenderOrder(osg::Camera::POST_RENDER);
		axisCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		axisCamera->setAllowEventFocus(false);
		axisCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		root->addChild(axisCamera.get());
	}
	viewer.addEventHandler(new osgViewer::StatsHandler);

	viewer.setUpViewInWindow(100, 100, WIN_WIDTH, WIN_HEIGHT);
	viewer.setSceneData(root.get());
	viewer.realize();
	viewer.run();

	return 0;
}

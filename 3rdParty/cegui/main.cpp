
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>
#include <CEGUI/ScriptModules/Lua/ScriptModule.h>

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/CoordinateSystemNode>
#include <osgGA/GUIEventAdapter>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include "../../Util/Axis.h"
#include "../../Util/Shape.h"
#include <iostream>

unsigned int g_Count = 0;

struct UserItem : public osg::Referenced
{
	UserItem(int i):count(i)
	{}

	int count;
};

class CEGUIDrawable : public osg::Drawable
{
public:

    CEGUIDrawable();

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    CEGUIDrawable(const CEGUIDrawable& drawable,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):
        Drawable(drawable,copyop) {}
    
    META_Object(osg,CEGUIDrawable);
    
	void loadDefault();
    void loadScheme();
    void loadFont();
    void loadLayout();

	void bindViewer(osgViewer::Viewer *viewer);

    void drawImplementation(osg::RenderInfo& renderInfo) const;

protected:   

    virtual ~CEGUIDrawable();

	bool clickButton(const CEGUI::EventArgs &e);
	bool changedScrollbarPosition(const CEGUI::EventArgs &e);

	mutable bool mInitial;
	mutable unsigned int mContextID;
	mutable double mLastTime;
	osgViewer::Viewer *localViewer;
};


struct CEGUIEventCallback : public osgGA::GUIEventHandler
{
    CEGUIEventCallback() {}
    
    /** do customized Event code. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv)
    {
        osgGA::EventVisitor* ev = dynamic_cast<osgGA::EventVisitor*>(nv);
        CEGUIDrawable* cd = dynamic_cast<CEGUIDrawable*>(obj);
        
        if (!ev || !cd) return false;

		int x = ea.getX();
		int y = ea.getY();
		int width = ea.getWindowWidth();
		int height = ea.getWindowHeight();
		if (ea.getMouseYOrientation() == osgGA::GUIEventAdapter::Y_INCREASING_UPWARDS)
		{
			y = height - y;
		}

		CEGUI::GUIContext &gc = CEGUI::System::getSingleton().getDefaultGUIContext();
		bool bAddCube = false;
        
        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::DRAG):
            case(osgGA::GUIEventAdapter::MOVE):
                gc.injectMousePosition(x, y);
				break;
            case(osgGA::GUIEventAdapter::PUSH):
            {
                gc.injectMousePosition(x, y);

                if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)  // left
                  gc.injectMouseButtonDown(CEGUI::LeftButton);

                else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)  // middle
                  gc.injectMouseButtonDown(CEGUI::MiddleButton);

                else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)  // right
                  gc.injectMouseButtonDown(CEGUI::RightButton);
      
				break;
            }
            case(osgGA::GUIEventAdapter::RELEASE):
            {
                gc.injectMousePosition(x, y);

                if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)  // left
                  gc.injectMouseButtonUp(CEGUI::LeftButton);

                else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)  // middle
                  gc.injectMouseButtonUp(CEGUI::MiddleButton);

                else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)  // right
                  gc.injectMouseButtonUp(CEGUI::RightButton);
      
				break;
            }
            case(osgGA::GUIEventAdapter::DOUBLECLICK):
            {
                // do we need to do something special here to handle double click???  Will just assume button down for now.
                gc.injectMousePosition(x, y);

                if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)  // left
                  gc.injectMouseButtonDown(CEGUI::LeftButton);

                else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)  // middle
                  gc.injectMouseButtonDown(CEGUI::MiddleButton);

                else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)  // right
                  gc.injectMouseButtonDown(CEGUI::RightButton);

				break;
            }
            case(osgGA::GUIEventAdapter::KEYDOWN):
                
				if (ea.getKey() == 'a')
				{
					bAddCube = true;
					break;
				}
				gc.injectKeyDown( static_cast<CEGUI::Key::Scan>(ea.getKey()) );
                gc.injectChar( static_cast<CEGUI::Key::Scan>( ea.getKey() ) );
				
				break;
            case(osgGA::GUIEventAdapter::KEYUP):
                gc.injectKeyUp( static_cast<CEGUI::Key::Scan>(ea.getKey()) );
				break;
			case (osgGA::GUIEventAdapter::USER):
			{
				const UserItem *uItem = dynamic_cast<const UserItem*>(ea.getUserData());
				if (uItem)
				{
					bAddCube = true;
					std::cout << "user count is " << uItem->count << std::endl;
				}
				
			}
				break;
            default:
                break;
        }
		if (bAddCube)
		{
			g_Count++;
			/*osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
			if (viewer)
			{
				osg::Group *root = dynamic_cast<osg::Group*>(viewer->getSceneData());
				if (root)
				{
					std::cout << "here add " << std::endl;
					std::srand(time(NULL));
					float x = 0.5 + std::rand() % 100 / 100.0f;
					float y = 0.5 + std::rand() % 100 / 100.0f;
					float z = 0.5 + std::rand() % 100 / 100.0f;
					root->addChild(Util::Shape::createBox(osg::Vec3(x, y, z)));
				}
			}*/
			std::cout << "now count is " << g_Count << std::endl;
			return true;
		}
		// 拦截CEGUI的事件
		CEGUI::Window *root = gc.getRootWindow();
		if (root && root->getChildAtPosition(CEGUI::Vector2f(x, y)))
		{	
			return true;
		}
        return false;
    }
};

CEGUIDrawable::CEGUIDrawable()
	: mContextID(0), mInitial(false), mLastTime(0.0)
{
	setSupportsDisplayList(false);
	setDataVariance(osg::Object::DYNAMIC);
	osg::StateSet *ss = getOrCreateStateSet();
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

    setEventCallback(new CEGUIEventCallback());

	CEGUI::OpenGLRenderer &myRender = CEGUI::OpenGLRenderer::bootstrapSystem();
}

CEGUIDrawable::~CEGUIDrawable()
{}

static int localCount = 0;
bool CEGUIDrawable::clickButton(const CEGUI::EventArgs & e)
{
	std::cout << "click button" << std::endl;
	if (localViewer)
	{
		localViewer->getEventQueue()->userEvent(new UserItem(localCount++));
	}
	return false;
}

bool CEGUIDrawable::changedScrollbarPosition(const CEGUI::EventArgs & e)
{	
	std::cout << "scrollbar change position" << std::endl;
	if (localViewer)
	{
		localViewer->getEventQueue()->userEvent(new UserItem(localCount++));
	}
	return false;
}

void CEGUIDrawable::loadDefault()
{
	CEGUI::DefaultResourceProvider *rp =
		static_cast<CEGUI::DefaultResourceProvider *>(CEGUI::System::getSingleton().getResourceProvider());
	rp->setResourceGroupDirectory("Schemes", "../datafiles/schemes/");
	rp->setResourceGroupDirectory("Imagesets", "../datafiles/imagesets/");
	rp->setResourceGroupDirectory("Fonts", "../datafiles/fonts/");
	rp->setResourceGroupDirectory("Layouts", "../datafiles/layouts/");
	rp->setResourceGroupDirectory("LookNFeels", "../datafiles/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", "../datafiles/lua_scripts/");

	CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeels");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
}

void CEGUIDrawable::loadScheme()
{
	try
	{
		CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
		//CEGUI::ImageManager::getSingleton().get("TaharezLook.png");
	}
	catch (CEGUI::Exception e)
	{
		std::cout << "CEGUIDrawable::loadScheme Error: " << e.getMessage() << std::endl;
	}
}

void CEGUIDrawable::loadFont()
{
    try
    {
		CEGUI::FontManager::getSingleton().createFreeTypeFont("DefaultFont", 10, true, "simhei.ttf");
		CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DefaultFont");
    }
    catch (CEGUI::Exception e)
    {
        std::cout<<"CEGUIDrawable::loadFont Error: "<<e.getMessage()<<std::endl;
    }
}

CEGUI::String ws2CEString(const std::wstring& s)
{
	size_t max_bytes = s.size() * 4 + 1;
	CHAR* buf = new CHAR[max_bytes];
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, buf, max_bytes, 0, 0);
	CEGUI::String ret((CEGUI::utf8*)buf);
	delete buf;
	return ret;
}

void CEGUIDrawable::loadLayout()
{
    try
    {
		if (true)
		{
			CEGUI::Window* myRoot = CEGUI::WindowManager::getSingleton().loadLayoutFromFile(
				//"Demo8.layout"
				"ParametricModelScene.layout"
			);
			CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(myRoot);

			CEGUI::Window *btn = myRoot->getChild("Frame/Reset");
			btn->subscribeEvent(CEGUI::Window::EventMouseClick,
				CEGUI::Event::Subscriber(&CEGUIDrawable::clickButton, this));

			for (int i = 1; i < 17; i++)
			{
				std::ostringstream convert;
				convert << i;
				std::string winName = "Frame/Slider" + convert.str();
				CEGUI::Scrollbar *slider = static_cast<CEGUI::Scrollbar*>(myRoot->getChild(winName));
				if (slider)
				{
					//HPM::ValueRange &vr = (*pVRConfig)[i - 1];
					//slider->setDocumentSize(vr.max - vr.min);
					//slider->setPageSize(vr.step);
					//slider->setStepSize(vr.step);
					//slider->setScrollPosition(vr.def - vr.min);
					slider->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged,
						CEGUI::Event::Subscriber(&CEGUIDrawable::changedScrollbarPosition, this));
				}
			}

			CEGUI::Window *label = myRoot->getChild("Frame/Description");
			label->setText(
				//"description is empty"
				ws2CEString(L"description here is, 可能是中文！")
			);
		}
		/*! 脚本处理事件 */
		else
		{
			CEGUI::ScriptModule &script(CEGUI::LuaScriptModule::create());
			CEGUI::System::getSingleton().setScriptingModule(&script);
			CEGUI::System::getSingleton().executeScriptFile("demo8.lua");
		}
    }
    catch (CEGUI::Exception e)
    {
        std::cout<<"CEGUIDrawable::loadLayout error: "<<e.getMessage()<<std::endl;
    }

}

void CEGUIDrawable::bindViewer(osgViewer::Viewer * viewer)
{
	localViewer = viewer;
}

void CEGUIDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
	if (!mInitial) 
	{		
		mContextID = renderInfo.getContextID();
		mInitial = true;
	}
	else if (mContextID == renderInfo.getContextID())
	{
		osg::State *ss = renderInfo.getState();
		
		//ss->disableAllVertexArrays();

		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		double now = (ss->getFrameStamp() ? ss->getFrameStamp()->getSimulationTime() : 0.0);
		CEGUI::System::getSingleton().injectTimePulse((now -mLastTime) / 1000.0);
		CEGUI::System::getSingleton().renderAllGUIContexts();
		mLastTime = now;

		glPopAttrib();
		glPopMatrix();

		ss->checkGLErrors("CEGUIDrawable::drawImplementation");
	}
}

int main( int argc, char **argv )
{
    osgViewer::Viewer viewer;

	viewer.setUpViewInWindow(100, 100, 1024, 768);

	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	/*! 一定要在CEGUI部分之前先执行 */
	viewer.realize();

	osg::Timer_t start_tick = osg::Timer::instance()->tick();

	osg::ref_ptr<osg::Node> modelGeode = osgDB::readNodeFile("../models/cow.osg");

	// if no model has been successfully loaded report failure.
	if (!modelGeode.valid())
	{
		std::cout << " No model data loaded" << std::endl;
		return 1;
	}

	osg::Timer_t end_tick = osg::Timer::instance()->tick();

	std::cout << "Time to load = " << osg::Timer::instance()->delta_s(start_tick, end_tick) << std::endl;

	/*! CEGUI部分的逻辑都需要在makecurrent 与 releaseContext之间，否则会容易报错 */
	viewer.getCamera()->getGraphicsContext()->makeCurrent();

    osg::ref_ptr<osg::Geode> uiGeode = new osg::Geode;
    osg::ref_ptr<CEGUIDrawable> cd = new CEGUIDrawable();
    uiGeode->addDrawable(cd.get());
	cd->getOrCreateStateSet()->setDataVariance(osg::Object::DYNAMIC);
	cd->loadDefault();
    cd->loadScheme();
    cd->loadFont();
    cd->loadLayout();
	cd->bindViewer(&viewer);
	viewer.getCamera()->getGraphicsContext()->releaseContext();

    osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::StateSet* ssRoot = root->getOrCreateStateSet();
	ssRoot->setDataVariance(osg::Object::DYNAMIC);
    root->addChild(modelGeode.get()); 
    root->addChild(uiGeode.get());
	root->addChild(Util::Axis::createAxis(1.0f, 1.0f, 5.0f));
    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(modelGeode.get());

    // pass the loaded scene graph to the viewer.
    viewer.setSceneData(root.get());

	/*!
	  注意对比，单线程中要避免一个问题，就是动态添加node的问题 
	  不加cegui时，可以随意添加，通过键盘事件来添加
	  但是带了cegui时，就不行了。可能是在Drawable的drawImplementation中调用
	  CEGUI的绘制时产生了问题。这个过程中线程不安全的原因吧！具体的还不知道如何处理的。
	  不了解时，使用逻辑清晰的来处理吧！
	*/
    // run the viewer
	if (false)
	{
		viewer.run();
	}
	else
	{
		viewer.setCameraManipulator(new osgGA::TrackballManipulator);
		while (!viewer.done())
		{
			if (g_Count > 0)
			{
				osg::Group *root = dynamic_cast<osg::Group*>(viewer.getSceneData());
				if (root)
				{
					std::cout << "here add " << std::endl;
					std::srand(time(NULL));
					float x = 0.5 + std::rand() % 100 / 100.0f;
					float y = 0.5 + std::rand() % 100 / 100.0f;
					float z = 0.5 + std::rand() % 100 / 100.0f;
					root->addChild(Util::Shape::createBox(osg::Vec3(x, y, z)));
				}
				g_Count--;
			}
			viewer.frame();
		}
	}

	return 0;
}
/*!
  \brief 拾取事件处理
  \class 拾取类对象

  \author meijie.li
  \date 2018.2.12
  \version 1.0
*/
#ifndef PICK_EVENT_HANDLER_H
#define PICK_EVENT_HANDLER_H 1

#include <osg/AnimationPath>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/Quat>
#include <osg/Array>

class PickEventHandle :public osgGA::GUIEventHandler
{
	enum TypeIntersect
	{
		/*! 多面体 */
		Polytope,
		/*! 线段 */
		LineSegment,
		/*! 面 */
		Plane,
		/*! Ray射线 */
		Ray
	};
public:
	PickEventHandle();

	virtual ~PickEventHandle()
	{}
	
	osg::AnimationPath* createPath();
	
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	
	void pick(const osgGA::GUIEventAdapter &ea, osgViewer::Viewer *viewer);
	
	void toggleScribe(osg::Group *parent, osg::Node* node);
	
	/*！ 
	  底视图查看，按下空格键还原，
	*/
	void viewBottom(osgViewer::Viewer *viewer);

private:
	osg::ref_ptr<osg::Vec3Array> listPoint;
	//osg::ref_ptr<osg::QuatArray> listOrient;
	float xmouse;
	float ymouse;
	TypeIntersect eSelTI;  ///< 多面体
	bool useWCoordinate; ///< 视口窗口坐标系
};

#endif //!PICK_EVENT_HANDLER_H
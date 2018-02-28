/// \file TravelManipulator.h
///
/// 漫游 osg的实现
///
/// \author meijie.li
/// \version 0.1
/// \date 2018.1.30

#ifndef TRAVEL_MANIPULATOR_H

#include <osgViewer/Viewer>
#include <osg/LineSegment>
#include <osg/Point>
#include <osg/Geometry>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osgGA/CameraManipulator>
#include <osgUtil/IntersectVisitor>

#include <vector>

/// 漫游器继承自CameraManipluator抽象类
/// 达到可以控制OSG相机对应GUI事件，主要为键盘与鼠标
class TravelManipulator :public osgGA::CameraManipulator
{
public:

	TravelManipulator();
	~TravelManipulator();

	/// \brief 关联漫游器与场景浏览器
	static TravelManipulator * TravelToScence(osg::ref_ptr<osgViewer::Viewer>viewer);
public:
	bool m_bLeftButtonDown; ///< 鼠标左键状态
	float m_fpushX; ///< 鼠标位置
	float m_fpushY;
	float m_fAngle; ///< 屏幕角度
	bool m_bPeng; ///< 碰撞检测状态

	/// \brief 实现抽象类的接口
	virtual void setByMatrix(const osg::Matrix &matrix);
	virtual void setByInverseMatrix(const osg::Matrix &matrix);
	virtual osg::Matrixd getMatrix() const;
	virtual osg::Matrixd getInverseMatrix() const;
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
		
	/// \brief 位置变换
	void ChangePosition(osg::Vec3 delta);
	/// \brief 拾取
	void PickObject(osgViewer::View *pView, const osgGA::GUIEventAdapter &ea);
	

	/// \brief 创建一条线段
	osg::ref_ptr<osg::Node> createShape(osg::PrimitiveSet::Mode mode, osg::Vec3Array &v);

	/// \brief 设置速度
	float getSpeed();
	void setSpeed(float &);

	/// \brief 位置
	void SetPosition(osg::Vec3 &position);
	osg::Vec3 GetPosition();

private:
	osg::ref_ptr<osgViewer::Viewer> m_pHostViewer;
	float m_fMoveSpeed; ///< 移动速度
	osg::Vec3 m_vPosition; ///< 当前位置
	osg::Vec3 m_vRotation; ///< 旋转角度
};

#endif // !TRAVEL_MANIPULATOR_H

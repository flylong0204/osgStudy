/// \file TravelManipulator.h
///
/// ���� osg��ʵ��
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

/// �������̳���CameraManipluator������
/// �ﵽ���Կ���OSG�����ӦGUI�¼�����ҪΪ���������
class TravelManipulator :public osgGA::CameraManipulator
{
public:

	TravelManipulator();
	~TravelManipulator();

	/// \brief �����������볡�������
	static TravelManipulator * TravelToScence(osg::ref_ptr<osgViewer::Viewer>viewer);
public:
	bool m_bLeftButtonDown; ///< ������״̬
	float m_fpushX; ///< ���λ��
	float m_fpushY;
	float m_fAngle; ///< ��Ļ�Ƕ�
	bool m_bPeng; ///< ��ײ���״̬

	/// \brief ʵ�ֳ�����Ľӿ�
	virtual void setByMatrix(const osg::Matrix &matrix);
	virtual void setByInverseMatrix(const osg::Matrix &matrix);
	virtual osg::Matrixd getMatrix() const;
	virtual osg::Matrixd getInverseMatrix() const;
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);
		
	/// \brief λ�ñ任
	void ChangePosition(osg::Vec3 delta);
	/// \brief ʰȡ
	void PickObject(osgViewer::View *pView, const osgGA::GUIEventAdapter &ea);
	

	/// \brief ����һ���߶�
	osg::ref_ptr<osg::Node> createShape(osg::PrimitiveSet::Mode mode, osg::Vec3Array &v);

	/// \brief �����ٶ�
	float getSpeed();
	void setSpeed(float &);

	/// \brief λ��
	void SetPosition(osg::Vec3 &position);
	osg::Vec3 GetPosition();

private:
	osg::ref_ptr<osgViewer::Viewer> m_pHostViewer;
	float m_fMoveSpeed; ///< �ƶ��ٶ�
	osg::Vec3 m_vPosition; ///< ��ǰλ��
	osg::Vec3 m_vRotation; ///< ��ת�Ƕ�
};

#endif // !TRAVEL_MANIPULATOR_H

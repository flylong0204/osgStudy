/*!
  \brief ʰȡ�¼�����
  \class ʰȡ�����

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
		/*! ������ */
		Polytope,
		/*! �߶� */
		LineSegment,
		/*! �� */
		Plane,
		/*! Ray���� */
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
	
	/*�� 
	  ����ͼ�鿴�����¿ո����ԭ��
	*/
	void viewBottom(osgViewer::Viewer *viewer);

private:
	osg::ref_ptr<osg::Vec3Array> listPoint;
	//osg::ref_ptr<osg::QuatArray> listOrient;
	float xmouse;
	float ymouse;
	TypeIntersect eSelTI;  ///< ������
	bool useWCoordinate; ///< �ӿڴ�������ϵ
};

#endif //!PICK_EVENT_HANDLER_H
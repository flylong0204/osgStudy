#include "FollowNodeManip.h"

followNodeMatrixManipulator::followNodeMatrixManipulator( transformAccumulator* ta)
{
   worldCoordinatesOfNode = ta;
   theMatrix = osg::Matrixd::identity();
}

//��������ڴ�һ��������л�����һ�������ʱ���ã���������һ�������
//����ͼ���󴫹����������Ϳ������趨�Լ��ĳ�ʼλ���ˡ�
void followNodeMatrixManipulator::setByMatrix(const osg::Matrixd& mat)
{
   theMatrix = mat;
}

//������������ⲿֱ�ӵ���Viewer��setViewByMatrix����ʱ�������õľ���
//�����������������ס�¸��ĵ�λ�á�
void followNodeMatrixManipulator::setByInverseMatrix(const osg::Matrixd& mat)
{
   theMatrix = mat.inverse(mat);
}

//SetByMatrix������Ҫ�ľ����������������õ��ģ���������һ����������ݾ���
osg::Matrixd followNodeMatrixManipulator::getMatrix() const
{
	return theMatrix;
}


//���������Ҫ�ķ������������ÿ֡�ᱻ���ã������ص�ǰ����ͼ����
/*
�ȵõ���ת���󣬿����þ������ makeRotate ��������������ת�ǶȺ���ת�ᣬȻ���þ�����
��translate�����õ�ƽ�ƾ�������ת�������ƽ�ƾ���(��ƽ�ƾ������ұ�)���͵õ��������
��ת�任���󣬵�����ͼ�����Ǳ任����ķ������������Ҫ�þ������ inverse�����ĵ�������
*/
osg::Matrixd followNodeMatrixManipulator::getInverseMatrix() const
{
   osg::Matrixd m;
   m = theMatrix * osg::Matrixd::rotate(-osg::PI/2.0, osg::Vec3(1,0,0) );
   return m;
}

void followNodeMatrixManipulator::updateTheMatrix()
{
   theMatrix = worldCoordinatesOfNode->getMatrix();
}

bool followNodeMatrixManipulator::handle(const osgGA::GUIEventAdapter&ea, osgGA::GUIActionAdapter&aa)
{
   switch(ea.getEventType())
   {
   case (osgGA::GUIEventAdapter::FRAME):
      {
         updateTheMatrix();
         return false;
      }
   }
   return false;
}
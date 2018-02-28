#include "FollowNodeManip.h"

followNodeMatrixManipulator::followNodeMatrixManipulator( transformAccumulator* ta)
{
   worldCoordinatesOfNode = ta;
   theMatrix = osg::Matrixd::identity();
}

//这个函数在从一个摄像机切换到另一个摄像机时调用，用来把上一个摄像机
//的视图矩阵传过来，这样就可依此设定自己的初始位置了。
void followNodeMatrixManipulator::setByMatrix(const osg::Matrixd& mat)
{
   theMatrix = mat;
}

//这个方法当在外部直接调用Viewer的setViewByMatrix方法时，把设置的矩阵
//传过来，让摄像机记住新更改的位置。
void followNodeMatrixManipulator::setByInverseMatrix(const osg::Matrixd& mat)
{
   theMatrix = mat.inverse(mat);
}

//SetByMatrix方法需要的矩阵就是用这个方法得到的，用来向下一个摄像机传递矩阵。
osg::Matrixd followNodeMatrixManipulator::getMatrix() const
{
	return theMatrix;
}


//这个是最重要的方法，这个方法每帧会被调用，它返回当前的视图矩阵。
/*
先得到旋转矩阵，可以用矩阵类的 makeRotate 方法，参数是旋转角度和旋转轴，然后用矩阵类
的translate方法得到平移矩阵，用旋转矩阵左乘平移矩阵(即平移矩阵在右边)，就得到摄像机的
旋转变换矩阵，但是视图矩阵是变换矩阵的反矩阵，所以最后要用矩阵类的 inverse方法的到反矩阵。
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
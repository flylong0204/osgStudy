/*!
   漫游器的定制化实现

   \author meijie.li
   \date 2018.2.12
   \version 1.0
*/
#ifndef CUSTOM_MANIPULATOR_H
#define CUSTOM_MANIPULATOR_H 1

#include <osg/ComputeBoundsVisitor>
#include <osgGA/CameraManipulator>
#include <osgViewer/Viewer>

class WalkForward : public osgGA::CameraManipulator
{
public:
	WalkForward(const osg::BoundingBox &bb);

	/*!
	  实现接口
	*/
	virtual void setByMatrix(const osg::Matrixd &matrix);
	virtual osg::Matrixd getMatrix() const;
	virtual void setByInverseMatrix(const osg::Matrixd &matrix);
	virtual osg::Matrixd getInverseMatrix() const;

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

	bool move2NextPosition(osg::Vec3 delta, osgViewer::Viewer *viewer);
	bool rotateByZAxis(float delta);
	bool rotateByXAxis(float delta);
	bool rotateByYAxis(float delta);

private:
	/*! 视点当前位置 */
	osg::Vec3 vPosition;
	/*! 视点朝向 */
	osg::Vec3 vRotation;
	/*! 行进步长 */
	float vStep;
	/*! 旋转角度 */
	float vRotateStep;

	~WalkForward();
};

#endif //!CUSTOM_MANIPULATOR_H
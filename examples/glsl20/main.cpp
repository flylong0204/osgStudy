
#include <osg/Node>
#include <osg/Program>
#include <osg/Shader>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "../../Util/Axis.h"

/*!
  varying与attribute在3.x以后废弃了的，等价于 in/out

  这里使用顶点坐标值来设置颜色值
  在片段处理中，对color进行clamp，限制其取值范围
  
  添加一个原点单位坐标轴做对比，发现得到的结果是红绿两种颜色，
  因为高度很小，RGB对应着XYZ，还有可以得出clamp函数的作用，对负数丢弃了
*/
static char * vertexShader = {
	"varying vec4 color;\n"
	"void main(void ){\n"
	"color = gl_Vertex;\n"
	"gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;\n"
	"}\n"
};
static char * fragShader = {
	"varying vec4 color;\n"
	"void main(void){\n"
	"	gl_FragColor = clamp(color,0.0,1.0);\n"
	"}\n"
};

int main( int argc, char** argv )
{
	osgViewer::Viewer viewer;

	osg::ref_ptr<osg::Group> root = new osg::Group;

	osg::ref_ptr<osg::Node>node = osgDB::readNodeFile("../models/glider.osg");
	
	osg::StateSet * ss = node->getOrCreateStateSet();
	osg::Program * program = new osg::Program;
	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragShader));
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShader));
	ss->setAttributeAndModes(program, osg::StateAttribute::ON);

	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	root->addChild(node.get());
	root->addChild(Util::Axis::createAxis(1.0f, 1.0f, 1.0f));
	viewer.setSceneData(root.get());

	viewer.run();

	return 0;
}

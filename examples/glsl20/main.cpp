
#include <osg/Node>
#include <osg/Program>
#include <osg/Shader>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "../../Util/Axis.h"

/*!
  varying��attribute��3.x�Ժ�����˵ģ��ȼ��� in/out

  ����ʹ�ö�������ֵ��������ɫֵ
  ��Ƭ�δ����У���color����clamp��������ȡֵ��Χ
  
  ���һ��ԭ�㵥λ���������Աȣ����ֵõ��Ľ���Ǻ���������ɫ��
  ��Ϊ�߶Ⱥ�С��RGB��Ӧ��XYZ�����п��Եó�clamp���������ã��Ը���������
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

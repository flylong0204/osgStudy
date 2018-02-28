
#include <osg/shader>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/fileutils>
#include <osg/Geometry>
#include <osgViewer/viewer>
#include <osgViewer/ViewerEventHandlers>

#include "../../Util/Axis.h"

static char * vertexShader = {
	"#version 430 \n"
	"layout (location=0) in vec3 VertexPosition;\n"
	"layout (location=1) in vec4 VertexColor;\n"
	"uniform mat4 MVP;"
	"out vec4 Color;\n"
	"void main()\n"
	"{\n"
	"	Color = VertexColor;\n"
	"	gl_Position = MVP * vec4(VertexPosition,1.0);\n"
	"}\n"
};

static char * fragShader = {
	"#version 430 \n"
	"in vec4 Color;\n"
	"layout (location=0) out vec4 FragColor;\n"
	"void main() {\n"
	"	FragColor = Color;//vec4(0.5,0.5,0.5,0.4);\n"
	//"	FragColor = vec4(1.0,0.5,0.5,0.4);\n"
	"}\n"
};

osg::Node *  CreateNode()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> polyGeom = new osg::Geometry();
	
	polyGeom->setUseDisplayList(false);

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
	vertices->push_back(osg::Vec3(-1, 0, 0));
	vertices->push_back(osg::Vec3(1, 0, 0));
	vertices->push_back(osg::Vec3(0, 0, 1));
	polyGeom->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> colorsArray = new osg::Vec4Array;
	colorsArray->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colorsArray->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	colorsArray->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	polyGeom->setColorArray(colorsArray.get(), osg::Array::BIND_PER_VERTEX);

	osg::DrawElementsUShort *triangles = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES, 0);
	triangles->push_back(0);
	triangles->push_back(1);
	triangles->push_back(2);
	polyGeom->addPrimitiveSet(triangles);
	//polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));

	/*
	The osg::Geometry class uses the setVertexAttribArray() and
	setVertexAttribBinding() methods to bind vertex attributes to shaders. They should
	be provided per vertex. GLSL's built-in vertex attributes include the gl_Position, gl_
	Normal, and gl_MultiTexCoord* variables. However, you may still specify your own
	vertex attributes, such as tangents or vertex weights.
	Try declaring an attribute in the vertex shader and make use of the osg::Geometry's vertex
	attribute arrays. Another important task that you need to perform is to bind the external
	attribute array and the GLSL attribute, with the help of the addBindAttribLocation()
	method of osg::Program. It has a name and an index parameter, the first of which
	indicates the attribute name in the shader source code, and the second should correspond
	to the input index value of setVertexAttribArray().
	*/
	polyGeom->setVertexAttribArray(0, vertices.get(), osg::Array::BIND_PER_VERTEX);
	polyGeom->setVertexAttribArray(1, colorsArray.get(), osg::Array::BIND_PER_VERTEX);

	geode->addDrawable(polyGeom);
	return geode.release();
}

class MVPCallback : public osg::Uniform::Callback
{
public:
	MVPCallback(osg::Camera * camera) :mCamera(camera) 
	{}
	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) 
	{
		osg::Matrix mv = mCamera->getViewMatrix();
		osg::Matrix p = mCamera->getProjectionMatrix();
		uniform->set(mv * p);
	}

private:
	osg::ref_ptr<osg::Camera> mCamera;
};

int main(int argc, char *argv[]) {

	osgViewer::Viewer viewer;
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::StatsHandler);

	osg::ref_ptr<osg::Group> root = new osg::Group;

	osg::ref_ptr<osg::Node> node = CreateNode();
	osg::StateSet * ss = node->getOrCreateStateSet();

	osg::ref_ptr<osg::Program> program = new osg::Program;
	ss->setAttributeAndModes(program, osg::StateAttribute::ON);

	osg::Shader * vS = new osg::Shader(osg::Shader::FRAGMENT, fragShader);
	osg::Shader * fS = new osg::Shader(osg::Shader::VERTEX, vertexShader);
	program->addShader(vS);
	program->addShader(fS);

	osg::Uniform* MVPUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "MVP");
	MVPUniform->setUpdateCallback(new MVPCallback(viewer.getCamera()));
	ss->addUniform(MVPUniform);

	root->addChild(node.get());
	root->addChild(Util::Axis::createAxis(1.0f, 1.0f, 1.0f));
	viewer.setSceneData(root.get());

	viewer.run();

	return 0;
}

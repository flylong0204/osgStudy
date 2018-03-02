#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Shape>
#include <osg/Shapedrawable>
#include <osg/MatrixTransform>

#include "../../Util/Shader.h"

osg::Node *  CreateNode()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> polyGeom = new osg::Geometry();
	
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
	vertices->push_back(osg::Vec3(-10, 0, 0));
	vertices->push_back(osg::Vec3(10, 0, 0));
	vertices->push_back(osg::Vec3(0, 10, 10));
	polyGeom->setVertexArray(vertices.get());

	osg::ref_ptr<osg::Vec4Array> colorsArray = new osg::Vec4Array;
	colorsArray->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	colorsArray->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	colorsArray->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	polyGeom->setColorArray(colorsArray.get(), osg::Array::BIND_PER_VERTEX);

	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
	normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	polyGeom->setNormalArray(normals.get(), osg::Array::BIND_PER_VERTEX);

	polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));

	polyGeom->setVertexAttribArray(0, vertices.get(), osg::Array::BIND_PER_VERTEX);
	polyGeom->setVertexAttribArray(1, colorsArray.get(), osg::Array::BIND_PER_VERTEX);
	polyGeom->setVertexAttribArray(2, normals.get(), osg::Array::BIND_PER_VERTEX);

	geode->addDrawable(polyGeom.get());

	return geode.release();
}


osg::ref_ptr<osg::MatrixTransform> lightPos;

class MyNodeVisitor : public osg::NodeVisitor
{
public:
	MyNodeVisitor() :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN){}

	virtual void apply(osg::Geode& node)
	{
		for (int i = 0; i < node.getNumDrawables(); ++i)
		{
			osg::Geometry * polyGeom = dynamic_cast<osg::Geometry*>(node.getDrawable(i));

			if (!polyGeom)return;

			polyGeom->setVertexAttribArray(0, polyGeom->getVertexArray(), osg::Array::BIND_PER_VERTEX);
			polyGeom->setVertexAttribArray(1, polyGeom->getColorArray(), osg::Array::BIND_PER_VERTEX);
			polyGeom->setVertexAttribArray(2, polyGeom->getNormalArray(), osg::Array::BIND_PER_VERTEX);
		}
	}
};


class LightPosCallback : public osg::Uniform::Callback
{
public:
	LightPosCallback()
	{
	}
	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv)
	{
		osg::Matrix m = lightPos->getMatrix();
		uniform->set(m.getTrans());
	}
};

osg::Node * createlight()
{
	osg::ShapeDrawable *sun_sd = new osg::ShapeDrawable;
	osg::Sphere* sun_sphere = new osg::Sphere;
	sun_sphere->setName("SunSphere");
	sun_sphere->setRadius(0.5);
	sun_sd->setShape(sun_sphere);
	sun_sd->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));

	osg::Geode* sun_geode = new osg::Geode;
	sun_geode->setName("SunGeode");
	sun_geode->addDrawable(sun_sd);

	return sun_geode;
}


class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:

	KeyboardEventHandler(){}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
	{
		switch (ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey() == 'x')//绕x轴旋转
			{
				osg::Matrix trans = lightPos->getMatrix();
				trans = trans * osg::Matrix::rotate(osg::PI_2 / 10, osg::X_AXIS);
				lightPos->setMatrix(trans);
			}
			if (ea.getKey() == 'y')//绕y轴旋转
			{
				osg::Matrix trans = lightPos->getMatrix();
				trans = trans * osg::Matrix::rotate(osg::PI_2 / 10, osg::Y_AXIS);
				lightPos->setMatrix(trans);
			}
			if (ea.getKey() == 'z')//绕z轴旋转
			{
				osg::Matrix trans = lightPos->getMatrix();
				trans = trans * osg::Matrix::rotate(osg::PI_2 / 10, osg::Z_AXIS);
				lightPos->setMatrix(trans);
			}
		}
		}

		return false;
	}
};
int main()
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	osg::ref_ptr<osg::Node> geode =
		//osgDB::readNodeFile("../models/cow.osg");
		CreateNode();
	MyNodeVisitor visitor;
	geode->accept(visitor);


	osg::ref_ptr<osg::StateSet> stateSet = geode->getOrCreateStateSet();

	osg::ref_ptr<osg::Shader> vShader = new osg::Shader(osg::Shader::VERTEX);
	if (!Util::Shader::loadShaderFile(vShader, "../shaders/demo1.vert"))
	{
		return -1;
	}
	osg::ref_ptr<osg::Shader> fShader = new osg::Shader(osg::Shader::FRAGMENT);
	if (!Util::Shader::loadShaderFile(fShader, "../shaders/demo1.frag"))
	{
		return -1;
	}
	osg::ref_ptr<osg::Program> program = new osg::Program;
	program->addShader(vShader.get());
	program->addShader(fShader.get());

	//program->addBindAttribLocation("MCvertex", 0);
	//program->addBindAttribLocation("MCcolors", 1);
	//program->addBindAttribLocation("MCnormal", 2);

	osg::ref_ptr<osg::Uniform> M4 = new osg::Uniform(osg::Uniform::DOUBLE_VEC3, "LightPosition");
	M4->setUpdateCallback(new LightPosCallback());
	stateSet->addUniform(M4.get());

	stateSet->setAttributeAndModes(program.get(), osg::StateAttribute::ON);

	lightPos = new osg::MatrixTransform;
	lightPos->setMatrix(osg::Matrix::translate(0, 0, 5));
	lightPos->addChild(createlight());

	osg::Group * root = new osg::Group;
	root->addChild(lightPos);
	root->addChild(geode);

	viewer->addEventHandler(new KeyboardEventHandler());
	viewer->setSceneData(root);
	viewer->setUpViewInWindow(35, 35, 1024, 800);

	viewer->realize();
	
	osg::State* state = viewer->getCamera()->getGraphicsContext()->getState();
	state->setUseModelViewAndProjectionUniforms(true);


	return viewer->run();
}
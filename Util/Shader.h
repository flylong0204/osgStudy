
/*!
   着色器相关的工具函数
*/
#ifndef MEIJIE_SHADER_H
#define MEIJIE_SHADER_H 1

#include <osg/Shader>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

namespace Util
{
	class Shader
	{
	public:
		static bool loadShaderFile(osg::ref_ptr<osg::Shader> shader, const std::string& fileName)
		{
			std::string fqFileName = osgDB::findDataFile(fileName);
			if (fqFileName.length() == 0)
			{
				std::cout << "File \"" << fileName << "\" not found." << std::endl;
				return false;
			}
			bool success = shader->loadShaderSourceFromFile(fqFileName.c_str());
			if (!success)
			{
				std::cout << "Couldn't load file: " << fileName << std::endl;
				return false;
			}
			else
			{
				return true;
			}
		}

	};

}
#endif // !MEIJIE_SHADER_H
/*!
  get opengl version
  
  \author meijie.li
  \date 2018.2.13
  \version 1.0
*/
#ifndef SUPPORTED_OPENGL_VERSION_H
#define SUPPORTED_OPENGL_VERSION_H 1

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <osg/GraphicsContext>

class SupportedOpenGLVersion : public osg::GraphicsOperation
{
public:
	SupportedOpenGLVersion()
		: osg::Referenced(true)
		, osg::GraphicsOperation("SupportedOpenGLVersion", false)
		, mSupported(true)
		, mMessage()
		, mVersion(0.0f)
	{}

	virtual void operator()(osg::GraphicsContext *gc)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mMutex);
		osg::GLExtensions *gl2ext = gc->getState()->get<osg::GLExtensions>();
		if (gl2ext)
		{
			if (!gl2ext)
			{
				mSupported = false;
				mMessage = "Error: GLSL not supported by OpenGL Driver.";
			}
			else
			{
				mVersion = gl2ext->glVersion;
			}
		}
		else
		{
			mSupported = false;
			mMessage = "Error: GLSL not supported.";
		}
	}
	OpenThreads::Mutex  mMutex;
	bool mSupported;
	std::string mMessage;
	float mVersion;
};

#endif
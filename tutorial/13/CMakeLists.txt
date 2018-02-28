# tutorial
SET(EXAMPLE_NAME 13MultiCamera)
SET(EXAMPLE_FILES 
	TestMultiCamera.cpp
	../Util/findNodeVisitor.h
	../Util/findNodeVisitor.cpp
	../Util/FollowNodeManip.h
	../Util/FollowNodeManip.cpp
	../Util/TransformAccumulator.h
	../Util/TransformAccumulator.cpp
	../Util/Util.h
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()

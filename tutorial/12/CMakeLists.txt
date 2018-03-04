# tutorial
SET(EXAMPLE_NAME 12TwoCamera)
SET(EXAMPLE_FILES 
	WorkingTwoCamera.cpp
	../Util/TransformAccumulator.h
	../Util/TransformAccumulator.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "tutorial")
ENDIF()

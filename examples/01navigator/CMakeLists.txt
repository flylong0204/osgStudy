# 导航测试案例
SET(EXAMPLE_NAME 01Travel)
SET(EXAMPLE_FILES 
	main.cpp
	PickEventHandler.h
	PickEventHandler.cpp
	Util.h
	Util.cpp
	CustomManipulator.h
	CustomManipulator.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "example")
ENDIF()
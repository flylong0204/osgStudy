# 导航测试案例
SET(EXAMPLE_NAME 02Travel)
SET(EXAMPLE_FILES 
	main.cpp
	TravelManipulator.h
	TravelManipulator.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "example")
ENDIF()
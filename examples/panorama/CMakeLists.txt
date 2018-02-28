# 全景图 滑翔导航
SET(EXAMPLE_NAME panorama)
SET(EXAMPLE_FILES 
	main.cpp
	RotateGlider.h
	RotateGlider.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "example")
ENDIF()
# 
SET(EXAMPLE_NAME glsldemo1)
SET(EXAMPLE_FILES 
	main.cpp
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "example")
ENDIF()
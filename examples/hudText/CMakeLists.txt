# other: hud for text
SET(EXAMPLE_NAME HudText)
SET(EXAMPLE_FILES 
	main.cpp
	../../Util/Axis.h
	)
START_EXAMPLE()
IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "example")
ENDIF()
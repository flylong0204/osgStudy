# 
SET(EXAMPLE_NAME microPather)
SET(EXAMPLE_FILES 
	main.cpp
    micropather.h
	micropather.cpp
	)
START_EXAMPLE()

IF(MSVC)
	SET_TARGET_PROPERTIES(${EXAMPLE_NAME} PROPERTIES FOLDER "3rdParty")
ENDIF()
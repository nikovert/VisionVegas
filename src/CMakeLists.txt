PROJECT(PPS_JASS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_COLOR_MAKEFILE ON)

IF( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )
    # Debug
    SET(CMAKE_C_FLAGS "-std=c11 -O3 -g -fno-inline -Wall -Wunused -Wextra")
    SET(CMAKE_CXX_FLAGS "-std=gnu++14 -O3 -g -fno-inline -Wall -Wunused -Wextra")
ELSE( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )
    # Release
    SET(CMAKE_C_FLAGS "-std=c11 -O3 -Wall -Wunused -Wextra")
    SET(CMAKE_CXX_FLAGS "-std=gnu++14 -O3 -Wall -Wunused -Wextra")
ENDIF( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISPLAY")

INCLUDE_DIRECTORIES(.)

LINK_DIRECTORIES()

FILE(GLOB FILE_SRC *.cpp)
FILE(GLOB FILE_HDR *.hpp)

SET(LIBS "")

ADD_EXECUTABLE(recognition ${FILE_SRC} ${FILE_HDR})
TARGET_LINK_LIBRARIES(recognition ${LIBS})

# Required for IDE
source_group("Header Files" FILES ${FILE_HDR})
source_group("Source Files" FILES ${FILE_SRC})

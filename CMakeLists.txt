cmake_minimum_required(VERSION 2.8)


#in windows, cmake . -G "NMake Makefiles"
#in unix, cmake . -G "Unix Makefiles"
project(fuzzylite CXX)

set(CMAKE_VERBOSE_MAKEFILE true)


if(WIN32)
    message("Windows")
    add_definitions(-DFL_WINDOWS)
endif()

if (UNIX)
    message("Unix")
    add_definitions(-DFL_UNIX)
endif()

if (APPLE)
    message("Apple")
    add_definitions(-DFL_APPLE)
endif()


set(FL_VERSION 4.1)
add_definitions(-DFL_VERSION="${FL_VERSION}")

set(FL_DATE "0309")
add_definitions(-DFL_DATE="0309")

option(FL_USE_FLOAT "Use fl::scalar as float" OFF)
option(FL_DEBUG "Enable additional debugging information" OFF)
option(FL_BACKTRACE "Provide backtrace information in case of errors" ON)


if(FL_DEBUG)
    add_definitions(-DFL_DEBUG=true)
    set(CMAKE_BUILD_TYPE Debug)
    set(FL_CONF -dbg)
else()
    set(CMAKE_BUILD_TYPE Release)
    set(FL_CONF)
endif(FL_DEBUG)



add_definitions(-DFL_BUILD_PATH="${CMAKE_SOURCE_DIR}") #used to determine FL__FILE__

#set(EXECUTABLE_OUTPUT_PATH bin)
#set(LIBRARY_OUTPUT_PATH bin)

set(FL_LIBS)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -fPIC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
if(NOT APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined") #apple does not recognise these
endif()

elseif(WIN32)
    #http://msdn.microsoft.com/en-us/library/thxezb7y.aspx
    set(CMAKE_CXX_FLAGS " /WX /W4 /EHsc")
    set(FL_LIBS dbghelp)
endif()


if(FL_USE_FLOAT)
    add_definitions(-DFL_USE_FLOAT)
endif(FL_USE_FLOAT)

if(NOT FL_BACKTRACE)
    add_definitions(-DFL_BACKTRACE_OFF)
endif()

include_directories(.)
include_directories ("${PROJECT_SOURCE_DIR}/LoggerCpp/include/")
add_definitions (-std=c++0x)  # -std=c++11

file(STRINGS FL_HEADERS fl-headers)
file(STRINGS FL_SOURCES fl-sources)

string(REGEX REPLACE "\n" " " ${fl-headers} ${fl-headers})
string(REGEX REPLACE "\n" " " ${fl-sources} ${fl-sources})

message("${exepath}")


# add sources of the logger library as a "LoggerCpp" library
add_library (LoggerCpp STATIC
 LoggerCpp/include/LoggerCpp/Channel.h
 LoggerCpp/include/LoggerCpp/Config.h
 LoggerCpp/include/LoggerCpp/DateTime.h
 LoggerCpp/include/LoggerCpp/Exception.h
 LoggerCpp/include/LoggerCpp/Formatter.h
 LoggerCpp/include/LoggerCpp/Log.h
 LoggerCpp/include/LoggerCpp/Logger.h
 LoggerCpp/include/LoggerCpp/LoggerCpp.h
 LoggerCpp/include/LoggerCpp/Manager.h
 LoggerCpp/include/LoggerCpp/Output.h
 LoggerCpp/include/LoggerCpp/OutputConsole.h
 LoggerCpp/include/LoggerCpp/OutputDebug.h
 LoggerCpp/include/LoggerCpp/OutputFile.h
 LoggerCpp/include/LoggerCpp/shared_ptr.hpp
 LoggerCpp/include/LoggerCpp/Utils.h
 LoggerCpp/src/Config.cpp
 LoggerCpp/src/DateTime.cpp
 LoggerCpp/src/Log.cpp
 LoggerCpp/src/Logger.cpp
 LoggerCpp/src/Manager.cpp
 LoggerCpp/src/OutputConsole.cpp
 LoggerCpp/src/OutputDebug.cpp
 LoggerCpp/src/OutputFile.cpp
)
#set_target_properties(LoggerCpp PROPERTIES COMPILE_FLAGS -fvisibility=hidden)


set(CMAKE_DEBUG_POSTFIX -dbg)

add_library(fl-shared SHARED ${fl-headers} ${fl-sources})
set_target_properties(fl-shared PROPERTIES OUTPUT_NAME fuzzylite)
set_target_properties(fl-shared PROPERTIES VERSION ${FL_VERSION} SOVERSION ${FL_VERSION})
target_link_libraries(fl-shared ${FL_LIBS} LoggerCpp)

#add_library(fl-static STATIC ${fl-headers} ${fl-sources})
#set_target_properties(fl-static PROPERTIES OUTPUT_NAME fuzzylite-static)
#target_link_libraries(fl-static ${FL_LIBS} LoggerCpp)

add_executable(fl-bin fl/Console.h src/Console.cpp src/main.cpp)
set_target_properties(fl-bin PROPERTIES OUTPUT_NAME fuzzylite)
set_target_properties(fl-bin PROPERTIES OUTPUT_NAME fuzzylite IMPORT_PREFIX _) #To prevent LNK1149 in Windows
set_target_properties(fl-bin PROPERTIES DEBUG_POSTFIX -dbg)
target_link_libraries(fl-bin fl-shared ${FL_LIBS})

include(GNUInstallDirs)

install(TARGETS fl-bin fl-shared
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		)

install(DIRECTORY fl/ DESTINATION include/fl)
install(DIRECTORY LoggerCpp/include/LoggerCpp DESTINATION include)

#if(WIN32)
#	set(fl-suffix)
#	add_custom_command(TARGET fl-bin POST_BUILD 
#		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
#        "${CMAKE_SOURCE_DIR}/lib/fuzzylite${FL_CONF}.dll"
#        ${CMAKE_SOURCE_DIR}/bin)                 # <--this is out-file path
#endif()

message("=====================================")
message("fuzzylite v.${FL_VERSION}b${FL_DATE}\n")
message("FL_DEBUG=${FL_DEBUG}")
message("FL_USE_FLOAT=${FL_USE_FLOAT}")
message("FL_BACKTRACE=${FL_BACKTRACE}")
message("=====================================\n")

# uninstall target
#configure_file(
    #"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    #"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    #IMMEDIATE @ONLY)

#add_custom_target(uninstall
    #COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

#unix uninstall
#xargs rm < install_manifest.txt

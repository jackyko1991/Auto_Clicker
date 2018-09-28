cmake_minimum_required(VERSION 3.1.0)

project(Auto_Clicker)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# hide console
if (WIN32)
option(HIDE_CONSOLE "Hide the console window" OFF)
endif(WIN32)

# Find the libraries
find_package(Qt5Widgets CONFIG REQUIRED QUIET)
find_package(Qt5Network CONFIG REQUIRED QUIET)

# Populate a CMake variable with the sources
file(GLOB UI_FILES *.ui)
file(GLOB HDR_FILES *.h)
file(GLOB CXX_FILES *.c*)
file(GLOB QRC_FILES ${PROJECT_SOURCE_DIR}/resource/resource.qrc)

qt5_wrap_ui(PROJECT_UI_H ${UI_FILES})
qt5_wrap_cpp(PROJECT_MOC ${HDR_FILES})
qt5_add_resources(PROJECT_QRC_SRC ${QRC_FILES})

# WINDOWS RESOURCES
set(
	WINDOWS_RESOURCES
	# PATH
	${PROJECT_SOURCE_DIR}/resource/resource.rc
)

# grouping in MSVC
source_group(
	"Generated Files"
	FILES
	${PROJECT_MOC}
	${PROJECT_UI_H}
	${PROJECT_QRC_SRC}
)

source_group(
	"Forms Files"
	FILES
	${UI_FILES}
)

# CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
if (HIDE_CONSOLE)
add_executable(Auto_Clicker MACOSX_BUNDLE WIN32
	${CXX_FILES} 
	${HDR_FILES} 
	${UI_FILES} 
	${QRC_FILES} 
	${WINDOWS_RESOURCES}
	${PROJECT_QRC_SRC}
	${PROJECT_MOC}
	${PROJECT_UI_H}
)
else(HIDE_CONSOLE)
add_executable(Auto_Clicker MACOSX_BUNDLE
	${CXX_FILES} 
	${HDR_FILES} 
	${UI_FILES} 
	${QRC_FILES} 
	${WINDOWS_RESOURCES}
	${PROJECT_QRC_SRC}
	${PROJECT_MOC}
	${PROJECT_UI_H}
)
endif(HIDE_CONSOLE)

qt5_use_modules(Auto_Clicker Core Gui)
target_link_libraries(Auto_Clicker Qt5::Widgets Qt5::Network)
cmake_minimum_required(VERSION 3.1.0)

project(Auto_Clicker)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED QUIET)

# Populate a CMake variable with the sources
file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.c*)

qt5_wrap_ui(UISrcs ${UI_FILES})
# CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
add_executable(Auto_Clicker MACOSX_BUNDLE
	${CXX_FILES} ${UISrcs} ${QT_WRAP})
qt5_use_modules(Auto_Clicker Core Gui)
target_link_libraries(Auto_Clicker Qt5::Widgets)
# CMakeList.txt : CMake project for NodeService, include source and define
# project specific logic here.
#
find_library(LIB1 libmicrohttpd-dll PATHS "C:/dev/vcpkg/installed/x64-windows/lib")
find_library(LIB2 sqlite3 PATHS "C:/dev/vcpkg/installed/x64-windows/lib")
find_library(LIB3 odbc32 PATHS "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64")
# Add source to this project's executable.
add_executable (NodeService "NodeService.cpp" "NodeService.h" "pingext.cpp" "pingext.h" "Tools.cpp" "Tools.h" "Sqlcon.h" "Sqlcon.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NodeService PROPERTY CXX_STANDARD 20)
endif()
target_include_directories(NodeService PRIVATE "C:/dev/vcpkg/installed/x64-windows/include")
link_directories("C:/dev/vcpkg/installed/x64-windows/lib")
target_link_libraries(NodeService ${LIB1} ${LIB2} ${LIB3})
target_compile_options(NodeService PRIVATE "-DSQLITE_THREADSAFE=2")
target_compile_options(NodeService PRIVATE "-lodbc32")
# TODO: Add tests and install targets if needed.

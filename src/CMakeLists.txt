# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/modern/*.hpp")
set(HEADER_LIST "${dist_SOURCE_DIR}/include/dist/entrypoint.hpp"
                "${dist_SOURCE_DIR}/include/dist/execution.hpp"
                "${dist_SOURCE_DIR}/include/dist/token.hpp"
                "${dist_SOURCE_DIR}/include/dist/lexing.hpp")

set(SOURCE_LIST "entrypoint.cpp" "execution.cpp" "lexing.cpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(dist_lib ${SOURCE_LIST} ${HEADER_LIST} ${GENERATED_FILES})

# We need this directory, and users of our library will need it too
target_include_directories(dist_lib PUBLIC ../include ${re2_SOURCE_DIR})

target_link_libraries(dist_lib PRIVATE re2)

# All users of this library will need at least C++17
target_compile_features(dist_lib PUBLIC cxx_std_17)

if(CLANG_TIDY_EXE)
  set_target_properties(
    dist_lib PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif()

# IDEs should put the headers in a nice place
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})

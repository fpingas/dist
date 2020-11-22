add_executable(dist main.cpp)

if(CLANG_FORMAT_EXE)
  add_dependencies(dist clangformat)
endif()

target_compile_features(dist PRIVATE cxx_std_17)

target_link_libraries(dist PRIVATE dist_lib)

if(CLANG_TIDY_EXE)
  set_target_properties(
    dist PROPERTIES
    CX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif()
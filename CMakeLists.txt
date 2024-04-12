cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Add external components as git submodules and include like: list(APPEND
# EXTRA_COMPONENT_DIRS external/<dir>)

project(esp-project)
idf_build_set_property(COMPILE_OPTIONS "-Wno-missing-field-initializers" APPEND)

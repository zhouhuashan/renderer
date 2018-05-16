cmake_minimum_required(VERSION 3.9)

project("raytracer" LANGUAGES CUDA)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xptxas='-v'")

list(APPEND HEADERS "rt.cuh")

list(APPEND SOURCES "rt.cu")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

target_include_directories(${PROJECT_NAME} INTERFACE ".")

set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)


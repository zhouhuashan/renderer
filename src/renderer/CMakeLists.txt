cmake_minimum_required(VERSION 3.9)

project("renderer" LANGUAGES CXX CUDA)

list(APPEND HEADERS "camera.hpp")
list(APPEND HEADERS "engine.hpp")
list(APPEND HEADERS "rendererinterface.hpp")
list(APPEND HEADERS "framebufferrenderer.hpp")
list(APPEND HEADERS "renderitem.hpp")
list(APPEND HEADERS "clipboard.hpp")

list(APPEND SOURCES "camera.cpp")
list(APPEND SOURCES "engine.cpp")
list(APPEND SOURCES "rendererinterface.cpp")
list(APPEND SOURCES "framebufferrenderer.cpp")
list(APPEND SOURCES "renderitem.cpp")
list(APPEND SOURCES "clipboard.cpp")
list(APPEND SOURCES "main.cpp")

add_translation(QM_FILES "${PROJECT_NAME}.ru_RU")

qt5_add_resources(RESOURCES "${PROJECT_NAME}.qrc" "qml/${PROJECT_NAME}_qml.qrc")
set_property(SOURCE ${RESOURCES} PROPERTY SKIP_AUTOGEN ON)

add_executable(${PROJECT_NAME} ${OS_BUNDLE} ${SOURCES} ${HEADERS} ${RESOURCES} ${QM_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE "utility" "raytracer")

target_compile_definitions(${PROJECT_NAME} PRIVATE -DPROJECT_NAME="${PROJECT_NAME}")

qt5_use_modules(${PROJECT_NAME} LINK_PRIVATE Core Gui Qml Quick QuickControls2 LinguistTools)



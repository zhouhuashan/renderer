cmake_minimum_required(VERSION 3.9)

project("src")

add_definitions(-DORGANIZATION_NAME="${ORGANIZATION_NAME}")
add_definitions(-DORGANIZATION_SHORTNAME="${ORGANIZATION_SHORTNAME}")
add_definitions(-DORGANIZATION_DOMAIN="${ORGANIZATION_DOMAIN}")
add_definitions(-DPACKAGE_NAME="${PACKAGE_NAME}")

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS -DQT_USE_QSTRINGBUILDER)

#add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DQT_RESTRICTED_CAST_FROM_ASCII)
#add_definitions(-DQT_NO_CAST_FROM_ASCII)
add_definitions(-DQT_NO_CAST_TO_ASCII)
#add_definitions(-DQT_NO_PROCESS_COMBINED_ARGUMENT_START)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
add_definitions(-DQT_DEPRECATED_WARNINGS)

add_definitions(-DQT_MESSAGELOGCONTEXT)

add_subdirectory("utility")
add_subdirectory("raytracer")
add_subdirectory("renderer")

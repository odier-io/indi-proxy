########################################################################################################################

cmake_minimum_required(VERSION 3.25)

########################################################################################################################

project(indi_proxy C)

########################################################################################################################

set(CMAKE_C_STANDARD 99)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O0 -g")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -flto -ffast-math -fomit-frame-pointer")

########################################################################################################################

find_package(LibXml2 REQUIRED)

########################################################################################################################

add_library(indi-proxy
    src/indi_proxy.h
    src/indi_proxy_schema.h
    src/indi_proxy_internal.h
    #
    src/structs/indi_boolean.c
    src/structs/indi_dict.c
    src/structs/indi_list.c
    src/structs/indi_null.c
    src/structs/indi_number.c
    src/structs/indi_string.c
    #
    src/alloc.c
    src/json.c
    src/proxy.c
    src/transform_json_to_xml.c
    src/transform_xml_to_json.c
    src/validation.c
    src/xml.c
)

target_include_directories(indi-proxy PRIVATE ${LIBXML2_INCLUDE_DIR})

########################################################################################################################

add_executable(test1 test1.c)

target_link_libraries(test1 ${LIBXML2_LIBRARIES} indi-proxy)

########################################################################################################################

add_executable(test2 test2.c)

target_link_libraries(test2 ${LIBXML2_LIBRARIES} indi-proxy)

########################################################################################################################

########################################################################################################################

cmake_minimum_required(VERSION 3.0)

########################################################################################################################

project(indi_proxy C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused-result -O3 -fPIC -flto -fomit-frame-pointer")

########################################################################################################################

find_package(LibXml2 REQUIRED)

########################################################################################################################

include(CheckFunctionExists)

check_function_exists(malloc_size HAVE_MALLOC_SIZE)

check_function_exists(malloc_usable_size HAVE_MALLOC_USABLE_SIZE)

########################################################################################################################

set(SOURCE_FILES
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
    #
    src/control.c
)

########################################################################################################################

add_library(indi-proxy-static STATIC ${SOURCE_FILES})

if(HAVE_MALLOC_SIZE)
    target_compile_definitions(indi-proxy-static PRIVATE HAVE_MALLOC_SIZE)
endif()

if(HAVE_MALLOC_USABLE_SIZE)
    target_compile_definitions(indi-proxy-static PRIVATE HAVE_MALLOC_USABLE_SIZE)
endif()

target_include_directories(indi-proxy-static PRIVATE ${LIBXML2_INCLUDE_DIR})

######_link_libraries(indi-proxy-static ${LIBXML2_LIBRARIES})

set_target_properties(indi-proxy-static PROPERTIES
    OUTPUT_NAME "indi-proxy"
)

########################################################################################################################

add_library(indi-proxy-shared SHARED ${SOURCE_FILES})

if(HAVE_MALLOC_SIZE)
    target_compile_definitions(indi-proxy-shared PRIVATE HAVE_MALLOC_SIZE)
endif()

if(HAVE_MALLOC_USABLE_SIZE)
    target_compile_definitions(indi-proxy-shared PRIVATE HAVE_MALLOC_USABLE_SIZE)
endif()

target_include_directories(indi-proxy-shared PRIVATE ${LIBXML2_INCLUDE_DIR})

target_link_libraries(indi-proxy-shared ${LIBXML2_LIBRARIES})

set_target_properties(indi-proxy-shared PROPERTIES
    OUTPUT_NAME "indi-proxy"
)

########################################################################################################################

add_executable(test1 test/test1.c)

target_link_libraries(test1 ${LIBXML2_LIBRARIES} indi-proxy-static)

########################################################################################################################

add_executable(test2 test/test2.c)

target_link_libraries(test2 ${LIBXML2_LIBRARIES} indi-proxy-static)

########################################################################################################################

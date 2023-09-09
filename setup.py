#!/usr/bin/env python3
########################################################################################################################

import ctypes
import ctypes.util

from setuptools import setup, Extension

########################################################################################################################

def check_function_exists(lib_name, func_name):

    # noinspection PyBroadException
    try:

        lib = ctypes.CDLL(lib_name)

        getattr(lib, func_name)

        return True

    except:

        return False

########################################################################################################################

define_macros = []

libc_name = ctypes.util.find_library('c')

if check_function_exists(libc_name, 'malloc_size'):
    define_macros.append(('HAVE_MALLOC_SIZE', '1'))

if check_function_exists(libc_name, 'malloc_usable_size'):
    define_macros.append(('HAVE_MALLOC_USABLE_SIZE', '1'))

########################################################################################################################

indi_proxy_module = Extension(
    'indi_proxy',
    sources = [
        'src/structs/indi_boolean.c',
        'src/structs/indi_dict.c',
        'src/structs/indi_list.c',
        'src/structs/indi_null.c',
        'src/structs/indi_number.c',
        'src/structs/indi_string.c',
        #
        'src/alloc.c',
        'src/json.c',
        'src/proxy.c',
        'src/transform_json_to_xml.c',
        'src/transform_xml_to_json.c',
        'src/validation.c',
        'src/xml.c',
        #
        'src/control.c',
        #
        'src/module.c',
    ],
    include_dirs = [
        '/usr/include/libxml2'
    ],
    define_macros = define_macros
)

########################################################################################################################

def prepare():

    ####################################################################################################################

    with open('src/schema/indi.xsd', 'rb') as f:

        data_in = bytearray(f.read())

    ####################################################################################################################

    data_len = len(data_in)
    data_out = (((('\n'))))

    for part in range(0, data_len, 16):

        data_out += f'\t{", ".join(["0x%02X" % b for b in data_in[part: part + 16]])},\n'

    ####################################################################################################################

    with open('src/indi_proxy_schema.h', 'wt') as f:

        f.write(f'/* file: indi.xsd */\n\n')

        f.write(f'#define INDI_PROXY_XSD_SIZE {data_len}\n\n')

        f.write(f'char indi_proxy_xsd_buff[] = {{{data_out}}};\n')

########################################################################################################################

if __name__ == '__main__':

    prepare()

    setup(
        name = 'indi_proxy',
        version = '1.0.0',
        description = 'Blabla',
        author = 'Jérôme ODIER',
        ext_modules = [indi_proxy_module]
    )

########################################################################################################################

#!/usr/bin/env python3
########################################################################################################################

import indi_proxy

import eventlet.green

########################################################################################################################

def _emit(msg):

    doc = indi_proxy.xmldoc_parse(msg)

    if doc != 0:

        obj = indi_proxy.xmldoc_to_object(doc, False)

        if obj != 0:

            print(indi_proxy.object_to_string(obj).decode('utf-8'))

            indi_proxy.xmldoc_free(doc)

        indi_proxy.object_free(obj)

########################################################################################################################

def main():

    indi_proxy.memory_initialize()

    proxy = indi_proxy.proxy_initialize(
        10 * 1024 * 1024,
        _emit
    )

    sock = eventlet.green.socket.socket(eventlet.green.socket.AF_INET, eventlet.green.socket.SOCK_STREAM)

    try:

        sock.connect(('localhost', 7624))

        print('Connected to server.')

        sock.sendall('<getProperties version="1.7" />\n'.encode())

        while True:

            data = sock.recv(1024)

            if not data:
                break

            indi_proxy.proxy_consume(proxy, data)

    except Exception as e:

        print(f'Error occurred: {e}')

    finally:

        indi_proxy.proxy_finalize(proxy)

        indi_proxy.memory_finalize()

        sock.close()

########################################################################################################################

if __name__ == '__main__':

    main()

########################################################################################################################

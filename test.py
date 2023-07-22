#!/usr/bin/env python3
########################################################################################################################

import indi_proxy

########################################################################################################################

def emit(message):

    print(message)

########################################################################################################################

if __name__ == '__main__':

    indi_proxy.memory_initialize()

    proxy = indi_proxy.proxy_initialize(1024, emit)

    indi_proxy.proxy_consume(proxy, '....................................................<defSwitchVector>foo</def')
    indi_proxy.proxy_consume(proxy, 'SwitchVector>........')

    indi_proxy.proxy_consume(proxy, '...........................................<defNumberVector>foo')
    indi_proxy.proxy_consume(proxy, '</defNumberVector>................................')

    indi_proxy.proxy_finalize(proxy)

    indi_proxy.memory_finalize()

########################################################################################################################

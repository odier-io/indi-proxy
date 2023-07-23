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

    xml = '''
<defSwitchVector device="CCD Simulator" name="CONNECTION" label="Connection" group="Main Control" state="Ok" perm="rw" rule="OneOfMany" timeout="60" timestamp="2023-07-22T17:06:07">
    <defSwitch name="CONNECT" label="Connect">
On
    </defSwitch>
    <defSwitch name="DISCONNECT" label="Disconnect">
Off
    </defSwitch>
</defSwitchVector>
    '''.encode('utf-8')

    #indi_proxy.proxy_consume(proxy, '....................................................<defSwitchVector>foo</def'.encode('utf-8'))
    #indi_proxy.proxy_consume(proxy, 'SwitchVector>........'.encode('utf-8'))

    #indi_proxy.proxy_consume(proxy, '...........................................<defNumberVector>foo'.encode('utf-8'))
    #indi_proxy.proxy_consume(proxy, '</defNumberVector>................................'.encode('utf-8'))

    indi_proxy.proxy_consume(proxy, xml)

    indi_proxy.proxy_finalize(proxy)

    indi_proxy.memory_finalize()

########################################################################################################################

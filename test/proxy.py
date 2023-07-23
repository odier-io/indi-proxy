#!/usr/bin/env python3
########################################################################################################################

import flask
import atexit
import eventlet
import eventlet.green
import indi_proxy

########################################################################################################################

app = flask.Flask(__name__)

########################################################################################################################

class Client(object):

    ####################################################################################################################

    def __init__(self, host, port):

        self.host = host
        self.port = port

        self.out_message_queue = eventlet.queue.Queue()
        self.in_message_queue = eventlet.queue.Queue()

        indi_proxy.memory_initialize()

        self.proxy = indi_proxy.proxy_initialize(
            10 * 1024 * 1024,
            self.emit
        )

        atexit.register(self.bye)

    ####################################################################################################################

    def bye(self):

        indi_proxy.proxy_finalize(self.proxy)

        indi_proxy.memory_finalize()

        print('Bye!')

    ####################################################################################################################

    def emit(self, msg):

        doc = indi_proxy.xmldoc_parse(msg)

        if doc != 0:

            obj = indi_proxy.xmldoc_to_object(doc, False)

            if obj != 0:

                json = indi_proxy.object_to_string(obj)

                print(json.decode('utf-8'))
                print()

                indi_proxy.xmldoc_free(doc)

            indi_proxy.object_free(obj)

    ####################################################################################################################

    def start(self):

        ################################################################################################################

        sock = eventlet.green.socket.socket(eventlet.green.socket.AF_INET, eventlet.green.socket.SOCK_STREAM)

        sock.connect((self.host, self.port, ))

        ################################################################################################################

        #self.send_message('{"<>": "getProperties", "@version": "1.7"}')

        self.send_message('<getProperties version="1.7" />')

        ################################################################################################################

        def receive_messages():

            while True:

                indi_proxy.proxy_consume(self.proxy, sock.recv(1024))

        ################################################################################################################

        def send_messages():

            while True:

                while not self.out_message_queue.empty():

                    sock.sendall(self.out_message_queue.get().encode('utf-8'))

                eventlet.sleep(0.1)

        ################################################################################################################

        eventlet.spawn_n(receive_messages)

        eventlet.spawn_n(send_messages)

    ####################################################################################################################

    def send_message(self, message):

        self.out_message_queue.put(message)

########################################################################################################################

@app.route('/')
def hello_world():

    return 'Hello, World!'

########################################################################################################################

if __name__ == '__main__':

    eventlet.monkey_patch()

    client = Client('localhost', 7624)

    eventlet.spawn(client.start)

    app.run(host = '0.0.0.0', port = 5050, debug = True, threaded = True)

########################################################################################################################

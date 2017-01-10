import argparse
import json
import time
import zmq

# If dispatcher is on Windows or another system with above-microsecond precision, this
# would have to be changed.
UTC_OFFSET = -(time.altzone if time.daylight else time.timezone)*1000000

def utc_to_local(utc):
    '''
    Convert a UTC unix timestamp with microsecond precision to one in the local timezone.
    '''
    return utc + UTC_OFFSET

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='dspy', description='dispatcher log client')
    
    parser.add_argument(
            '-e',
            '--endpoint',
            metavar='dispatcher',
            dest='dispatcher',
            type=str,
            required=True,
            help='Dispatcher logging endpoint to connect to.'
        )
    actionFilterChoices = ('configure_detector',
                          'tx', 
                          'configure_qswitch',
                          'configure_node',
                          'rx',
                          'simulator_request',
                          'simulator_response')
    parser.add_argument(
            '-a',
            '--action',
            metavar='action',
            dest='action',
            choices=actionFilterChoices,
            type=str,
            default='',
            help='Filter actions to one of the following: {}.'.format(actionFilterChoices)
        )
    args = parser.parse_args()
    
    context = zmq.Context(1)
    socket = context.socket(zmq.SUB)
    socket.setsockopt(zmq.SUBSCRIBE, args.action)
    socket.connect(args.dispatcher)
    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)
    
    while True:
        socks = dict(poller.poll(500))
        if socks and socks.get(socket) == zmq.POLLIN:
            # [topic, message]
            data =socket.recv_multipart()
            # JSON loader doesn't like null terminators
            jmsg = json.loads(data[1][:-1])
            # We get the time in UTC so we translate to the local timezone
            jmsg['time'] = utc_to_local(jmsg['time'])
            print(jmsg)
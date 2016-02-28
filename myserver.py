__author__ = 'johnkalin'

import asyncore
import socket

conns = []

class EchoHandler(asyncore.dispatcher_with_send):

    def handle_read(self):
        data = self.recv(8192)
        print (data)
        if data:
            #self.send(data)
            sendAll(data)

def sendAll(data):
	for conn in conns:
		try:
			conn.send(data)
		except:
			pass

class EchoServer(asyncore.dispatcher):

    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
		
        self.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)        
		#self.set_reuse_addr()
        self.bind((host, port))
        self.listen(5)

    def handle_accept(self):
        pair = self.accept()
        if pair is not None:
            sock, addr = pair
            conns.append(sock)
            print ('Incoming connection from %s') % repr(addr)
            handler = EchoHandler(sock)

server = EchoServer('', 50650)
asyncore.loop()

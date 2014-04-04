all : Chat.c Leer

CPPFLAGS = -g -I.

Leer : Chat.c
	
	cc -g -I. Chat.c -o Chat

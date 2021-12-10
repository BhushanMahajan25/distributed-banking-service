#makefile

CC = g++

CFLAGS = -c --std=c++11

OBJDIR = obj

SRCDIR = src

UTILDIR = util

BINDIR = bin

compile : $(BINDIR)/server $(BINDIR)/front $(BINDIR)/client

$(BINDIR)/server : $(OBJDIR)/service.o $(OBJDIR)/server.o
	$(CC) $(OBJDIR)/service.o $(OBJDIR)/server.o -o $(BINDIR)/server -lpthread

$(BINDIR)/front : $(OBJDIR)/service.o $(OBJDIR)/front.o
	$(CC) $(OBJDIR)/service.o $(OBJDIR)/front.o -o $(BINDIR)/front -lpthread

$(BINDIR)/client : $(OBJDIR)/service.o $(OBJDIR)/client.o
	$(CC) $(OBJDIR)/service.o $(OBJDIR)/client.o -o $(BINDIR)/client

$(OBJDIR)/server.o : $(SRCDIR)/server.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/server.cpp -o $(OBJDIR)/server.o

$(OBJDIR)/front.o : $(SRCDIR)/front-end-server.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/front-end-server.cpp -o $(OBJDIR)/front.o

$(OBJDIR)/client.o : $(SRCDIR)/client.cpp
	$(CC) $(CFLAGS) $(SRCDIR)/client.cpp -o $(OBJDIR)/client.o

$(OBJDIR)/service.o : $(UTILDIR)/services.cpp
	$(CC) $(CFLAGS) $(UTILDIR)/services.cpp -o $(OBJDIR)/service.o

clean : 
	rm $(OBJDIR)/* $(BINDIR)/server $(BINDIR)/client $(BINDIR)/front
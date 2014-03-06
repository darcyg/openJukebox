CC= gcc
CFLAGS= -std=gnu99 
LDFLAGS+=-L$(HOME)/libs -lmpdclient 
# build the serialMPDClient executable when user executes "make"
serialMPDClient: serialMPDClient.o
	$(CC) $(LDFLAGS) serialMPDClient.o -o serialMPDClient
serialMPDClient.o: serialMPDClient.c
	$(CC) $(CFLAGS) -c serialMPDClient.c -lmpdclient 

# remove object files and executable when user executes "make clean"
clean:
	rm *.o serialMPDClient


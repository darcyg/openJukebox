# build the serialMPDClient executable when user executes "make"
serialMPDClient: serialMPDClient.o
	$(CC) $(LDFLAGS) serialMPDClient.o -o serialMPDClient
serialMPDClient.o: serialMPDClient.c
	$(CC) $(CFLAGS) -c serialMPDClient.c

# remove object files and executable when user executes "make clean"
clean:
	rm *.o serialMPDClient


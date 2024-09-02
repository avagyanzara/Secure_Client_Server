C99=gcc
#CFLAGS=-D _POSIX_C_SOURCE=200112L -D _ANSI_SOURCE -std=c99
CFLAGS=-D _POSIX_C_SOURCE=200112L -std=c99
APP:=app

all: server client

clean:
	rm -rf $(APP)

client: $(APP)/client/client.o libs
	$(C99) $(CFLAGS) -g $(APP)/client/client.o $(APP)/libs/*.o -o $(APP)/client/client

$(APP)/client/client.o: client.c
	mkdir -p $(APP)/client
	$(C99) $(CFLAGS) -c client.c -o $(APP)/client/client.o

clean_client:
	rm -rf $(APP)/client

server: $(APP)/server/server.o libs
	$(C99) $(CFLAGS) -g $(APP)/server/server.o $(APP)/libs/*.o -o $(APP)/server/server

$(APP)/server/server.o: server.c
	@mkdir -p $(APP)/server
	@echo "Compiling server..."
	$(C99) $(CFLAGS) -c server.c -o $(APP)/server/server.o

libs: $(APP)/libs/utility.o $(APP)/libs/socket.o $(APP)/libs/encryption.o
	@echo "Compiling libraries..."

$(APP)/libs/utility.o: utility.c
	@mkdir -p $(APP)/libs
	$(C99) $(CFLAGS) -c utility.c -o $(APP)/libs/utility.o

$(APP)/libs/socket.o: socket.c
	@mkdir -p $(APP)/libs
	$(C99) $(CFLAGS) -c socket.c -o $(APP)/libs/socket.o

$(APP)/libs/encryption.o: encryption.c
	@mkdir -p $(APP)/libs
	$(C99) $(CFLAGS) -c encryption.c -o $(APP)/libs/encryption.o

server_clean:
	rm -rf $(APP)/server

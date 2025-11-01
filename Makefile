.PHONY: all client server clean

all: client server

export CPATH=/usr/include/postgresql:$CPATH
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH

LDFLAGS += -L/usr/lib/x86_64-linux-gnu
LIBS += -lpq

CFLAGS += -I/usr/include/postgresql

client:
	$(MAKE) -C client

server:
	$(MAKE) -C server

clean:
	$(MAKE) -C client clean
	$(MAKE) -C server clean

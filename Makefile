.PHONY: all libraries client server clean

all: libraries client server

export CPATH=/usr/include/postgresql:$CPATH
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH

LDFLAGS += -L/usr/lib/x86_64-linux-gnu
LIBS += -lpq

CFLAGS += -I/usr/include/postgresql

libraries:
	$(MAKE) -C libraries/cJSON
	$(MAKE) -C libraries/sqlite

client:
	$(MAKE) -C client

server:
	$(MAKE) -C server

clean:
	$(MAKE) -C libraries/cJSON clean
	$(MAKE) -C libraries/sqlite clean
	$(MAKE) -C client clean
	$(MAKE) -C server clean

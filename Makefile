.PHONY: all libraries client server clean

all: libraries client server

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

CFLAGS += $(shell pkg-config --cflags --libs gtk+-2.0 webkit-1.0)
CFLAGS += -ggdb

change-uri: change-uri.c
	$(CC) $(CFLAGS) -o $@ change-uri.c

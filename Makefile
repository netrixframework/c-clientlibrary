SHELL := /bin/sh
CC := gcc
WFLAGS := -Wno-void-pointer-to-int-cast -Wno-int-to-void-pointer-cast -Wno-incompatible-pointer-types-discards-qualifiers
CFLAGS := -I/usr/local/include -I/usr/local/include/json-c -Iinclude $(WFLAGS)
LDFLAGS := -shared -L/usr/local/lib -ljson-c -lcurl

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

TARGET := libnetrixclient.so
SOURCES := $(shell echo src/*.c)
HEADERS := $(shell echo include/*.h)
OBJECTS := $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

clean: 
	-@rm src/*.o $(TARGET)

install: 
	install -d $(DESTDIR)$(PREFIX)/include/libnetrixclient
	install -c $(HEADERS) $(DESTDIR)$(PREFIX)/include/libnetrixclient
	install -d $(DESTDIR)$(PREFIX)/lib
	install $(TARGET) $(DESTDIR)$(PREFIX)/lib
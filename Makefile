SHELL := /bin/sh
CC := gcc
WFLAGS := -Wno-void-pointer-to-int-cast -Wno-int-to-void-pointer-cast -Wno-incompatible-pointer-types-discards-qualifiers -Wno-int-conversion
CFLAGS := -I/usr/local/include -I/usr/local/include/json-c -Iinclude $(WFLAGS)
LDFLAGS := -shared -L/usr/local/lib -ljson-c -lcurl
TESTLDFLAGS := -L/usr/local/lib -ljson-c -lcurl -lnetrixclient

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

TARGET := libnetrixclient.so
TESTTARGET := test.o
SOURCES := $(shell echo src/*.c)
TESTSOURCES := $(shell echo test/*.c)
HEADERS := $(shell echo include/*.h)
OBJECTS := $(SOURCES:.c=.o)

all: $(TARGET)
	$(CC) $(CFLAGS) $(LDFLAGS) 

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

test: $(TESTTARGET)
	./$(TESTTARGET)
	rm $(TESTTARGET)

$(TESTTARGET): $(TARGET)
	$(CC) -Itest $(CFLAGS) $(TESTLDFLAGS) $(TESTSOURCES) -o $(TESTTARGET)

clean: 
	-@rm src/*.o $(TARGET)

install: 
	install -d $(DESTDIR)$(PREFIX)/include/libnetrixclient
	install -c $(HEADERS) $(DESTDIR)$(PREFIX)/include/libnetrixclient
	install -d $(DESTDIR)$(PREFIX)/lib
	install $(TARGET) $(DESTDIR)$(PREFIX)/lib
INSTALL_DIR ?= /usr/local/bin

# Dynamic
LDFLAGS += -lkinotto

# Static
# LDFLAGS += libkinotto.a

CFLAGS += -Wall

C_FILES := $(wildcard *.c)
OBJ_FILES := $(patsubst %.c, %.o,$(C_FILES))

CC ?= gcc

.PHONY = kinottocli install clean

all: kinottocli

kinottocli: $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	install -m 755 kinottocli $(INSTALL_DIR)

clean:
	rm -f *.o kinottocli

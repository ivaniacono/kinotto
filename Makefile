OBJ_INSTALL_DIR ?= /usr/local/lib
HEADERS_INSTALL_DIR ?= /usr/local/include/kinotto

WPA_SUPPLICANT := ./wpa_supplicant

CFLAGS += \
	-std=c99 \
	-Iinclude/ -I$(WPA_SUPPLICANT) \
	-DDHCLIENT \
	-fPIC -Wall

WPA_CFLAGS += \
	-I$(WPA_SUPPLICANT) \
	-DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX \
	-fPIC -Wall

# -DCONFIG_BACKEND_FILE -DCONFIG_IEEE80211W  -DCONFIG_DRIVER_WEXT \
# -DCONFIG_WIRELESS_EXTENSION  -DCONFIG_DRIVER_NL80211 -DCONFIG_LIBNL20 \
# -DEAP_PSK -DIEEE8021X_EAPOL -DCONFIG_SHA256 -DCONFIG_CTRL_IFACE \
# -DCONFIG_CTRL_IFACE_UNIX  -DCONFIG_SME

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c,%.o,$(C_FILES))

WPA_C_FILES := $(wildcard $(WPA_SUPPLICANT)/*.c)
WPA_OBJ_FILES := $(patsubst $(WPA_SUPPLICANT)/%.c,%.o,$(WPA_C_FILES))

CC ?= gcc

.PHONY = doc libkinotto.so libkinotto.a install clean clean_doc

all: libkinotto.so libkinotto.a

doc:
	rm -rf docs
	doxygen Doxyfile

libkinotto.so: $(OBJ_FILES) $(WPA_OBJ_FILES)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

libkinotto.a: $(OBJ_FILES) $(WPA_OBJ_FILES)
	ar rcs $@ $^

os_unix.o: $(WPA_SUPPLICANT)/os_unix.c
	$(CC) $(WPA_CFLAGS) -c -o $@ $<

wpa_ctrl.o: $(WPA_SUPPLICANT)/wpa_ctrl.c
	$(CC) $(WPA_CFLAGS) -c -o $@ $<

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	install -m 644 libkinotto.so $(OBJ_INSTALL_DIR)
	ldconfig
	install -m 755 -d $(HEADERS_INSTALL_DIR)
	install -m 644 include/*.h $(HEADERS_INSTALL_DIR)

clean:
	rm -f *.a *.so *.o

clean_doc:
	rm -rf docs

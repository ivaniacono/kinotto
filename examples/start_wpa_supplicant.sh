#!/bin/sh

CTRL_IFACE_DIR="/var/run/wpa_supplicant/"
WPA_SUPPLICANT_CONF="$(pwd)/wpa_supplicant.conf"

if [ "$#" -eq 0 ]; then
    echo "Usage: $0 INTERFACE"
    exit 1
fi

IFACE="$1"

service network-manager stop 2>/dev/null
killall wpa_supplicant 2>/dev/null
killall dhclient 2>/dev/null
sleep 1

wpa_supplicant -i "$IFACE" -C "$CTRL_IFACE_DIR" -B -c "$WPA_SUPPLICANT_CONF"
dhclient "$IFACE"

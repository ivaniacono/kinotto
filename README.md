# Kinotto

```
   _                                       
  !.!     _   ___             _   _        
  : :    | | / (_)           | | | |       
 :   :   | |/ / _ _ __   ___ | |_| |_ ___  
!_____!  |    \| | '_ \ / _ \| __| __/ _ \ 
!__K__!  | |\  \ | | | | (_) | |_| || (_) |
:     :  \_| \_/_|_| |_|\___/ \__|\__\___/ 
 '''''                                     
```

Kinotto is a library designed to simplify developing systems which need to
interact with network configuration tools, such as embedded systems.

The idea behind it is providing a consistent interface across different platforms,
abstracting the platform specific system calls.

Currently the library offers the following functionalities:
- Assigning static IPv4 addresses
- Assigning DHCP addresses (currently via dhclient)
- Assigning MAC addresses including random ones
- Connecting to WPA/WPA2/Open Wi-Fi networks (via wpa_supplicant)
- Disconnecting from a Wi-Fi network (via wpa_supplicant)
- Saving Wi-Fi network information (currently in wpa_supplicant format)
- Retriving Wi-Fi network status
- Retriving interface status

## Usage
Building the library:

`$ make`

This will produce `libkinotto.a` and `libkinotto.so`.

Installing shared object:

`# make install` (it will install `libkinotto.so` under `/usr/local/lib`)

## Developing
Documentation is availabe [here](http://ivaniacono.com/kinotto/).

## Running the example kinottocli
An example project that uses kinotto to provide some network configuration functionalities is available under the `examples` folder.

`$ cd examples`

`$ make`

which produces the binary `kinottocli`.

Optionally the example binary can be installed with:

`# make install` (it will install it under `/usr/local/bin`)

NOTE: if you're running network-manager you will probably need to stop / disable it first to avoid your settings to be changed by the former without notice.

A simple script which automates stopping network-manager and starting WPA supplicant is provided under the same folder. WPA supplicant is required if managing a Wi-Fi network is desired.

`# ./start_wpa_supplicant.sh wlan0`

Running `./kinottocli` without arguments will print the help.

## Notes
The project so far has only been tested on Ubuntu and Debian systems. Support for other OS such as *BSD will be added soon.

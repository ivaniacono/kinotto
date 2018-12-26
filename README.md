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

Kinotto is a library designed to simplify writing custom network configuration tools which can speed up the development of *nix based embedded systems.

__This is very much a work in progress therefore not production ready.__

Its API and design is also very luckily to change.

Currently offers the following functionalities:
- Assigning static IPv4 address
- Assigning DHCP address (currently via dhclient)
- Connecting to WPA/WPA2/Open Wi-Fi networks (via wpa_supplicant)
- Retriving Wi-Fi network status
- Retriving interface status

## Usage
Build the library:

`$ make`.

Then install it:

`# make install` (it will install `libkinotto.so` under `/usr/local/lib`)

## Developing
An example project that uses kinotto to provide some network configuration functionalities is available under the `examples` folder.

## Running the example
Once the library is installed run:

`$ cd examples`

`$ make`

which produces the binary `kinottocli`.

Optionally the example binary can be installed with:

`# make install` (it will install it under `/usr/local/bin`)

NOTE: if you're running network-manager you will probably need to stop / disable it first to avoid your settings to be changed by the former without notice.

A simple script that automates stopping network-manager and starting WPA supplicant is provided under the same folder. WPA supplicant is required if managing a Wi-Fi network is desired.

`# start_wpa_supplicant.sh wlan0`

Running `./kinottocli` without arguments will print the help.

## Notes
The project so far has only been tested on Ubuntu and Debian systems. Support for other OS such as *BSD will be added soon.

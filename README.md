![](https://i.imgur.com/wJ6tGHd.png)

minimalistic SOCKS5 proxy-server for Linux

Features
- IPv4 & IPv6 support
- Logging to CSV file 
- CONNECT only, because BIND and UDP ASSOC commands are poorly documented and are widely unsupported
## Dependencies
- POSIX

## Installation
1. Clone repo 
```
https://github.com/spawn18/tightsocks
cd tightsocks
``` 

2. Build with CMake
```
mkdir build
cd build
cmake ..
cmake --build .
```


## Usage
Basic
```
./tightsocks
```

With logging, IPv4 and port changed, run as daemon
```
./tightsocks --ipv4 --log --port=5060 &
```


```
Usage: %s [OPTION]... \n\n"
           "  -4, --ipv4                                accept IPv4 connections\n"
           "  -6, --ipv6                                accept IPv6 connections\n"
           "                                            if unset, both are assumed\n"
           "  -l, --log                                 enable access logging to csv file\n"
           "  -p, --port=NUMBER                         set server port [default: 1080]\n"
           "  -c, --max-connections=LIMIT               limit for connections [default: 1024]\n"
           "  -b, --bufsize=SIZE                        set data buffer size in bytes [default: 65535]\n"
           "  -v, --verbose                             print information\n"
           "  -h, --help                                print this usage guide \n
```

## Logging
Each connection is logged to log.csv file with the following signature


| date  | time | source host | source port | command | address type | destination host | destination port |
| ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | ------------- | 

COMMAND is always CONNECT


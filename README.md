# memutil
Simple program to read/write from/to any location in memory for AARCH64
# How to compile 
```bash
$ gcc memutil.c -o memutil
``` 
# Usage
```bash
Usage:  ./memutil { address } [ type [ data ] ]
        address : memory address to act upon
        type    : access operation type : [b]yte, [h]alfword, [w]ord
        data    : data to be written
``` 
# Example
## Read memory
```bash
$ ./memutil 0x1c020000 w
``` 
## Write to memory
```bash
$ ./memutil 0x1c020000 w 0xABCD
``` 

# Purpose
Display system information of the Intel Core Processor, BIOS, and RAM memories 
* Processor features (SMBios & CPUID) 
* Core i7 frequency (MSR) 
* Temperature for each Core (MSR) 

![](http://blog.cyring.free.fr/wp-content/uploads/2012/09/CyrIng_coremod_screen18.jpg)
![](http://blog.cyring.free.fr/wp-content/uploads/2012/09/CyrIng_coremod_screen19.jpg)
![](http://blog.cyring.free.fr/wp-content/uploads/2012/07/CyrIng_coremod_screen12.jpg)

* BIOS summary 
* DDR3 timings (through memory controller I/O) 
* Motherboard 

![](http://blog.cyring.free.fr/wp-content/uploads/2012/07/CyrIng_coremod_screen17.jpg)
![](http://blog.cyring.free.fr/wp-content/uploads/2012/09/CyrIng_coremod_screen20.jpg)

* Base Clock frequency 
* Processor socket and voltage 
* Cache Memories 

![](http://blog.cyring.free.fr/wp-content/uploads/2012/07/CyrIng_coremod_screen11.jpg)

# How to build CoreMod
## Requirements
_Mandatory_ :
* Linux x86_64 3.x or 4.x
* The GNU Compiler Collection: gcc - or - The C language family frontend for LLVM: clang
* Being root !
* Any Intel Core, i7, i5, i3 Processor

_Optionally_ :
* The GNU Debugger: gdb and one graphical front-end for gdb: ddd
* [The Intel 64 and IA-32 Architectures Software Developer’s Manual Volume 3](http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html)
* [The SMBIOS Reference Specification](http://www.dmtf.org/standards/smbios) (C) Distributed Management Task Force

## Compilation
Clone or download the source code then build the binary with ```make``` 
```
$ make
Compiling : Done.
Linking   : Done.
Ready     : ./bin/coremod
Remark    : You must be root to run CoreMod
```
## Debug
* Compile sources with the -g switch, or set it in the Makefile's CFLAGS
* Run the debugger GUI
```
# ddd ./bin/coremod
```
# How to use CoreMod
Two kernel modules must be loaded with modprobe or insmod
* msr
* dmi-sysfs

then execute CoreMod as root
```
# ./coremod
```

![](http://blog.cyring.free.fr/wp-content/uploads/2012/07/CyrIng_coremod_screen12.jpg)

_press the menu key of the function you want to execute_.

# Algorithm

![](http://blog.cyring.free.fr/images/CoreMod_algorithm.png)

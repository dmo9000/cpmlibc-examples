cpmlibc
=======

A C library for CPM2.2, may work for other versions as well but not tested at this point. Primary test environment
is z80pack and an unamed UE4 project where the object of the game is to control your spaceship with software run from 
a small computer that is based in reality (ie. a "z80pack compatible"). 

The goal is to keep the base C library compatible with other Z80/CPM systems but to offer extended functionality for the
UE4 computer architecture (ie. PSG's, graphics etc). 

check the libc/.arf files for examples of the different profiles available (standard, mini, tiny etc). At the moment 
the only difference is that mini/tiny leave out the fcntl module - this makes about a 7k or 8k difference in executable
size at the moment. Use the CP/M BDOS directly if you don't want to use the fcntl module. 


issues
=======
fcntl.c is very incomplete and quite large for what it does. After the C implementation is finished I might reconsider 
rerwriting in assembly to optimize for size. It might be fine for writing/porting UNIX small programs but that's only
for expediency's sake. For most programs wanting to handle larger amounts of data using the CP/M BDOS will be faster
at the cost of losing the stdio API.

I am planning a malloc() but its arguable how useful it is other than (again) ensuring compatibility with small UNIX
programs. 

cpm0.s grabs the data from the CP/M command tail and packs it into argv which is located at 0x0050. This works on CPM2
but may collide with something on later/multiuser versions. 


Originally based on sdcc-cpm-example - see below. 


sdcc-cpm-example
================

HELLO WORLD / experimentation program running under CP/M-80, crosscompiled with SDCC (Small Devices C Compiler).
It's based on sources from http://n8vem-sbc.pbworks.com/ (maily found in the unified-bios package).

This comes without any warranty: It's mostly a experimentation code for my N8VEM SBC V2.

http://www.sarnau.info/posts/1992/z80_disassembler_assembler/

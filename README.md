## MBuddy  

### What is mbuddy? 

mbuddy is a simple two-way CLI serial monitor based on the ncurses library. 
It allows you to connect to any serial device you have access rights to for 
both receiving and sending data. When searching for a good command line based 
serial monitor, I found that many of them were lacking the ability to send data 
across the port and instead were only capable of reading.

Currently mbuddy only works on linux systems, but in the future I'd like to port
it to Windows as well. 

Aside for core system dependencies present on most distributions, mbuddy also 
has a dependency on the ncurses library. This can be installed in the 
distribution's package manager and is called `libncursesw6`

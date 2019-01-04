# Firmware_for_Core_Board

 ZJUNlict Core Board's Firmware for the Robocup Soccer Small-Size League https://zjunlict.cn/

The firmware for [mother board](https://github.com/ZJUNlict/Mother_Board)'s corresponding [core board](https://github.com/ZJUNlict/Core_Board) is designed using Altera (now Intel) [Quartus II 9.1](https://www.intel.com/content/www/us/en/programmable/downloads/software/quartus-ii-se/91.html) and [Nios II 9.1 IDE](https://www.intel.com/content/altera-www/global/en_us/index/downloads/software/nios-ii/91.html). 

The main features are:
* * [Nios® II embedded processor](https://www.intel.com/content/www/us/en/products/programmable/processor/nios-ii.html) (set the running frequency at 100MHz) is built in the Cyclone III FPGA through Quartus II software. 
* Nios® II embedded firmware is designed in C language handling hardware configuration, radio communicaton, resolution of velocity and other functions. 
* The C project for the embedded processor is in software\SmallSizeV14_sim. 
* Supports ZJUNlict's latest [communication protocol](https://github.com/ZJUNlict/Wireless_Communication_Protocol). 

Other information for this firmware can be found in our 2013 and 2014 [champion papers](https://zjunlict.cn/?page_id=54). 
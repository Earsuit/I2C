# I2C
I2C.cpp - a free library for I2C communication of Arduino Nano or Mega. I haven't tested it on other boards but any one uses ATmega48A/PA/88A/PA/168A/PA/328/P or Atmel ATmega640/V-1280/V-1281/V-2560/V-2561/V chip should work. Please note that this library needs the 8-bit Timer/Counter0.

Please refer to https://longnight975551865.wordpress.com/2018/02/11/write-your-own-i%c2%b2c-library/ for more information.

Feedback and contribution is welcome!

Version 0.2
* added timeout function which ensures the program won't hang up. Please note
  the timeout() function uses the 8-bit Timer/Counter0.
* added a new requestFrom() that overloaded the previous one. The new requestFrom() takes a user-defined array and its index and stores data in the specified location of this array.

-------------------------------------
Created by Yudi Ren, Feb 1, 2018.
renyudicn@outlook.com
Version 0.1


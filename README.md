# I2C
I2C.cpp - A free library for I2C communication of Arduino Nano or Mega. I haven't tested it on other boards but any one uses ATmega48A/PA/88A/PA/168A/PA/328/P or Atmel ATmega640/V-1280/V-1281/V-2560/V-2561/V chip should work. Please note that this library needs the 8-bit Timer/Counter0. Please note that the delay() may block this library.

Please refer to https://longnight975551865.wordpress.com/2018/02/11/write-your-own-i%c2%b2c-library/ for more information.

Feedback and contribution is welcome!

Version 1.2
* Fix some bugs in timeout.

Version 1.1
* added timeout function which ensures the program won't hang up. Please note
  the timeout() function uses the 8-bit Timer/Counter0.
* added a new requestFrom() that overloaded the previous one. The new requestFrom() takes a user-defined array and its index and stores data in the specified location of this array.

Version 1.0
* Basic functions of I2C bus, similar to the official "wire" library.

-------------------------------------
Created by Yudi Ren, Feb 1, 2018.

renyudicn@outlook.com

Version 1.0
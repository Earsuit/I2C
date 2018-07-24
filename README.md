# I2C
I2C.cpp - A free library for I2C communication of Arduino Nano or Mega. Please note that this library needs the 8-bit Timer/Counter0. It is not recommended to put these functions in an interrupt routing. And sometimes the delay() blocks this library.

Please refer to https://longnight975551865.wordpress.com/2018/02/11/write-your-own-i%c2%b2c-library/ for more information.

Feedback and contribution is welcome!

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
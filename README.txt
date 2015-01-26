BrundleFab printhead controller
-------------------------------

Drives a 96DPI HP C6602A printhead, with a travel of 8.75"
Each dotline drives 12 nozzles

Pinout:
-------

  0     RX
  1     TX
  2     --
  3     Encoder A 
  4     DIR_CLK
  5     Encoder B
  6     ENDSTOP_MIN
  7     --   
  8     DIR_SET
  9     PWM2A
  10    HEATER_EN
  11    DIR_EN          /* Moved from D7 */
  12    DIR_LATCH
  13    INK_PULSE

  A0    INK_A
  A1    INK_B
  A2    INK_C
  A3    INK_D

  A4    HEATER_TEMP     (thermistor)
  A5    Ink Tank sensor (TODO)

Serial commands:
----------------

Status report:

=> "ok SS I NNN TT"

  SS:
     Bit 0 - Motor on
     Bit 1 - Motor stalled
     Bit 2 - Motor min endstop
     Bit 3 - Motor max endstop
     Bit 4 - Heater on
     Bit 5 - Heater at temp
     Bit 6 - Ink spraying
     Bit 7 - Ink tank empty (todo, always 0)

  I:
     Ink sprays per position

  NNN:
     Entries left in dotline buffer

  TT:
     Temp of the heater in degress C/2

Commands:

  Up to 4 characters, terminated by a space characters (ie '\r', '\n', etc)

  "lXXX"
      Add to dot line buffer, 0xbb is free dotlines in buffer
      (a dotline is 1/96th of an inch, or 0.2646 mm)

  "rXXX"
      Repeat last dot line NNN+1 times 

  "?"
      No-op - return status
      
  "h"
      Reset dot line buffer, move to start

  "i"
      Start inking

  "sX"
      Set number of sprays/dotline - 1

  "tXX"
      Heater temp, in degrees C / 2

# vim: set shiftwidth=4 expandtab: 

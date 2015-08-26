BrundleFab printhead controller
-------------------------------

Drives a 96DPI HP C6602A printhead, with a travel of 8.75"
Each dotline drives 12 nozzles

Pinout:
-------

  0     RX
  1     TX
  2     Encoder A
  3     Encoder B
  4     --
  5     INK_PULSE
  6     --
  7     --
  8     MOTOR_B_BRAKE
  9     MOTOR_A_BRAKE
  10    MOTOR_A_PWM
  11    MOTOR_B_PWM
  12    MOTOR_A_DIR
  13    MOTOR_B_DIR

  A0    INK_A
  A1    INK_B
  A2    INK_C
  A3    INK_D

  A4    HEATER_TEMP     (thermistor)
  A5    Ink Tank sensor (TODO)

Serial commands:
----------------

Command is a newline termininated string, of the form:

  Cnnn

  'C' is the command, and 'nnn' is the parameter.

Status report response to a command:

=> "ok SS I NNN LL MMM\n" - ? command

=> "ok\n" - all other commands

==> "!!\n" - error

  SS:
     Bit 0 - Motor on
     Bit 1 - Motor stalled
     Bit 2 - Motor min endstop
     Bit 3 - Motor max endstop
     Bit 4 - Heater on
     Bit 5 - Heater at or above temp
     Bit 6 - Ink spraying
     Bit 7 - Ink tank empty (todo, always 0)

  I:
     Ink sprays per dotline

  NNN:
     Entries left in dotline buffer

  LL:
     Line number (incremented after this command)

  MMM:
     Dotline position

Commands:

  Up to 4 characters, terminated by a space characters (ie '\r', '\n', etc)

  "lXXX"
      Add 12-bit pattern to dotline buffer
      (a dotline is 1/96th of an inch, or 0.2646 mm)

  "rXXX"
      Repeat last dot line NNN+1 times

  "?"
      No-op - return status

  "h"
      Reset dot line buffer, move to start

  "i"
      Ink - Start inking from here till end of dotline

  "j"
      Jump-back - Start inking from here till start of dotline

  "k"
      Kill - Cancel movement, clear dotline buffer

  "nXXX"
      Number - set current 12-bit line number (wrapping) to X

  "sX"
      Set number of sprays/dotline - 1


# vim: set shiftwidth=4 expandtab: 

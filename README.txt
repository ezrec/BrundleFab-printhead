BrundleFab printhead controller

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
  10
  11    DIR_EN          /* Moved from D7 */
  12    DIR_LATCH
  13    INK_PULSE

  A0    INK_A
  A1    INK_B
  A2    INK_C
  A3    INK_D

  A4    SDA
  A5    SCL

Serial commands:
----------------

  0x8F 0xFF 0x8F 0xFF 0x00 => 0xXX 0xXX 0xFF 0xBF 0x01
      Sync code

  0x80 => 0xXX
      Reset dot line buffer, move to start

  0x81 => 0xXX
      Start inking

  0x82 0xNN 0xNN => 0xXX 0x82 0xNN
      Set number of ink pulses/position

  0x0m 0xmm => 0xXX 0xbb
      Add to dot line buffer, 0xbb is free dotlines in buffer

  0x4n 0xnn => 0xXX 0xbb
      Repeat last dot line NNN+1 times 

# vim: set shiftwidth=4 expandtab: 

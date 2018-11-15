# Encoder

Lees een rotary encoder uit, display dit op een SSD1306 display en verstuur de positie m.b.v. een websocket.

## SSD1306 display

Aansluitingen:
* SDA -> D1
* SCL -> D2
* GND -> GND
* VCC -> 3.3v

M.b.v. een (i2c scanner)[https://github.com/wemos/D1_mini_Examples/blob/master/examples/02.Special/Wire/I2C_Scanner/I2C_Scanner.ino] gekeken welk I2C adres dit display gebruikt:

```
Scanning...
I2C device found at address 0x3C !
Done.
```

Ofwel `0x3C`

Eerst een *hello world* tonen. 



## Encoder



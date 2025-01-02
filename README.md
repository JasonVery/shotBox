# shotBox
Cool beverage dispenser using an esp32

Current wiring setup so I can refer to it when I solder:

Buttons
---------------------------------
Button 0: pin 15 
Button 1: pin 2
Button 2: pin 0
Button 3: pin 4

Start button: pin 5

*All buttons to common ground (second from bottom with star)


Motor
---------------------------------
Voltage pin on controller to 5v pin. (Bottom One)
Ground pin on contoller to own ground. (above the button grounds)


L293D
---------------------------------
pin 1 -> pin 21 on esp32
pin 2 -> pin 22 on esp32
pin 3 -> positive terminal on pump
pin 4 -> ground (one above the 13 pin * we also connect the negative on battery to this)
pin 6 -> negative terminal on pump
pin 8 -> positive on battery
pin 16 -> 5v on esp32 (currently same pin as motor controller)


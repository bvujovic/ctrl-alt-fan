# Ctrl+Alt+Fan

Periodic fan. Computer fan is turned on/off in regular intervals.

## Test device: ESP32-C3, breadboard, fan
![Test device: ESP32-C3, breadboard, fan](<docs/test device 2.jpg>)

## TODO
- [ ] Improve interface:
    - [x] Make clicks for pause intervals and fan speed as **consistent** as possible (click, dbl click, long click)
    - [ ] Small display (<ins>ESP32-C3 with built-in display</ins> or separate display) or LED diodes
- [ ] Maybe settings for speed and pause should be saved/loaded automatically. Or maybe the fan should start at full speed and no pause. Check if the peek current can be reduced by gradual acceleration.
- [ ] Make final design for the device:
    - [ ] How to turn off device: <ins>switch on separate USB C connector</ins> or make ESP go to deep sleep

### Clicks

|Button          |Click         |DblClick   |LongClick|
|--|--|--|--|
|interval/pause  |inc pause     |dec pause  |no pause (fan continuously works)|
|pwm/speed       |inc speed     |dec speed  |max speed

- If speed or pause is max/min - inc/dec doesn't do anything.
- LongClick for pwm/speed: if it's not max speed -> max speed, if it's at max speed - min speed
- Display will turn off automatically. If display is off, first click turns it on. Maybe ESP should go to sleep and should start if some button is pressed.

### Current consumption (5V)

- ESP32 C3 mini: 18mA
- ESP32 C3 mini with Noctua NF-P12 redux-1700 PWM at 100%: 0.27, 0.35A peak
- Old 12cm fan: 0.55A regular, 0.8A peak
- AVC DS09225R12HP049: 0.65A regular
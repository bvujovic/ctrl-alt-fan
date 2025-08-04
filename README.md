# Ctrl+Alt+Fan

Periodic fan. Computer fan is turned on/off in regular intervals.

## Test device: ESP32-C3, breadboard, fan
![Test device: ESP32-C3, breadboard, fan](<docs/test device 2.jpg>)

## TODO
- [ ] Improve interface:
    - [ ] Make clicks for pause intervals and fan speed as **consistent** as possible (click, dbl click, long click)
    - [ ] Decide if there should be 4 or 5 PWM options (fan speed). Check how silent/loud fan is on 40/50/60 % of duty cycle - is it disturbing when I rest or sleep.
    - [ ] Small display (<ins>ESP32-C3 with built-in display</ins> or separate display) or LED diodes
- [ ] Make final design for the device:
    - [x] Glue a wire to the fan frame against tipping over
    - [ ] How to turn off device: <ins>switch on separate USB C connector</ins> or make ESP go to deep sleep

### Clicks
Proposed clicks if display is used insted of LED diodes

|Button          |Click         |DblClick   |LongClick|
|--|--|--|--|
|interval/pause  |inc pause     |dec pause  |no pause (fan continuously works)|
|pwm/speed       |inc speed     |dec speed  |max speed

- If speed/pause is max/min - inc/dec speed/pause doesn't do anything.
- Display will turn off automatically. If display is off, first click turns it on. Maybe ESP should go to sleep and should start if some button is pressed.
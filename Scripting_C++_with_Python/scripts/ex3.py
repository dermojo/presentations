import example
import time
import random

def toggle():
    on = False
    for i in range(10):
       for i in range(4):
          example.setIconState(i, on)
          time.sleep(0.2)
       on = not on

def setAll(on):
    if on:
        what = "on"
    else:
        what = "off"
    print("Setting all to %s" % what)
    for i in range(4):
        example.setIconState(i, on)

def setRandom():
    icons = (0, 1, 2, 3)
    states = (True, False)
    for i in range(50):
        icon = random.choice(icons)
        state = random.choice(states)
        example.setIconState(icon, state)
        time.sleep(0.1)

def mycb(num):
    print("Callback: num=%r" % num)
    if num == 0:
        toggle()
    elif num == 1:
        setAll(True)
    elif num == 2:
        setAll(False)
    else:
        setRandom()

example.clearCallbacks()
example.addCallback(mycb)


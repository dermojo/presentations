import example
import time
on = False
for i in range(5):
   for i in range(4):
      print(example.setIconState(i, on))
      time.sleep(0.2)
   time.sleep(1)
   on = not on

# uP2_Lab4

This is microprocessor applications lab 4 (second implementation of an RTOS)

## Important Notes

- Part A:
  - Ensure that there will be no deadlocks and no situation where all threads are asleep
- Stuff that works:
  - Sleep function and systick handler unsleep function work
  - PWM works
  - UART and I2C work
  - Idle Thread
  - Same priorities, diff priorities, blocking, yielding, and sleeping work
- Stuff I used for Testing:
  - Changed LaunchpadLED.c file so that I could only see the red LED (well just make sure not to mess with blue)
- Stuff to Work On:
  - Finish up FIFO consumer stuff
  - Figure out how to connect producer and consumer (wait they're internally connected)

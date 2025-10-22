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
  - FIFO Works by itself, UARTing other stuff breaks the code
- Stuff I used for Testing:
  - Changed LaunchpadLED.c file so that I could only see the red LED (well just make sure not to mess with blue)
- Stuff to Work On:
  - Figure out how to not brick the code when having multiple threads UART at once
    - Fixed the priority of the FIFOs to make them lower than the other threads
  - Figure out how to fix the timing for the consumer thread
  - Fix weird bug with the code where the value decrements by 15 for no reason (drops because the head cycles) -- not a bug

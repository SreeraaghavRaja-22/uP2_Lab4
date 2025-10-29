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
      - It works even if the producer has the same sleep time
    - Fix weird bug with the code where the value decrements by 15 for no reason drops because the head cycles?
      - This is not a bug because the head pointer cycles -- could still be a bug
  
- Part C:
  - I accounted for alive and dead threads
  - I accounted for the linked list change in the scheduler (for killself function)
  - Aperiodic threads only require you to use intRegister as that moves the Vector table from Flash to SRAM
  - Switches are affected by the TFT display, so when testing the switches, turn off everything related to the display
  - Make sure to set the SW semaphore's value to 0 so that it actually gets blocked until we trigger it
  - Now time for child threads

- Part D:
  - Things to Do:
    - Finish up the Joystick Functions and learn more about the ADC
    - Finish part E
    - Debounce multimode and joystick switches
  
- Disabled Periodic threads in SysTick to test code
  - when uncommenting out that portion, make sure to add a periodic thread to no mess stuff up!
  
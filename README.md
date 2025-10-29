# uP2_Lab4

This is microprocessor applications lab 4 (second implementation of an RTOS)

## Important Notes

- Part A:
  - Ensure that there will be no deadlocks and no situation where all threads are asleep
  - Stuff I used for Testing:
    - Changed LaunchpadLED.c file so that I could only see the red LED (well just make sure not to mess with blue)
  
- Part B:
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
  - Switches are affected by the TFT display, so when testing the switches, turn off everything related to the display (getting this to work early on was a blessing in disguise)
  - Make sure to set the SW semaphore's value to 0 so that it actually gets blocked until we trigger it
  - Now time for child threads

- Part D:
  - Test Joystick
  - Debounce multimod and joystick switches
    - Joystick Switch Debounced!!!
    - Multimod is finally done
    - Aperiodic threads must have the highest priority
  
- Part E:
  - use time.h for the header file for randomness
  - Things to do:
    - Understand Cube Thread
    - Write CamMove Thread
    - Write ReadButtons
    - Write Read_JoystickPress
    - Write Print_WorldCoords() (Period: 100ms)
    - Write Get_Joystick(Period: 100ms)
    - Write GPIOD_Handler()
  
- Important Notes:
  - Disabled Periodic threads in SysTick to test code
  - when uncommenting out that portion, make sure to add a periodic thread to not mess stuff up!
  
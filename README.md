# uP2_Lab4

This is microprocessor applications lab 4 (second implementation of an RTOS)

## Important Notes

- Part A:
  - Understand how to immediately switch if a thread is blocked?
  - Where do I decrement the thread's sleepCount value?
    - the systick handler or inside the sleep function?
  - Ensure that there will be no deadlocks and no situation where all threads are asleep
  - How do I assign priority?
    - Is it randomized?
    - Is there a specific way to assign priority?

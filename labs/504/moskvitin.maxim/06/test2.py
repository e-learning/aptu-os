from subprocess import call
import time


for i in range(2, 10):
  limit = "1" + "0" * i
  start_time = time.time()
  call(["./sieve_unith", limit])
  print("UNI;  " + limit + ";" + " " * (11 - i) + str((time.time() - start_time) * 1000))     
  start_time = time.time()     
  call(["./sieve_multith", limit, "1"])
  print("MULT; " + limit + ";" + " " * (11 - i) + str((time.time() - start_time) * 1000)) 

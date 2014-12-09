from subprocess import call
import time

for j in range(1, 21):
  for i in range(2, 10):
    limit = "1" + "0" * i
    start_time = time.time()
    if j == 1:
      call(["./sieve_unith", limit])
    else:
      call(["./sieve_multith", limit, str(j)])
    print(str(j) + ";\t " + limit + ";" + " " * (11 - i) + str((time.time() - start_time) * 1000)) 

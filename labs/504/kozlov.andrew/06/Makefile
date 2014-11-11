all: sieve_unith sieve_multith

sieve_unith:
	javac LongBooleanList.java AbstractSieve.java SieveUniThread.java
	@chmod +x sieve_unith

sieve_multith:
	javac LongBooleanList.java AbstractSieve.java SieveMultiThread.java
	@chmod +x sieve_multith

task1:
	javac *.java
	@chmod +x task1

task2:
	javac *.java
	@chmod +x task2

find_optima:
	javac *.java
	@chmod +x find_optima

clean:
	rm -f *.class

.PHONY: sieve_unith sieve_multith task1 task2 find_optima clean

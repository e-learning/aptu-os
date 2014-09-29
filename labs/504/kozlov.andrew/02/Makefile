all: scheduler

Scheduler.class: Scheduler.java
	javac Scheduler.java

scheduler: Scheduler.class
	touch scheduler
	chmod +x scheduler
	echo "#!/bin/bash" >> scheduler
	echo "java Scheduler" >> scheduler

clean:
	rm -f *.class scheduler

.PHONY: clean

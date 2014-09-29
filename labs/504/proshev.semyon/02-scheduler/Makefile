main: main.class scheduler.class task.class taskParser.class
	jar cfe scheduler.jar Main *.class
	rm -f *.class
	echo "#!/bin/bash" > scheduler
	echo 'java -jar $${BASH_SOURCE[0]}.jar' >> scheduler
	chmod a+x scheduler

main.class: Main.java
	javac Main.java

scheduler.class: Scheduler.java
	javac Scheduler.java

task.class: Task.java
	javac Task.java

taskParser.class: TaskParser.java
	javac TaskParser.java

clean:
	rm -f scheduler.jar scheduler

.PHONY: clean
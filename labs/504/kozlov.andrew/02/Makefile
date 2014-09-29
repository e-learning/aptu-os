java_name=Scheduler

all: scheduler

$(java_name).class: $(java_name).java
	javac $(java_name).java

scheduler: $(java_name).class
	chmod +x scheduler

clean:
	rm -f *.class

.PHONY: clean

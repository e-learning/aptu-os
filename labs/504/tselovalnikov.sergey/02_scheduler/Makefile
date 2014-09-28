all:
	mkdir -p out
	javac src/ru/serce/*.java -d out/
	cd out/ && jar cfe scheduler.jar ru.serce.Sheduler  ru/serce/*.class && mv scheduler.jar ../ && cd ..	
	echo "#!/bin/bash" > scheduler
	echo "java -jar scheduler.jar" >> scheduler

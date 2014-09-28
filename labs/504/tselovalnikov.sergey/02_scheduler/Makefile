all:
	mkdir -p out
	javac src/ru/serce/*.java -d out/
	cd out/ && jar cfe scheduler.jar ru.serce.Sheduler  ru/serce/*.class && mv scheduler.jar ../ && cd ..	

run:
	java -jar scheduler.jar

all:
	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe init.jar pavljuchenko.denis.init  pavljuchenko/denis/*.class && mv init.jar ../ && cd ..	
	echo "#!/bin/bash" > init
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1' >> init
	chmod a+x init

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe format.jar pavljuchenko.denis.format  pavljuchenko/denis/*.class && mv format.jar ../ && cd ..	
	echo "#!/bin/bash" > format
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1' >> format
	chmod a+x format

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe import.jar pavljuchenko.denis.importClass  pavljuchenko/denis/*.class && mv import.jar ../ && cd ..	
	echo "#!/bin/bash" > import
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1 $$2 $$3' >> import
	chmod a+x import

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe export.jar pavljuchenko.denis.export  pavljuchenko/denis/*.class && mv export.jar ../ && cd ..	
	echo "#!/bin/bash" > export
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1 $$2 $$3' >> export
	chmod a+x export

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe ls.jar pavljuchenko.denis.ls  pavljuchenko/denis/*.class && mv ls.jar ../ && cd ..	
	echo "#!/bin/bash" > ls
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1 $$2' >> ls
	chmod a+x ls

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe copy.jar pavljuchenko.denis.copy  pavljuchenko/denis/*.class && mv copy.jar ../ && cd ..	
	echo "#!/bin/bash" > copy
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1 $$2 $$3' >> copy
	chmod a+x copy

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe move.jar pavljuchenko.denis.move  pavljuchenko/denis/*.class && mv move.jar ../ && cd ..	
	echo "#!/bin/bash" > move
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1 $$2 $$3' >> move
	chmod a+x move

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe rm.jar pavljuchenko.denis.rm  pavljuchenko/denis/*.class && mv rm.jar ../ && cd ..	
	echo "#!/bin/bash" > rm
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1 $$2' >> rm
	chmod a+x rm

	mkdir -p out
	javac src/pavljuchenko/denis/*.java -d out/
	cd out/ && jar cfe mkdir.jar pavljuchenko.denis.mkdir  pavljuchenko/denis/*.class && mv mkdir.jar ../ && cd ..	
	echo "#!/bin/bash" > mkdir
	echo 'java -jar $${BASH_SOURCE[0]}.jar $$1 $$2' >> mkdir
	chmod a+x mkdir
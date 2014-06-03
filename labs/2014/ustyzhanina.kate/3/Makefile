default:  clean init format export import ls copy move rm mkdir 
	
directory.o : directory.cpp
	g++ -c -pedantic -std=c++11 directory.cpp  
fs.o: fs.cpp
	g++ -c -pedantic -std=c++11 fs.cpp

init.o: init.cpp 
	g++ -c -pedantic -std=c++11  init.cpp
init: init.o directory.o fs.o
	g++ init.o directory.o fs.o -o init

format.o: format.cpp 
	g++ -c -pedantic -std=c++11  format.cpp
format: format.o directory.o fs.o
	g++ format.o directory.o fs.o -o format 

import.o: import.cpp 
	g++ -c -pedantic -std=c++11  import.cpp
import: import.o directory.o fs.o
	g++ import.o directory.o fs.o -o import 

export.o: export.cpp 
	g++ -c -pedantic -std=c++11  export.cpp
export: export.o directory.o fs.o
	g++ export.o directory.o fs.o -o export 

ls.o: ls.cpp 
	g++ -c -pedantic -std=c++11  ls.cpp
ls: ls.o directory.o fs.o
	g++ ls.o directory.o fs.o -o ls 

copy.o: copy.cpp 
	g++ -c -pedantic -std=c++11  copy.cpp
copy: copy.o directory.o fs.o
	g++ copy.o directory.o fs.o -o copy 

move.o: move.cpp 
	g++ -c -pedantic -std=c++11  move.cpp
move: move.o directory.o fs.o
	g++ move.o directory.o fs.o -o move 

rm.o: rm.cpp 
	g++ -c -pedantic -std=c++11  rm.cpp
rm: rm.o directory.o fs.o
	g++ rm.o directory.o fs.o -o rm 

mkdir.o: mkdir.cpp 
	g++ -c -pedantic -std=c++11  mkdir.cpp
mkdir: mkdir.o directory.o fs.o
	g++ mkdir.o directory.o fs.o -o mkdir 


clean:
	rm -rf init format export import ls copy move rm mkdir  *.o 

	


run: all



ls:main_ls.o fs_utils.o
	g++ -o ls main_ls.o fs_utils.o

main_ls.o:main_ls.cpp
	g++ -o main_ls.o -c main_ls.cpp


	
	
mkdir:main_mkdir.o fs_utils.o
	g++ -o mkdir main_mkdir.o fs_utils.o

main_mkdir.o:main_mkdir.cpp
	g++ -o main_mkdir.o -c main_mkdir.cpp




export:main_export.o fs_utils.o
	g++ -o export main_export.o fs_utils.o

main_export.o: main_export.cpp
	g++ -o main_export.o -c main_export.cpp




import:main_import.o fs_utils.o
	g++ -o import main_import.o fs_utils.o

main_import.o:main_import.cpp
	g++ -o main_import.o -c main_import.cpp





format: main_format.o fs_utils.o
	g++ -o format main_format.o fs_utils.o

main_format.o: main_format.cpp
	g++ -o main_format.o -c main_format.cpp




init: main_init.o fs_utils.o
	g++ -o init main_init.o fs_utils.o

main_init.o: main_init.cpp
	g++ -o main_init.o -c main_init.cpp

fs_utils.o: fs_utils.cpp
	g++ -o fs_utils.o -c fs_utils.cpp

all:init format import export mkdir ls


clear: *.o
	rm *.o

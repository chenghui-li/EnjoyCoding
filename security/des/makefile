run:des.o main.o
	g++ -o run -g des.o main.o

des.o:des.cpp des.h table.h
	g++ -c -g des.cpp

main.o:main.cpp des.h table.h
	g++ -c  -g main.cpp

clean:
	rm run main.o des.o

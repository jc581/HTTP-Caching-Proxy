all: parseTest

parseTest: test.cpp parseRes.cpp 
	g++ -o test test.cpp -Werror -Wall -pedantic -ggdb3 --std=gnu++11

clean:
	rm *.~ *~ *# test

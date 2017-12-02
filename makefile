tsp: tsp.cpp
	g++ -g tsp.cpp -o tsp -Wall -Werror -std=c++11 -O3 

clean:
	rm -f tsp tsp.o tsp.exe

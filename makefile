FLAGS = -Wall -Werror -std=c++11 -g

tsp: tsp.cpp
	g++ tsp.cpp -o tsp $(CFLAGS)

clean:
	rm -f tsp tsp.o tsp.exe

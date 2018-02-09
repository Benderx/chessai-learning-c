CC = g++
CFLAGS = -std=c++14 -Wall -Wno-unused-variable -Wno-unused-value
HEADERS = bitboard.hpp
OBJECTS = driver.o bitboard.o


default: driver

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

driver: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o driver

clean:
	-rm -f $(OBJECTS)
	-rm -f driver

CC = g++
CFLAGS = -std=c++14 -Ofast -flto -funroll-loops -march=native -Wall -Wno-unused-variable -Wno-unused-value -Wno-comment -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-delete-non-virtual-dtor -g
# CFLAGS = -std=c++14 -Wall -Wno-unused-variable -Wno-unused-value -Wno-comment -Wno-unused-but-set-variable -Wno-maybe-uninitialized
HEADERS = bitboard.hpp player.hpp
DRIVER_OBJECTS = bitboard.cpp driver.cpp move_generation.cpp piece_logic.cpp
PLAY_OBJECTS = bitboard.cpp player.cpp play.cpp move_generation.cpp piece_logic.cpp


all: driver play

driver: $(DRIVER_OBJECTS)
	$(CC) $(CFLAGS) bitboard.hpp $(DRIVER_OBJECTS) -o driver

play: $(PLAY_OBJECTS)
	$(CC) $(CFLAGS) bitboard.hpp player.hpp $(PLAY_OBJECTS) -o play

clean:
	-rm -f driver
	-rm -f play
	-rm -f callgrind.out.*
	-rm -f out.log



# CC = g++
# CFLAGS = -std=c++14 -Wall -Wno-unused-variable -Wno-unused-value -Wno-comment
# HEADERS = bitboard.hpp player.hpp
# PLAY_OBJECTS = driver.o bitboard.o
# DRIVER_OBJECTS = bitboard.o player.o play.o


# all: driver play

# %.o: %.cpp $(HEADERS)
# 	$(CC) $(CFLAGS) -c $< -o $@

# driver: $(DRIVER_OBJECTS)
# 	$(CC) $(CFLAGS) $(DRIVER_OBJECTS) -o driver

# play: $(PLAY_OBJECTS)
# 	$(CC) $(CFLAGS) $(PLAY_OBJECTS) -o play

# clean:
# 	-rm -f $(PLAY_OBJECTS)
# 	-rm -f $(DRIVER_OBJECTS)
# 	-rm -f driver
# 	-rm -f play
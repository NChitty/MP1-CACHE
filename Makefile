CC = g++
OPT = -O3
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

SIM_SRC = main.cpp sets/Set.cpp sets/LRUSet.cpp Cache.cpp

SIM_OBJ = main.o Set.o LRUSet.o Cache.o

all: sim_cache
		@echo "Finished compiling cache simulator"

sim_cache: $(SIM_OBJ)
			$(CC) -o sim_cache $(CFLAGS) $(SIM_OBJ) -lm
			@echo "------------------DONE WITH SIM_CACHE-----------------"

.cpp.o:
	$(CC) $(CLFAGS) -c $*.cpp

clean:
	rm -f *.o sim_cache

clobber:
	rm -f *.o
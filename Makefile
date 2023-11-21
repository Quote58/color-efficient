CC = g++
CFLAGS = `wx-config --cxxflags` -Wno-c++11-extensions -std=c++11
CLIBS = `wx-config --libs` -Wno-c++11-extensions -std=c++11
OBJ = themer.o palette.o preview.o

themer: $(OBJ)
	$(CC) -o themer $(OBJ) $(CLIBS)

themer.o: themer.cpp themer.h
	$(CC) -c themer.cpp $(CFLAGS)

palette.o: palette.cpp palette.h
	$(CC) -c palette.cpp $(CFLAGS)

preview.o: preview.cpp preview.h
	$(CC) -c preview.cpp $(CFLAGS)

.PHONY: clean
clean:
	-rm themer $(OBJ)
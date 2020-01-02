CCC = g++
CFLAGS = -Wall -std=c++11

TARGET = graph
MAIN = main

$(MAIN): $(MAIN).o $(TARGET).o
	$(CCC) $(CFLAGS) -o $(MAIN) $(MAIN).o $(TARGET).o

${MAIN}.o: src/$(MAIN).cpp src/$(TARGET).h
	$(CCC) $(CFLAGS)  -c src/$(MAIN).cpp


${TARGET}.o: src/$(TARGET).cpp src/$(TARGET).h
	$(CCC) $(CFLAGS) $(INC) -c src/$(TARGET).cpp

clean:
	rm --force *.o $(MAIN)
### VARIABLES #############
C = g++ -g -Wall
L = g++ -g
RM = /bin/rm -f
CD = cd
INC = 
LIBS = -lstdc++ -lpthread -lrt -lzmq

.PHONY: clean
clean:
	@$(RM) robotd
	@cd ./build && $(RM) *.o */*.o */*.so */*/*.o */*/*.so 

.PHONY: stats
stats:
	@wc -l -w -c -L */*.*pp */*/*.*pp 
	@echo " Lines\tWords\tBytes\tLine\t"

### MAIN BUILD TARGET #############

robotd: source/main.cpp build/Serial.o build/Serializer.o build/Command.o
	@echo "[compile] main"
	@$(CD) ./build && $(C) $(INC) -c ../source/main.cpp
	@$(L) build/*.o $(LIBS) -o robotd
	@chmod +x robotd

build/Serial.o: source/Serial.hpp source/Serial.cpp
	@echo "[compile serial]"
	@$(CD) ./build && $(C) $(INC) -c ../source/Serial.cpp

build/Serializer.o: source/Serializer.hpp source/Serializer.cpp
	@echo "[compile serializer]"
	@$(CD) ./build && $(C) $(INC) -c ../source/Serializer.cpp

build/Command.o: source/Command.hpp source/Command.cpp
	@echo "[compile command]"
	@$(CD) ./build && $(C) $(INC) -c ../source/Command.cpp



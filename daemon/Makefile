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

robotd: source/main.cpp
	@echo "[compile] main"
	@$(CD) ./build && $(C) $(INC) -c ../source/main.cpp
	@$(L) build/main.o $(LIBS) -o robotd
	@chmod +x robotd


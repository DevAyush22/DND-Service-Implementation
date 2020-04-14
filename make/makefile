INC = ../inc
SRC = ../src
BIN = ../bin
OBJ = ../obj
CFLAGS = -c -g -Wall
LFLAGS = -o
GFLAGS = -fprofile-arcs -ftest-coverage
GCC = g++
IFLAGS = -I $(INC)
SVFLAGS = -v --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valserver 
CVFLAGS = -v --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valclient


all : $(BIN)/client $(BIN)/server

$(OBJ)/clientmain.o : $(SRC)/clientmain.cpp $(INC)/client.h $(INC)/logger.h
	$(GCC) $(GFLAGS) $(CFLAGS) $(SRC)/clientmain.cpp $(IFLAGS)        
	mv *.o ../obj

$(OBJ)/client.o : $(SRC)/client.cpp $(INC)/client.h $(INC)/logger.h
	$(GCC) $(GFLAGS) $(CFLAGS) $(SRC)/client.cpp $(IFLAGS)
	mv *.o ../obj

$(OBJ)/servermain.o : $(SRC)/servermain.cpp $(INC)/server.h $(INC)/logger.h
	$(GCC) $(GFLAGS) $(CFLAGS) $(SRC)/servermain.cpp $(IFLAGS)
	mv *.o ../obj

$(OBJ)/server.o : $(SRC)/server.cpp $(INC)/server.h $(INC)/logger.h
	$(GCC) $(GFLAGS) $(CFLAGS) $(SRC)/server.cpp $(IFLAGS)
	mv *.o ../obj

$(OBJ)/logger.o : $(SRC)/logger.cpp $(INC)/logger.h
	$(GCC) $(GFLAGS)  $(CFLAGS) $(SRC)/logger.cpp $(IFLAGS)
	mv *.o ../obj

$(BIN)/client : $(OBJ)/client.o $(OBJ)/clientmain.o $(OBJ)/logger.o
	$(GCC) $(GFLAGS) $(OBJ)/client.o $(OBJ)/clientmain.o $(OBJ)/logger.o $(LFLAGS) $(BIN)/client

$(BIN)/server : $(OBJ)/server.o $(OBJ)/servermain.o $(OBJ)/logger.o
	$(GCC) $(GFLAGS) $(OBJ)/server.o $(OBJ)/servermain.o $(OBJ)/logger.o $(LFLAGS) $(BIN)/server
        
clean:
	 rm -f $(OBJ)/*.o $(BIN)/client server

valgrindserver:
	valgrind $(SVFLAGS) $(BIN)/server 
	mv valserver $(BIN)/

valgrindclient:
	valgrind $(CVFLAGS) $(BIN)/client 10.203.161.8 57322 9845303636
	mv valclient $(BIN)/
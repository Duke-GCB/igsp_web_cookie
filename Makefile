# needs occi library and headers under ORACLE_HOME
# needs openssl-devel
#
#LIBSTDC := /usr/lib64/libstdc++.so.6
#LIBSTDC := /usr/lib/libstdc++.so.5

SRC=src
BIN=bin
OBJ=obj

all : dirs $(BIN)/cookieDaemon $(BIN)/signCookie $(BIN)/verifyCookie $(BIN)/readconf

dirs :
	mkdir -p $(BIN)
	mkdir -p $(OBJ)

$(BIN)/cookieDaemon : $(OBJ)/IGSPnet_Cookie_Streamer.o $(OBJ)/OCCI_IGSPnet.o $(OBJ)/RSA_Sign_Verify.o $(SRC)/cookieDaemon.cpp $(OBJ)/CookieDaemonConfig.o
	g++ -O3 -I $(ORACLE_HOME)/sdk/include -L $(ORACLE_HOME) $(LIBSTDC) -locci -lclntsh -lcrypto -lnnz11 $(OBJ)/IGSPnet_Cookie_Streamer.o $(OBJ)/OCCI_IGSPnet.o $(OBJ)/RSA_Sign_Verify.o $(OBJ)/CookieDaemonConfig.o $(SRC)/cookieDaemon.cpp -o $(BIN)/cookieDaemon

$(BIN)/signCookie : $(OBJ)/IGSPnet_Cookie_Streamer.o $(OBJ)/OCCI_IGSPnet.o $(OBJ)/RSA_Sign_Verify.o $(SRC)/signCookie.cpp
	g++ -O3 -I $(ORACLE_HOME)/sdk/include -L $(ORACLE_HOME) $(LIBSTDC) -locci -lclntsh -lcrypto -lnnz11 $(OBJ)/IGSPnet_Cookie_Streamer.o $(OBJ)/OCCI_IGSPnet.o $(OBJ)/RSA_Sign_Verify.o $(OBJ)/CookieDaemonConfig.o $(SRC)/signCookie.cpp -o $(BIN)/signCookie

$(BIN)/verifyCookie : $(OBJ)/IGSPnet_Cookie_Streamer.o $(OBJ)/RSA_Sign_Verify.o $(SRC)/verifyCookie.cpp $(OBJ)/CookieDaemonConfig.o
	g++ -O3 -lcrypto $(OBJ)/IGSPnet_Cookie_Streamer.o $(OBJ)/RSA_Sign_Verify.o $(SRC)/verifyCookie.cpp $(OBJ)/CookieDaemonConfig.o -o $(BIN)/verifyCookie

$(OBJ)/IGSPnet_Cookie_Streamer.o : $(SRC)/IGSPnet_Cookie_Streamer.cpp $(SRC)/IGSPnet_Cookie_Streamer.h
	g++ -c -O3 $(SRC)/IGSPnet_Cookie_Streamer.cpp -o $(OBJ)/IGSPnet_Cookie_Streamer.o

$(OBJ)/RSA_Sign_Verify.o : $(SRC)/RSA_Sign_Verify.cpp $(SRC)/RSA_Sign_Verify.h
	g++ -c -O3 $(SRC)/RSA_Sign_Verify.cpp -o $(OBJ)/RSA_Sign_Verify.o

$(OBJ)/OCCI_IGSPnet.o : $(SRC)/OCCI_IGSPnet.cpp $(SRC)/OCCI_IGSPnet.h $(OBJ)/CookieDaemonConfig.o
	g++ -c -O3 -I $(ORACLE_HOME)/sdk/include $(SRC)/OCCI_IGSPnet.cpp $(OBJ)/CookieDaemonConfig.o -o $(OBJ)/OCCI_IGSPnet.o

$(BIN)/readconf: $(OBJ)/CookieDaemonConfig.o
	g++ $(SRC)/readconf.cpp $(OBJ)/CookieDaemonConfig.o -o $(BIN)/readconf

$(OBJ)/CookieDaemonConfig.o:
	g++ -c $(SRC)/CookieDaemonConfig.cpp -o $(OBJ)/CookieDaemonConfig.o

clean :
	-rm $(BIN)/* $(OBJ)/*

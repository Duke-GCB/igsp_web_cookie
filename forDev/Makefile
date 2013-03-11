# needs occi library and headers under ORACLE_HOME
# needs openssh-devel
#
#LIBSTDC := /usr/lib64/libstdc++.so.6
#LIBSTDC := /usr/lib/libstdc++.so.5

all : cookieDaemon signCookie verifyCookie

cookieDaemon : IGSPnet_Cookie_Streamer.o OCCI_IGSPnet.o RSA_Sign_Verify.o cookieDaemon.cpp IGSPnet_Cookie_Config.h
	g++ -O3 -I $(ORACLE_HOME)/sdk/include -L $(ORACLE_HOME) $(LIBSTDC) -locci -lclntsh -lcrypto IGSPnet_Cookie_Streamer.o OCCI_IGSPnet.o RSA_Sign_Verify.o cookieDaemon.cpp -o cookieDaemon

signCookie : IGSPnet_Cookie_Streamer.o OCCI_IGSPnet.o RSA_Sign_Verify.o signCookie.cpp
	g++ -O3 -I $(ORACLE_HOME)/sdk/include -L $(ORACLE_HOME) $(LIBSTDC) -locci -lclntsh -lcrypto IGSPnet_Cookie_Streamer.o OCCI_IGSPnet.o RSA_Sign_Verify.o signCookie.cpp -o signCookie

verifyCookie : IGSPnet_Cookie_Streamer.o RSA_Sign_Verify.o verifyCookie.cpp IGSPnet_Cookie_Config.h
	g++ -O3 -lcrypto IGSPnet_Cookie_Streamer.o RSA_Sign_Verify.o verifyCookie.cpp -o verifyCookie

IGSPnet_Cookie_Streamer.o : IGSPnet_Cookie_Streamer.cpp IGSPnet_Cookie_Streamer.h
	g++ -c -O3 IGSPnet_Cookie_Streamer.cpp
RSA_Sign_Verify.o : RSA_Sign_Verify.cpp RSA_Sign_Verify.h
	g++ -c -O3 RSA_Sign_Verify.cpp
OCCI_IGSPnet.o : OCCI_IGSPnet.cpp OCCI_IGSPnet.h IGSPnet_Cookie_Config.h
	g++ -c -O3 -I $(ORACLE_HOME)/sdk/include OCCI_IGSPnet.cpp

clean :
	-rm cookieDaemon signCookie verifyCookie *.o

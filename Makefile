TARGETS=proxyClient

all: $(TARGETS)
clean:
	rm -f $(TARGETS)
proxyClient: proxyClient.cpp proxyServerGET.cpp proxyServerCONNECT.cpp proxyServerPOST.cpp
	g++ -std=c++11 -g -o $@ $<

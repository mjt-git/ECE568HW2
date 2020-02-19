TARGETS=proxyClient

all: $(TARGETS)
clean:
	rm -f $(TARGETS)
proxyClient: proxyClient.cpp proxyServer.cpp
	g++ -std=c++11 -g -o $@ $<

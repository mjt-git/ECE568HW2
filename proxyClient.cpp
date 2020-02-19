#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <utility>
#include <unordered_map>

#include "proxyServer.cpp"

using namespace std;

class proxyClient {
private:
  char *  buffer;
  string hostServer;
  string serverUrl;
  size_t endIdx;
  unordered_map<int, struct sockaddr_storage> clients;// pair store id & addrinfo ipv4 ^ ipv6
public:
  void parseHttpRequest() {
    string total = "";
    for(int i = 0; buffer[i] != 0; i++) {
      total.push_back(buffer[i]);
    }
    cout << "HTTP request" << endl << total << endl;
    cout << "**********************" << endl;
    size_t hostIdx = total.find("Host:");
    size_t hostServerIdx = hostIdx + 6;
    size_t retIdx = total.find_first_of("\n", hostIdx);
    hostServer = total.substr(hostServerIdx, retIdx - hostServerIdx - 1);
    endIdx =total.find("\r\n\r\n");
  }
  
  size_t getEndIdx(){
    return endIdx;
  }
  
  int getHttpRequest() {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    const char *port     = "4444";

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
      cerr << "Error: cannot get address info for host" << endl;
      cerr << "  (" << hostname << "," << port << ")" << endl;
      return -1;
    } //if

    socket_fd = socket(host_info_list->ai_family, 
		       host_info_list->ai_socktype, 
		       host_info_list->ai_protocol);
    if (socket_fd == -1) {
      cerr << "Error: cannot create socket" << endl;
      cerr << "  (" << hostname << "," << port << ")" << endl;
      return -1;
    } //if

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      cerr << "Error: cannot bind socket" << endl;
      cerr << "  (" << hostname << "," << port << ")" << endl;
      return -1;
    } //if

    status = listen(socket_fd, 100);
    if (status == -1) {
      cerr << "Error: cannot listen on socket" << endl; 
      cerr << "  (" << hostname << "," << port << ")" << endl;
      return -1;
    } //if

    cout << "Waiting for connection on port " << port << endl;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);

    
    if (client_connection_fd == -1) {
      cerr << "Error: cannot accept connection on socket" << endl;
      return -1;
    } //if
    int numbytes;

    buffer = new char[65535];
    if((numbytes=recv(client_connection_fd, buffer, 65535, 0))==-1){
      perror("recv");
      exit(1);
    }

    buffer[numbytes] = '\0';
    parseHttpRequest(); 

    proxyServer ps(getHostServer(),getBuffer(),getEndIdx());
    int res2 = ps.getHttpResponse();
    if(res2 > 0) {
      cout << "res2: " << res2;
      //      exit(1);
    }
    char * httpResponse = ps.getBufFromServer();

    // for test
    //    cout << "HTTP RESPONSE RECEIVED: \n" << *httpResponse << endl;
    
    int bytesSend = send(client_connection_fd, httpResponse, strlen(httpResponse)+1, 0);
    cout << "bytesSend: " << bytesSend << endl;
    cout << "httpResponse length: " << strlen(httpResponse)+1 << endl;
   
    freeaddrinfo(host_info_list);
    close(socket_fd);

    // for test
    cout << "end of gethttprequest function" << endl;
    
    
    return 0;
  }
  
  char* getBuffer() {
    return buffer;
  }

  string getHostServer() {
    return hostServer;
  }
};

int main() {
  proxyClient ctop;
  int res = ctop.getHttpRequest();
  if(res == -1) {
    exit(1);
  }
  return 1;
}


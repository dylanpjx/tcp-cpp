#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // Create a socket
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  if (listening == -1) {
    std::cerr << "Can't create a socket" << std::endl;
    return -1;
  }

  // Bind socket to IP/port
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(54000);
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

  if (bind(listening, (sockaddr *)&hint, sizeof(hint)) == -1) {
    std::cerr << "Can't bind to IP/port" << std::endl;
    return -2;
  }

  // Mark socket for listening
  if (listen(listening, SOMAXCONN) == -1) {
    std::cerr << "Can't listen" << std::endl;
    return -3;
  }

  // Accept call
  sockaddr_in client;
  socklen_t clientSize = sizeof(client);
  char host[NI_MAXHOST];
  char svc[NI_MAXSERV];

  int clientSocket = accept(listening, (sockaddr *)&client, &clientSize);

  if (clientSocket == -1) {
    std::cerr << "Problem with client connecting" << std::endl;
    return -4;
  }

  // Close listening socket
  close(listening);
  memset(host, 0, NI_MAXHOST);
  memset(svc, 0, NI_MAXSERV);

  int result = getnameinfo((sockaddr *)&client, sizeof(client), host,
                           NI_MAXHOST, svc, NI_MAXSERV, 0);

  if (result) {
    std::cout << host << " connected on  " << svc << std::endl;
  } else {
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
  }

  // While receiving, display message, echo message
  char buf[4096];
  while(true) {
    // Clear buffer
    memset(buf, 0, 4096);
    // Wait for msg
    int bytesRecv = recv(clientSocket, buf, 4096, 0);
    if (bytesRecv == -1) {
      std::cerr << "There was a connection issue" << std::endl;
      break;
    }
    if (bytesRecv == 0) {
      std::cout << "The client disconnected" << std::endl;
      break;
    }
    
    // Display msg
    std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;

    // Resend msg
    send(clientSocket, buf, bytesRecv + 1, 0);
  }
  // Close socket
  close(clientSocket);
  return 0;
}

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9002
#define BUFFER_SIZE 4096

int main() {
  int serve_socket;
  serve_socket = socket(AF_INET, SOCK_STREAM, 0);

  // check if socket is live
  if (serve_socket < 0) {
    printf("There was an issue creating socket!");
    exit(EXIT_FAILURE);
    return 1;
  }

  int opt = 1;

  setsockopt(serve_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;
  socklen_t addrlen = sizeof(server_address);
  int _bind = bind(serve_socket, (struct sockaddr *)&server_address, addrlen);

  if (_bind < 0) {
    perror("Error binding");
    close(serve_socket);
    exit(EXIT_FAILURE);
  }

  if (listen(serve_socket, 5) < 0) {
    perror("Error listening");
    exit(EXIT_FAILURE);
    close(serve_socket);
  }

  printf("Server listening on http://localhost:%d\n", PORT);

  const char *http_response =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html; charset=UTF-8\r\n"
      "Content-Length: 48\r\n"
      "Connection: close\r\n"
      "\r\n"
      "<html><body><h1>Hello from C!</h1></body></html>";
  int client_socket;
  char buffer[BUFFER_SIZE] = {0};
  while (1) {
    client_socket = accept(serve_socket, (struct sockaddr *)&server_address,
                           (socklen_t *)&addrlen);
    if (client_socket < 0) {
      perror("Accepting failed");
      continue;
    }
    ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      printf("---incoming request---\n%s\n", buffer);
      send(client_socket, http_response, strlen(http_response), 0);
    }
    close(client_socket);
  }
  close(serve_socket);
  return 0;
}

/* Server code in C */
/*****************************
|> SANCHES YUCRA YHON YERSON
*****************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <map>
#include <algorithm> 

using namespace std;

map<string, int> clients;
map<int, string> socket_clients;

string int_to_string(int number, int digits){
  string ans = "";
  while (number) {
    ans.push_back('0'+ (number % 10));
    number /= 10;
  }
  reverse(ans.begin(), ans.end());
  return string(digits - ans.size(), '0') + ans;
}

void reading(int Socket_client){
  char buffer[256];
  int n;
  bool ok = 0;
  while (true){    
    bzero(buffer, 256);
    n = read(Socket_client, buffer, 1);
    // To check errors: read    
    if (n < 0) perror("ERROR reading from socket");
    char action = buffer[0];        
    switch (action){
      case 'b': {
        if(socket_clients.count(Socket_client)){
          string structure;
          string client_name = socket_clients[Socket_client];
          n = read(Socket_client, buffer, 3);
          if (n < 0) perror("ERROR reading from socket");
          int size_message = atoi(buffer);
          string size_message_string(buffer, 3);        
          n = read(Socket_client, buffer, size_message);
          if (n < 0) perror("ERROR reading from socket");
          structure = "B" + size_message_string + int_to_string(client_name.size(), 2) + string(buffer, size_message) + client_name;
          for (auto client:clients) {
            n = write(client.second, structure.c_str(), structure.size());      
            // To check errors: write
            if (n < 0) perror("ERROR writing to socket");
          }
        } else{
          string message_ = "You aren't logged in";
          string structure = "E" + message_ ;
          n = write(Socket_client, structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }
        
        break;
      } case 'u': {
        if(socket_clients.count(Socket_client)){
          string client_name = socket_clients[Socket_client];
          n = read(Socket_client, buffer, 3);
          if (n < 0) perror("ERROR reading from socket");
          int size_name_file = atoi(buffer);
          string size_name_file_string(buffer, 3);

          bzero(buffer, 256);
          n = read(Socket_client, buffer, 10);
          if (n < 0) perror("ERROR reading from socket");
          int size_data_file = atoi(buffer);
          string size_data_file_string(buffer, 10);

          bzero(buffer, 256);
          n = read(Socket_client, buffer, 2);
          if (n < 0) perror("ERROR reading from socket");
          int size_name_receiver = atoi(buffer);
          string size_name_receiver_string(buffer, 2);

          n = read(Socket_client, buffer, size_name_file);
          if (n < 0) perror("ERROR reading from socket");
          string name_file(buffer, size_name_file);

          n = read(Socket_client, buffer, size_data_file);
          if (n < 0) perror("ERROR reading from socket");
          string data_file(buffer, size_data_file);

          n = read(Socket_client, buffer, size_name_receiver);
          if (n < 0) perror("ERROR reading from socket");
          string name_receiver(buffer, size_name_receiver);

          string structure = "U" + size_name_file_string + size_data_file_string + int_to_string(client_name.size(), 2) + name_file + data_file + client_name;
          n = write(clients[name_receiver], structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }
        else{
          string message_ = "You aren't logged in";
          string structure = "E" + message_ ;
          n = write(Socket_client, structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }
        break;
      } case 'f': {
        if(socket_clients.count(Socket_client)){
          string client_name = socket_clients[Socket_client];
          n = read(Socket_client, buffer, 2);
          if (n < 0) perror("ERROR reading from socket");
          int size_name_sender = atoi(buffer);
          string size_name_sender_string(buffer, 2);

          n = read(Socket_client, buffer, size_name_sender);
          if (n < 0) perror("ERROR reading from socket");
          string name_sender(buffer, size_name_sender);

          string structure = "F" + int_to_string(client_name.size(), 2) + client_name;
          n = write(clients[name_sender], structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        } else{
          string message_ = "You aren't logged in";
          string structure = "E" + message_ ;
          n = write(Socket_client, structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }
        break;
      } case 'i': {
        if(socket_clients.count(Socket_client)){
          string client_name = socket_clients[Socket_client];
          string structure = "I" + int_to_string(clients.size(), 2);
          string client_sizes = "";
          string client_names = "";
          for (auto client:clients) {
            string size_name_client = int_to_string(client.first.size(), 2);
            client_sizes += size_name_client;
            client_names += client.first;
          }
          structure += client_sizes;
          structure += client_names;
          n = write(clients[client_name], structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }else{
          string message_ = "You aren't logged in";
          string structure = "E" + message_ ;
          n = write(Socket_client, structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }        
        break;
      } case 'm': {
        if(socket_clients.count(Socket_client)){
          string client_name = socket_clients[Socket_client];
          n = read(Socket_client, buffer, 3);
          if (n < 0) perror("ERROR reading from socket");
          int size_message = atoi(buffer);
          string size_message_string(buffer, 3);

          bzero(buffer, 256);
          n = read(Socket_client, buffer, 2);
          if (n < 0) perror("ERROR reading from socket");
          int size_name_receiver = atoi(buffer);
          string size_name_receiver_string(buffer, 2);

          n = read(Socket_client, buffer, size_message);
          if (n < 0) perror("ERROR reading from socket");
          string message(buffer, size_message);

          n = read(Socket_client, buffer, size_name_receiver);
          if (n < 0) perror("ERROR reading from socket");
          string receiver_name(buffer, size_name_receiver);

          string structure = "M" + size_message_string + int_to_string(client_name.size(), 2) + message + client_name;
          n = write(clients[receiver_name], structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }else{
          string message_ = "You aren't logged in";
          string structure = "E" + message_ ;
          n = write(Socket_client, structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }
        break;
      } case 'l': {
        n = read(Socket_client, buffer, 2);
        if (n < 0) perror("ERROR reading from socket");
        int size_user = atoi(buffer);

        n = read(Socket_client, buffer, 2);
        if (n < 0) perror("ERROR reading from socket");
        int size_password = atoi(buffer);
        
        n = read(Socket_client, buffer, size_user);
        if (n < 0) perror("ERROR reading from socket");
        string user(buffer, size_user);

        n = read(Socket_client, buffer, size_password);
        if (n < 0) perror("ERROR reading from socket");
        string password(buffer, size_password);

        if (password == "ucsp"){
          clients[user] = Socket_client;
          socket_clients[Socket_client] = user;
        } else{
          string message_ = "There was an error  ";
          string structure = "E" + message_ ;
          n = write(Socket_client, structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
        }
        // contraseña invalida
        break;
      } case 'x': {
        if(socket_clients.count(Socket_client)){
          string client_name = socket_clients[Socket_client];
          string structure = "X";
          n = write(clients[client_name], structure.c_str(), structure.size());      
          // To check errors: write
          if (n < 0) perror("ERROR writing to socket");
          socket_clients.erase(Socket_client);
          clients.erase(client_name);

        }
        break;
      }
    }
  }

  shutdown(Socket_client, SHUT_RDWR);
  close(Socket_client);
}

int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  char buffer[256];
  int n;

  // To check errors: socket
  if(-1 == SocketFD)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  // To clean memory
  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45000);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  // To check errors: bind
  if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  // To check errors: listen
  if(-1 == listen(SocketFD, 10))
  {
    perror("error listen failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }


  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    // To check errors: accept
    if(0 > ConnectFD)
    {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    thread(reading, ConnectFD).detach();

  }

  close(SocketFD);
  return 0;
}

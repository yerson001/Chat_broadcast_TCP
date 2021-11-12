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
#include <algorithm>
#include <fstream>
#include <chrono>
#include <vector>

using namespace std;

bool connected_ = 1;

string int_to_string(int number, int digits){
  string ans = "";
  while (number) {
    ans.push_back('0'+ (number % 10));
    number /= 10;
  }
  reverse(ans.begin(), ans.end());
  return string(digits - ans.size(), '0') + ans;
}

char confirm_file = '.';

void print_menu(){
  cout << "Actions: \n";
  cout << "  - l: login\n";
  cout << "  - i: To see the connected clients.\n";
  cout << "  - m: To send a message to a specific client.\n";
  cout << "  - b: To send a message to everyone.\n";
  cout << "  - u: To send a file to a specific client.\n";
  cout << "  - f: To reject a file from a specific client.\n";
  cout << "  - e: To exit the chat.\n";
}

void reading (int Socket_server) {
  char buffer[256];
  int n;  
  while (true) {    
    bzero(buffer, 256);
    n = read(Socket_server, buffer, 1);
    // To check errors: read    
    if (n < 0) perror("ERROR reading from socket");
    char action = buffer[0];
    switch (action) {
      case 'B': {
        int size_message = 0, size_sender = 0;
        string message = "";
        string name_sender = "";
        n = read(Socket_server, buffer, 3);
        // To check errors: read    
        if (n < 0) perror("ERROR reading from socket");
        size_message = atoi(buffer);
        bzero(buffer, 256);
        n = read(Socket_server, buffer, 2);
        // To check errors: read    
        if (n < 0) perror("ERROR reading from socket");
        size_sender = atoi(buffer);
        bzero(buffer, 256);
        n = read(Socket_server, buffer, size_message);
        // To check errors: read    
        if (n < 0) perror("ERROR reading from socket");
        message = string(buffer, size_message);
        bzero(buffer, 256);
        n = read(Socket_server, buffer, size_sender);
        // To check errors: read    
        if (n < 0) perror("ERROR reading from socket");
        name_sender= string(buffer, size_sender);
        cout << "From " << name_sender << ": " << message << "\n";
        break;
      } case 'U': {
        n = read(Socket_server, buffer, 3);
        if (n < 0) perror("ERROR reading from socket");
        int size_name_file = atoi(buffer);
        string size_name_file_string(buffer, 3);

        bzero(buffer, 256);
        n = read(Socket_server, buffer, 10);
        if (n < 0) perror("ERROR reading from socket");
        int size_data_file = atoi(buffer);
        string size_data_file_string(buffer, 10);

        bzero(buffer, 256);
        n = read(Socket_server, buffer, 2);
        if (n < 0) perror("ERROR reading from socket");
        int size_name_sender = atoi(buffer);
        string size_name_sender_string(buffer, 2);

        n = read(Socket_server, buffer, size_name_file);
        if (n < 0) perror("ERROR reading from socket");
        string name_file(buffer, size_name_file);

        n = read(Socket_server, buffer, size_data_file);
        if (n < 0) perror("ERROR reading from socket");
        string data_file(buffer, size_data_file);

        n = read(Socket_server, buffer, size_name_sender);
        if (n < 0) perror("ERROR reading from socket");
        string name_sender(buffer, size_name_sender);

        cout << name_sender << " is trying to send you a file, enter 'f' if you want to reject it.\n";
        clock_t end_time;
        end_time = clock() + 10000 * CLOCKS_PER_SEC / 1000;
        while (clock() < end_time){
          if(confirm_file != '.'){
            break;
          }
        }
        if(confirm_file == 'f') {
          confirm_file = '.';
          string structure = "f" + size_name_sender_string + name_sender;
          n = write(Socket_server, structure.c_str(), structure.size());
          // To check errors: write 
          if (n < 0) perror("ERROR writing to socket"); 
        } else {
          cout << "The file has been accepted.\n";
          ofstream outfile ("new" + name_file);
          outfile << data_file << "\n";
          outfile.close();
        }
        break;
      } case 'F': {
        n = read(Socket_server, buffer, 2);
        if (n < 0) perror("ERROR reading from socket");
        int size_name_receiver = atoi(buffer);

        n = read(Socket_server, buffer, size_name_receiver);
        if (n < 0) perror("ERROR reading from socket");
        string name_receiver(buffer, size_name_receiver);

        cout << name_receiver << " has rejected your file. \n";
        break;
      } case 'I': {
        n = read(Socket_server, buffer, 2);
        if (n < 0) perror("ERROR reading from socket");
        int number_clients = atoi(buffer);
        bzero(buffer, 256);
        n = read(Socket_server, buffer, 2 * number_clients);
        if (n < 0) perror("ERROR reading from socket");
        vector<string> online_clients;
        string sizes_clients(buffer, 2*number_clients);
        for(int i = 0; i < 2 * number_clients - 1; i+=2){
          char current_size_string[3];
          current_size_string[0] = sizes_clients[i];
          current_size_string[1] = sizes_clients[i + 1];
          current_size_string[2] = '\0';
          int current_size = atoi(current_size_string);
          n = read(Socket_server, buffer, current_size);
          if (n < 0) perror("ERROR reading from socket");
          string current_client(buffer, current_size);
          online_clients.push_back(current_client);
        }
        cout << " The clients are: \n";
        for(auto client: online_clients){
          cout << "  - " << client << "\n";
        }
        break;
      } case 'M': {
        n = read(Socket_server, buffer, 3);
        if (n < 0) perror("ERROR reading from socket");
        int size_message = atoi(buffer);
        string size_message_string(buffer, 3);

        bzero(buffer, 256);
        n = read(Socket_server, buffer, 2);
        if (n < 0) perror("ERROR reading from socket");
        int size_name_sender = atoi(buffer);
        string size_name_sender_string(buffer, 2);

        n = read(Socket_server, buffer, size_message);
        if (n < 0) perror("ERROR reading from socket");
        string message(buffer, size_message);

        n = read(Socket_server, buffer, size_name_sender);
        if (n < 0) perror("ERROR reading from socket");
        string sender_name(buffer, size_name_sender);

        cout << "[" << sender_name << "]: " << message << "\n";
        break;
      } case 'X': {
        cout << "Bye!!...\n";
        connected_ = 0;
        break;
      } case 'E':{
        n = read(Socket_server, buffer, 20);
        if (n < 0) perror("ERROR reading from socket");
        string error_message(buffer, 20);
        cout << error_message <<"\n";
        break;
      }
    }
  }

}

void writing(int Socket_server){
  int n;
  while (true) {
    char action;
    cin >> action;
    cin.ignore();
    string structure = "";
    switch (action) {
      case 'b': {
        cout << "Enter the message you want to send to everyone: \n";
        string message;
        getline(cin, message);
        structure = string(1, action) + int_to_string(message.size(), 3) + message;
        n = write(Socket_server, structure.c_str(), structure.size());
        // To check errors: write 
        if (n < 0) perror("ERROR writing to socket"); 
        break;
      } case 'u': {
        cout << "Enter the name of the file you want to send: \n";
        string name_file, name_receiver;
        getline(cin, name_file);
        cout << "Enter the name of the user you want to send the file: \n";
        getline(cin, name_receiver);
        string data = "", tmp = "";
        ifstream file;
        file.open(name_file);
        while(!file.eof()){
          getline(file, tmp);
          data += tmp;
          tmp.clear();
        }
        file.close();
        structure = string(1, action) + int_to_string(name_file.size(), 3) + int_to_string(data.size(), 10)  + int_to_string(name_receiver.size(), 2) + name_file + data + name_receiver;
        n = write(Socket_server, structure.c_str(), structure.size());
        // To check errors: write 
        if (n < 0) perror("ERROR writing to socket"); 
        break;
      } case 'f': {
        confirm_file = 'f';        
      } case 'i': {
        structure = string(1, action);
        n = write(Socket_server, structure.c_str(), structure.size());
        // To check errors: write 
        if (n < 0) perror("ERROR writing to socket"); 
        break;
      } case 'm': {
        cout << "Enter the nickname of the client: \n";
        string client_name;
        getline(cin, client_name);
        cout << "Enter the message: \n";
        string message;
        getline(cin, message);
        structure = string(1, action) + int_to_string(message.size(), 3) + int_to_string(client_name.size(), 2) + message + client_name;
        n = write(Socket_server, structure.c_str(), structure.size());
        // To check errors: write 
        if (n < 0) perror("ERROR writing to socket"); 
        break;
      } case 'l': {
        cout << "Enter yor nickname \n";
        string client_name;
        getline(cin, client_name);
        cout << "Enter your password: \n";
        string password;
        getline(cin, password);
        structure = string(1, action) + int_to_string(client_name.size(), 2) + int_to_string(password.size(), 2) + client_name + password;
        n = write(Socket_server, structure.c_str(), structure.size());
        // To check errors: write 
        if (n < 0) perror("ERROR writing to socket"); 
        connected_ = 1;
        break;
        // contraseña mal
      } case 'x': {
        structure = string(1, action);
        n = write(Socket_server, structure.c_str(), structure.size());
        // To check errors: write 
        if (n < 0) perror("ERROR writing to socket"); 
        break;
      }

    }

  }
}

int main(void)
{
  struct sockaddr_in stSockAddr;

  int Res;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n;

  char buffer[256];

  // To check errors: socket
  if (-1 == SocketFD)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }

  // To clean memory
  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45000);
  Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

  // To check errors: inet_pton
  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter does not contain valid ipaddress");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  // To check errors: connect
  if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("connect failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  print_menu();

  thread(reading, SocketFD).detach();
  thread(writing, SocketFD).detach();

  while (connected_){}

  shutdown(SocketFD, SHUT_RDWR);

  close(SocketFD);
  return 0;
}

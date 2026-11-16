#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <string>
using namespace std;

IPaddress ip;
TCPsocket serverSocket;
string input;
bool loop = true;

void getInput()
{
	std::getline(cin, input);
	
	if (input == "quit") { loop = false; }
}

int main(int argc, char* argv[]) {

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}
	
	int port;
	while (true)
	{
		cout << "Open a Port:" <<endl;
		getInput();
		string _port = input;
	
		if (_port == "quit")
		{
			SDLNet_Quit();
			return 1;
		}
		
		port = atoi(_port.c_str());
		if (port == 0)
		{
			cout << "Error: Invalid Port Number.\n";
		}
		else
		{
			if (SDLNet_ResolveHost(&ip, nullptr, port) == -1)
			{
				cout << "Error: SDLNet_ResolveHost error: " + to_string(*SDLNet_GetError()) + "\n";
			}
			else
			{
				break;
			}
		}
	}
	
	serverSocket = SDLNet_TCP_Open(&ip);
	if (!serverSocket)
	{
		cout << "Error: TCP_Open error: " + to_string(*SDLNet_GetError()) + "\n";
		return 1;
	}
	
	cout << "Server Online!\n";
	
	TCPsocket clientSocket;
	while (loop)
	{
		clientSocket = SDLNet_TCP_Accept(serverSocket);
		if (clientSocket)
		{
			cout << "Client Connected.." << endl;
			char buffer[1024];
			int bytes = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
			if (bytes > 0)
			{
				cout << buffer << endl;
			}
		}
	}

	cout << "Thank you for using ChArtFX !\n";
	return 0;
}



	
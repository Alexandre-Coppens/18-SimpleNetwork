#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <string>

#include "TchatBox.h"
using namespace std;

// 
enum CurrentStep
{
	NeedIp,
	NeedPort,
	Messaging,
};
void ConnectToServer();
void SendMessage(string _message);
void CloseConnection();
void ChangeCurrentStep(CurrentStep _step);
void getInput();

// Var
CurrentStep currentStep = NeedIp;
string input = "";
IPaddress ip;
TCPsocket clientSocket;
bool loop = true;

std::string strIP;
std::string strPort;

// Functions
 void ConnectToServer()
 {
 	NewLogMessage(Info, "Connecting to server...");
 	
 	int port = atoi(strPort.c_str());
 	if (port == 0)
 	{
 		NewLogMessage(Error, "Error: Invalid Port Number.");
 		ChangeCurrentStep(NeedIp);
 		return;
 	}
 	
 	if (SDLNet_ResolveHost(&ip, strIP.c_str(), port) == -1)
 	{
 		NewLogMessage(Error, "Error: SDLNet_ResolveHost error: " + to_string(*SDLNet_GetError()));
 		ChangeCurrentStep(NeedIp);
 		return;
 	}
 	
 	clientSocket = SDLNet_TCP_Open(&ip);
 	if (!clientSocket)
 	{
 		NewLogMessage(Error, "Error: TCP_Open error: " + to_string(*SDLNet_GetError()));
 		ChangeCurrentStep(NeedIp);
 		return;
 	}
 	ChangeCurrentStep(Messaging);
 }

 void SendMessage(string _message)
 {
 	int byteSent = SDLNet_TCP_Send(clientSocket, _message.c_str(), _message.length() + 1);
 	if (byteSent != _message.length() + 1)
 	{
 		NewLogMessage(Error, "Error: SDLNet_TCP_Send error: " + to_string(*SDLNet_GetError()));
 		return;
 	}
 	
 	NewLogMessage(Info, "Sent: " + to_string(byteSent) );
 	return;
 }

 void CloseConnection()
 {
 	SDLNet_TCP_Close(clientSocket);
 	currentStep = NeedIp;
 	SDLNet_Quit();
 	return;
 }

void ChangeCurrentStep(CurrentStep _step)
 {
	 currentStep = _step;
 	switch (currentStep)
 	{
 	case NeedIp:
 		NewLogMessage(Info, "-Specify IP-");
 		break;
		
 	case NeedPort:
 		NewLogMessage(Info, "-Specify Port-");
 		break;
		
 	case Messaging:
 		NewLogMessage(Info, "-Welcome to the tchat!-");
 		break;
 	}
 }

void UpdateClient()
{
	if (!TchatBox::newMessage) return;
	input = TchatBox::lastMessage;
	switch (currentStep)
	{
	case NeedIp:
		ChangeCurrentStep(NeedPort);
		strIP = input;
		break;
		
	case NeedPort:
		strPort = input;
		ConnectToServer();
		break;
		
	case Messaging:
		SendMessage(input);
		break;
	}
}

// Main
int main(int argc, char* argv[]) {

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}
	
	StartWindow();
 	ChangeCurrentStep(NeedIp);
	
	while (!WindowShouldClose())
	{
		Inputs();
		UpdateClient();
		DrawWindow();
	}

	CloseWindow();
	cout << "Thank you for using ChArtFX !\n";
	return 0;
}
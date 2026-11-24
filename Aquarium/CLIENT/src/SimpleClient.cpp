#include <charconv>
#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <string>
#include <WinDialogue.h>

#include "TchatBox.h"
using namespace std;

// 
enum class SentInfo
{
	Connect = 0,
	Disconnect = 1,
};

struct UserData
{
	std::string name;
	unsigned char texture[256 * 256 * 4];
};

struct Information
{
	std::string name;
	SentInfo info;
	unsigned char texture[256 * 256 * 4];
};

enum CurrentStep
{
	NeedIp,
	NeedPort,
	NeedImage,
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
UserData user;

std::vector<UserData> users;

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
 	NewLogMessage(Info, "Connected");
 	ChangeCurrentStep(NeedImage);
 }

void GetImage()
 {
 	while (true)
 	{
 		NewLogMessage(Info, "-Import an Image (256 / 256 max)-");
 		std::string path = WinDialogue::OpenFileWindow();
 		if (path == "") CloseConnection();
 	
 		Image img = LoadImage(path.c_str());
 		if (img.width <= 0 || img.height <= 0) NewLogMessage(Error, "Error: Invalid image file.");
 		else if (img.width > 256 || img.height > 256) NewLogMessage(Error, "Error: Image Size (Width & Height) > 1024.");
	    else
	    {
		    currentStep = Messaging;
	    	string imageBuffer;
	    	imageBuffer.reserve(256 * 256 * 4);
	    	for (int i = 0; i < img.width; i++)
	    	{
	    		for (int j = 0; j < img.height; j++)
	    		{
	    			Color pixel = GetImageColor(img, i, j);
	    			imageBuffer += static_cast<uint8_t>(pixel.r - (pixel.r == '|' ? 1 : 0));
	    			imageBuffer += static_cast<uint8_t>(pixel.b - (pixel.b == '|' ? 1 : 0));
	    			imageBuffer += static_cast<uint8_t>(pixel.a - (pixel.a == '|' ? 1 : 0));
	    			imageBuffer += static_cast<uint8_t>(pixel.g - (pixel.g == '|' ? 1 : 0));
	    		}
	    	}
	    	
	    	SendMessage(user.name + "|" + to_string(static_cast<int>(SentInfo::Connect)) + "|" + imageBuffer);
	    	currentStep = Messaging;
	    	return;
	    }
 	}
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
 		
 	case NeedImage:
 		NewLogMessage(Info, "-Specify Usename-");
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
		
	case NeedImage:
		user.name = input;
		GetImage();
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
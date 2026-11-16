#pragma once
#include <string>
#include <vector>
#include <raylib.h>

struct Color;
enum LogImportance
{
    Normal,
    Info,
    Error,
};

struct LogMessage
{
    std::string message;
    LogImportance importance;
    LogMessage(std::string _message, LogImportance _importance){message = _message; importance = _importance;};
};

class TchatBox
{
public:
    static int width;
    static int height;
    static std::string str;
    static std::string lastMessage;
    static std::vector<LogMessage> tchatLog;
    static float scrollY;
    static float backspaceSpeed;
    static double backspaceTimer;
    static bool newMessage;
};


void StartWindow();
void Inputs();
void DrawWindow();
void EndWindow();

Color GetColor(LogImportance _importance);
void NewLogMessage(LogImportance _importance, std::string _message);
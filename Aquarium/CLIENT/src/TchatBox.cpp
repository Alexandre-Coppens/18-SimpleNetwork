#include "TchatBox.h"

int TchatBox::width = 640;
int TchatBox::height = 480;
std::string TchatBox::str;
std::string TchatBox::lastMessage;
std::vector<LogMessage> TchatBox::tchatLog;
float TchatBox::scrollY = 0;
float TchatBox::backspaceSpeed = 0.1f;
double TchatBox::backspaceTimer = 0;
bool TchatBox::newMessage = false;

void StartWindow()
{
    TchatBox::width;
    TchatBox::height;
    InitWindow(TchatBox::width, TchatBox::height, "Tchat");
    SetTargetFPS(60);
}

void Inputs()
{
    TchatBox::newMessage = false;
    bool ctrl = IsKeyPressed(KEY_LEFT_CONTROL);
    int key = GetCharPressed();
    // Check if more characters have been pressed on the same frame
    while (key > 0)
    {
        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && !ctrl)
        {
            TchatBox::str += (char)key;
        }
				
        key = GetCharPressed();  // Check next character in the queue
    }
    
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V))
    {
        TchatBox::str += GetClipboardText();
    }
    
    if (IsKeyDown(KEY_BACKSPACE))
    {
        if (GetTime() - TchatBox::backspaceTimer < TchatBox::backspaceSpeed) return;
        if (TchatBox::str.length() > 0) TchatBox::str.erase(TchatBox::str.length() - 1);
        TchatBox::backspaceTimer = GetTime();
    }
    
    if(IsKeyPressed(KEY_ENTER))
    {
        if (TchatBox::str.length() > 0)
        {
            TchatBox::lastMessage = TchatBox::str;
            TchatBox::newMessage = true;
            NewLogMessage(Normal, TchatBox::str);
            TchatBox::str = "";            
        }
    }
    
    float wheel = GetMouseWheelMove();
    TchatBox::scrollY -= wheel * GetFrameTime() * 500.f;
    TchatBox::scrollY = std::min(std::max(TchatBox::scrollY, 0.f), 1000.f);
}

void DrawWindow()
{
    BeginDrawing();
    ClearBackground(GRAY);
    DrawText("Welcome to ChArtFX!", 220, 15, 25, WHITE);
    DrawRectangle(20, 50, TchatBox::width-40, TchatBox::height -150, DARKGRAY);
    DrawRectangle(20, TchatBox::height - 90, TchatBox::width-40, 50, LIGHTGRAY);
 
    for (int i = 0; i < TchatBox::tchatLog.size(); i++)
    {
        DrawText(TchatBox::tchatLog[i].message.c_str(), 25, (TchatBox::height - 100) + ((i) * 25) - TchatBox::scrollY, 20, GetColor(TchatBox::tchatLog[i].importance));
    }
    
    DrawText(TchatBox::str.c_str(), 30, TchatBox::height - 75, 20, BLACK);
    
    EndDrawing();
}

void EndWindow()
{
    CloseWindow();
}

Color GetColor(LogImportance _importance)
{
    switch (_importance)
    {
        case LogImportance::Normal:
            return WHITE;
        case LogImportance::Info:
            return GREEN;
        case LogImportance::Error:
            return RED;
    }
}

void NewLogMessage(LogImportance _importance, std::string _message)
{
    TchatBox::tchatLog.emplace_back(_message, _importance);
    TchatBox::scrollY += 25;
}

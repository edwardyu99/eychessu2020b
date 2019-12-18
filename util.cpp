#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "util.h"

/* */

bool CheckInput(void)
{
    static bool     s_Init=false;
    static BOOL     s_ConsoleMode;
    static HANDLE   s_InputHandle;
    DWORD           data;
//    if(stdin->_cnt > 0)
//    {
//        return true;
//    }
//    else
    {
        if(!s_Init)
        {
            s_Init=true;
            s_InputHandle=GetStdHandle(STD_INPUT_HANDLE);
            s_ConsoleMode=GetConsoleMode(s_InputHandle, &data);
            if(s_ConsoleMode)
            {
                SetConsoleMode(s_InputHandle, data &~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
                FlushConsoleInputBuffer(s_InputHandle);
            }
        }

        if(s_ConsoleMode)
        {
            GetNumberOfConsoleInputEvents(s_InputHandle, &data);
            return data > 1;
        }
        else
        {
            if(PeekNamedPipe(s_InputHandle, NULL, 0, NULL, &data, NULL))
            {
                return data > 0;
            }
            else
            {
                return true;
            }
        }
    }
}

const int   c_MaxInputBuff=1024;

/* */

char *ReadInput(void)
{
    static char s_LineStr[c_MaxInputBuff];
    char        *RetVal;
    if(CheckInput())
    {
        RetVal=fgets(s_LineStr, c_MaxInputBuff, stdin);
        if(RetVal != NULL)
        {
            RetVal=strchr(s_LineStr, '\n');
            if(RetVal != NULL)
            {
                *RetVal='\0';
            }

            RetVal=s_LineStr;
        }

        return RetVal;
    }
    else
    {
        return NULL;
    }
}

/* */
int ReadDigit(const char *LineStr, int MaxValue)
{
    int RetValue;
    RetValue=0;
    while(1)
    {
        if(*LineStr >= '0' && *LineStr <= '9')
        {
            RetValue*=10;
            RetValue+=*LineStr - '0';
            LineStr++;
            if(RetValue > MaxValue)
            {
                RetValue=MaxValue;
            }
        }
        else
        {
            break;
        }
    }

    return RetValue;
}

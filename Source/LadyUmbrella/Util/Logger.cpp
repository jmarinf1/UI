// 
// Logger.cpp
// 
// Implements the Lady Umbrella logging utility tool.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Logger.h"

#define DEBUG_BUFFER_SIZE 256
#define PREPARE_BUFFER()                 \
	char DebugBuffer[DEBUG_BUFFER_SIZE]; \
	va_list ArgList;                     \
	va_start(ArgList, Message);          \
	vsnprintf(DebugBuffer,               \
		DEBUG_BUFFER_SIZE,               \
	    Message,                         \
		ArgList                          \
	)

static void ConsoleLog(const char* Prefix, FColor Color, const char* Message, ...)
{
	FString FormattedMessage = FString(Prefix) + FString(Message);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			Color,
			FormattedMessage
		);
	}
}

// Version with Argument Packs.
static void ConsoleLog(const FString& _Prefix, FColor Color, const FString& _Message)
{
	FString FormattedMessage = _Prefix + _Message;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			Color,
			FormattedMessage
		);
	}
}

// Version with Argument Packs.
void FLogger::DebugLog(const char* Message, ...)
{
	PREPARE_BUFFER();
	ConsoleLog("[DEBUG]: ", FColor::Cyan, DebugBuffer);
}
void FLogger::ErrorLog(const char* Message, ...)
{
	PREPARE_BUFFER();
	ConsoleLog("[ERROR]: ", FColor::Red, DebugBuffer);
}
void FLogger::InfoLog(const char* Message, ...)
{
	PREPARE_BUFFER();
	ConsoleLog("[INFO]: ", FColor::Green, DebugBuffer);
}
void FLogger::WarningLog(const char* Message, ...)
{
	PREPARE_BUFFER();
	ConsoleLog("[WARNING]: ", FColor::Yellow, DebugBuffer);
}

// Version with FString.
void FLogger::DebugLog(const FString& _message)
{
	ConsoleLog("[DEBUG]: ", FColor::Cyan, _message);
}
void FLogger::ErrorLog(const FString& _message)
{
	ConsoleLog("[ERROR]: ", FColor::Red, _message);
}
void FLogger::InfoLog(const FString& _message)
{
	ConsoleLog("[INFO]: ", FColor::Green, _message);
}
void FLogger::WarningLog(const FString& _message)
{
	ConsoleLog("[WARNING]: ", FColor::Yellow, _message);
}

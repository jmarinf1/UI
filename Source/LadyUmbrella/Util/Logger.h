// 
// Logger.h
// 
// Utility that generalizes printing to screen. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

class
FLogger
{
public:
	// Version with Argument Packs.
	static void DebugLog(const char*, ...);
	static void ErrorLog(const char*, ...);
	static void InfoLog(const char*, ...);
	static void WarningLog(const char*, ...);

	// Version with FString.
	static void DebugLog(const FString& _message);
	static void ErrorLog(const FString& _message);
	static void InfoLog(const FString& _message);
	static void WarningLog(const FString& _message);
};



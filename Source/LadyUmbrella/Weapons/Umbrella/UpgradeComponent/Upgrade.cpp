// 
// Upgrade.cpp
// 
// Implementation of the Upgrade class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Upgrade.h"

FUpgrade::FUpgrade(const char* Name, const char* Summary, const int MaxLevel, const int Price1) 
{
	Level		   = 0;
	this->MaxLevel = MaxLevel;
	this->Price1   = Price1;
	this->Name	   = FText::FromString(Name);
	this->Summary  = FText::FromString(Summary);
}

void FUpgrade::SetLevel(const uint8 NewLevel)
{
	Level = FMath::Min(NewLevel, MaxLevel);
}

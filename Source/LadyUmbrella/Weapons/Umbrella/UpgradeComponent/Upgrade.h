// 
// Upgrade.h
// 
// Contains all the information regarding an Upgrade. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#pragma once

class FUpgrade
{
	uint8 Level;
	uint8 MaxLevel;
	//TODO: Esto hay que mirar cómo incrementa, por si queremos que los precios vayan aumentando
	uint8 Price1;
	//
	FText Name;
	FText Summary;
public:
	
	FUpgrade(const char* Name, const char* Summary, const int MaxLevel, const int Price1);

	FORCEINLINE uint8 GetLevel()	const { return Level;	 }
	FORCEINLINE uint8 GetMaxLevel() const { return MaxLevel; }
	FORCEINLINE uint8 GetPrice()	const { return Price1;	 }
	const FText& GetSummaryText()	const { return Summary;  }
	const FText& GetName()			const { return Name;	 }


	void SetLevel(const uint8 NewLevel);
	//For the future me
	/*
	void SetMaxLevel(const uint8 NewMaxLevel);
	void SetPrice(const uint8 NewPrice);
	void SetSummary(const FText& NewSummary);
	void SetName(const FText& NewName);
	*/
	
};

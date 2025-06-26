// Fill out your copyright notice in the Description page of Project Settings.


#include "LU_SaveGame.h"

#include "FChapterNames.h"


ULU_SaveGame::ULU_SaveGame()
{
	for (FString Chapter : ChapterNames::ChapterList)
	{
		ChaptersProgress.Add(FName(Chapter), {false, false, false});
	}
	ChaptersProgress.begin().Value().bLastChapterPlayed   = true;
	ChaptersProgress.begin().Value().bLastChapterUnlocked = true;

}

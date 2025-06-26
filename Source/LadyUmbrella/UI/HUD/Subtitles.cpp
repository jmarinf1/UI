
#include "Subtitles.h"

#include "Components/TextBlock.h"

void USubtitles::NativeConstruct()
{
	Super::NativeConstruct();

	StartFade = 3.0f;
	FadeSpeed = 0.01f;
	TargetOpacity = 0.0f;
	DefaultOpacity = 1.0f;
	InterpolationRate = 0.5f;
}

void USubtitles::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}

void USubtitles::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void USubtitles::AddSubtitle(const FString& CharacterName, const FString& Subtitle)
{
	const int EarliestIndex = GetEarliestZeroOpacityIndex();

	if (EarliestIndex >= 0)
	{
		const TObjectPtr<UTextBlock> SubtitleBlock = MapIndexToSubtitle(EarliestIndex);

		if (IsValid(SubtitleBlock))
		{
			UpdateText(EarliestIndex, SubtitleBlock, CharacterName, Subtitle);	
		}
	}
}

void USubtitles::UpdateText(const int Index, UTextBlock* TextBlock, const FString& CharacterName, const FString& Subtitle)
{
	UTextBlock* CorrespondingName = MapIndexToName(Index);

	SetText(CorrespondingName, TextBlock, CharacterName, Subtitle);
	SetOpacity(Index, CorrespondingName, TextBlock, DefaultOpacity);	

	TimerDelegates[Index].BindUFunction(this, FName("UpdateOpacity"), Index, CorrespondingName, TextBlock);
		
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandles[Index],
		TimerDelegates[Index],
		StartFade,
		false
	);
}

void USubtitles::UpdateOpacity(const int Index, UTextBlock* NameBlock, UTextBlock* SubtitleBlock)
{	
	SetOpacity(Index, NameBlock, SubtitleBlock, FMath::Lerp(Opacities[Index], TargetOpacity, InterpolationRate));

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandles[Index],
		TimerDelegates[Index],
		FadeSpeed,
		false
	);
}

int USubtitles::GetEarliestZeroOpacityIndex() const
{
	for (int Index = 0; Index < SUBTITLES_TEXT_BLOCK_COUNT; Index++)
	{
		if (IsZeroOpacity(Index))
			return Index;
	}

	return -1;
}

TObjectPtr<UTextBlock> USubtitles::MapIndexToName(const int Index) const
{
	switch (Index)
	{
	case 0: return BottomTextName;
	case 1: return MiddleTextName; 
	case 2: return TopTextName;
	default: return nullptr;
	}
}

TObjectPtr<UTextBlock> USubtitles::MapIndexToSubtitle(const int Index) const
{
	switch (Index)
	{
	case 0: return BottomTextSubtitle;
	case 1: return MiddleTextSubtitle; 
	case 2: return TopTextSubtitle;
	default: return nullptr;
	}
}

void USubtitles::SetText(UTextBlock* NameBlock, UTextBlock* SubtitleBlock, const FString& CharacterName,
	const FString& Subtitle)
{
	NameBlock->SetText(FText::FromString(CharacterName));
	SubtitleBlock->SetText(FText::FromString(Subtitle));
}

void USubtitles::SetOpacity(const int Index, UTextBlock* NameBlock, UTextBlock* SubtitleBlock, const float Amount)
{
	NameBlock->SetOpacity(Amount);
	SubtitleBlock->SetOpacity(Amount);
	Opacities[Index] = Amount;
}

bool USubtitles::IsZeroOpacity(const int Index) const
{
	return FMath::IsNearlyZero(Opacities[Index]);
}

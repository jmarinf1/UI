#pragma once

#include "Blueprint/UserWidget.h"
#include "Subtitles.generated.h"

class UTextBlock;

#define SUBTITLES_TEXT_BLOCK_COUNT 3

UCLASS()
class LADYUMBRELLA_API USubtitles : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	float StartFade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	float FadeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	float TargetOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	float DefaultOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	float InterpolationRate;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> EnvironmentText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TopTextName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> TopTextSubtitle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> MiddleTextName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> MiddleTextSubtitle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> BottomTextName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UTextBlock> BottomTextSubtitle;

	FTimerHandle TimerHandles[SUBTITLES_TEXT_BLOCK_COUNT];
	FTimerDelegate TimerDelegates[SUBTITLES_TEXT_BLOCK_COUNT];
	float Opacities[SUBTITLES_TEXT_BLOCK_COUNT];

public:
	
	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Show();

	UFUNCTION(BlueprintCallable, Category = Visibility)
	void Hide();

	UFUNCTION(BlueprintCallable, Category = Visibility)
	void AddSubtitle(const FString& CharacterName, const FString& Subtitle);

	FORCEINLINE TObjectPtr<UTextBlock> GetTopTextName() const { return TopTextName; }
	FORCEINLINE TObjectPtr<UTextBlock> GetTopTextSubtitle() const { return TopTextSubtitle; }
	FORCEINLINE TObjectPtr<UTextBlock> GetMiddleTextName() const { return MiddleTextName; }
	FORCEINLINE TObjectPtr<UTextBlock> GetMiddleTextSubtitle() const { return MiddleTextSubtitle; }
	FORCEINLINE TObjectPtr<UTextBlock> GetBottomTextName() const { return BottomTextName; }
	FORCEINLINE TObjectPtr<UTextBlock> GetBottomTextSubtitle() const { return BottomTextSubtitle; }
	
	FORCEINLINE void SetTopTextName(const TObjectPtr<UTextBlock>& Value) { TopTextName = Value; }
	FORCEINLINE void SetTopTextSubtitle(const TObjectPtr<UTextBlock>& Value) { TopTextSubtitle = Value; }
	FORCEINLINE void SetMiddleTextName(const TObjectPtr<UTextBlock>& Value) { MiddleTextName = Value; }
	FORCEINLINE void SetMiddleTextSubtitle(const TObjectPtr<UTextBlock>& Value) { MiddleTextSubtitle = Value; }
	FORCEINLINE void SetBottomTextName(const TObjectPtr<UTextBlock>& Value) { BottomTextName = Value; }
	FORCEINLINE void SetBottomTextSubtitle(const TObjectPtr<UTextBlock>& Value) { BottomTextSubtitle = Value; }

protected:
	virtual void NativeConstruct() override;
	
private:

	UFUNCTION(BlueprintCallable)
	void UpdateText(const int Index, UTextBlock* TextBlock, const FString& CharacterName, const FString& Subtitle);

	UFUNCTION(BlueprintCallable)
	void UpdateOpacity(const int Index, UTextBlock* CorrespondingName, UTextBlock* TextBlock);

	int GetEarliestZeroOpacityIndex() const;
	TObjectPtr<UTextBlock> MapIndexToName(const int Index) const;
	TObjectPtr<UTextBlock> MapIndexToSubtitle(const int Index) const;
	
	void SetText(UTextBlock* NameBlock, UTextBlock* SubtitleBlock, const FString& CharacterName, const FString& Subtitle);
	void SetOpacity(const int Index, UTextBlock* NameBlock, UTextBlock* SubtitleBlock, const float Amount);

	bool IsZeroOpacity(const int Index) const;
	

};

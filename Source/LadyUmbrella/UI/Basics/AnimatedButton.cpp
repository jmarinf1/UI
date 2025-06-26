// 
// AnimatedButton.cpp
// 
// Implementation of the AnimatedButton class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "LadyUmbrella/UI/Basics/AnimatedButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UAnimatedButton::NativeConstruct()
{
	Super::NativeConstruct();

	//Button
	//Button->OnUnhovered.AddDynamic(this, &UAnimatedButton::Idle);
	
	//Button->OnHovered.AddDynamic(this, &UAnimatedButton::Hovered);

	Button->OnPressed.AddDynamic(this, &UAnimatedButton::OnPressed);
	
	Button->OnClicked.AddDynamic(this, &UAnimatedButton::OnPressed);

	Button->OnReleased.AddDynamic(this, &UAnimatedButton::Hovered);
	
	IdleButtonScale = Button->GetRenderTransform().Scale;
	IdleButtonShear = Button->GetRenderTransform().Shear;
	ButtonColorOnPressed = Button->GetStyle().Pressed.TintColor.GetSpecifiedColor();
	if (ButtonColorOnIdle != FLinearColor::Transparent)
	{
		UpdateButtonColor(ButtonColorOnIdle);
	}

	IdleButtonColor = Button->GetStyle().Normal.TintColor.GetSpecifiedColor();
	HoveredButtonColor = Button->GetStyle().Hovered.TintColor.GetSpecifiedColor();
	
	//
	//	Text
	IdleTextTranslation = ButtonName->GetRenderTransform().Translation;
	TextColor = ButtonName->GetColorAndOpacity();
	if (!Text.Equals(" "))
	{
		ButtonName->SetText(FText::FromString(Text));
	}
	//

	Idle();
}

void UAnimatedButton::Idle()
{
	State = EUIState::Idle;
	UpdateState();
}


void UAnimatedButton::Hovered()
{
	State = EUIState::Hovered;
	UpdateState();
}

void UAnimatedButton::OnPressed()
{
	State = EUIState::Pressed;
	UpdateState();
}

UButton* UAnimatedButton::GetButton() const
{
	return Button;
}

void UAnimatedButton::SetButtonText(const FText& NewText) const
{
	ButtonName->SetText(NewText);
}

void UAnimatedButton::UpdateState()
{
	switch (State)
	{
	case EUIState::Idle:
		Button->SetRenderScale(FVector2D(IdleButtonScale.X, IdleButtonScale.Y));
		Button->SetRenderShear(FVector2D(IdleButtonShear.X, IdleButtonShear.Y));
		UpdateButtonColor(IdleButtonColor);
		ButtonName->SetColorAndOpacity(TextColor);
		ButtonName->SetRenderTranslation(FVector2D(IdleTextTranslation.X, IdleTextTranslation.Y));
		break;
	case EUIState::Hovered:
		if (ButtonHoveredX > 0.0f && ButtonHoveredY > 0.0f)
		{
			Button->SetRenderScale(FVector2D(ButtonHoveredX, ButtonHoveredY));
		}
		if (ButtonShearX < 0.0f || ButtonShearX > 0.0f || ButtonShearY < 0.0f || ButtonShearY > 0.0f )
		{
			Button->SetRenderShear(FVector2D(ButtonShearX, ButtonShearY));
		}
		UpdateButtonColor(HoveredButtonColor);
		ButtonName->SetColorAndOpacity(FSlateColor(TextColor));
		ButtonName->SetRenderTranslation(FVector2D(TextHoveredX, TextHoveredY));
		break;
	case EUIState::Pressed:
		Button->SetRenderScale(FVector2D(7.5f, 1.0f));
		UpdateButtonColor(ButtonColorOnPressed);
		ButtonName->SetColorAndOpacity(TextColorOnPressed);
		ButtonName->SetRenderTranslation(FVector2D(TextOnPressedX, TextOnPressedY));
	}
}

FReply UAnimatedButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	UpdateButtonColor(HoveredButtonColor);
	Hovered();

	return FReply::Handled();
}

void UAnimatedButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	UpdateButtonColor(IdleButtonColor);
	Idle();
}

void UAnimatedButton::UpdateButtonColor(const FLinearColor NewColor) const
{
	FSlateBrush CustomBrush = Button->GetStyle().Normal;
	CustomBrush.DrawAs = ESlateBrushDrawType::Box;
	CustomBrush.TintColor = FSlateColor(NewColor);
	FButtonStyle NewStyle = Button->GetStyle();
	NewStyle.SetNormal(CustomBrush);
	Button->SetStyle(NewStyle);
}

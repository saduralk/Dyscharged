// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogUI.h"
#include "DysPlayer.h"

void UDialogUI::UpdateSubtitles(TArray<FSubtitleStructure> Subtitles)
{
	if (!Subtitles.IsValidIndex(0)) return;

	//Start a background task in a low priority thread
	(new FAutoDeleteAsyncTask<UMGAsyncTask>(Subtitles, this, LetterSound))->StartBackgroundTask();
	UppercaseSubtitle = SubtitleToDisplay.ToUpper();
	UE_LOG(LogTemp, Warning, TEXT("UpdateSubtitlesIsCalled"));
}

void UDialogUI::UpdateQuestions()
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateQuestionsIsCalled"));


	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	ADysPlayer* MyDysPlayer = Cast<ADysPlayer>(PlayerController->GetPawn());

	Questions.Empty();

	Questions.Append(MyDysPlayer->Questions);
}

void UDialogUI::UpdatePortrait(UTexture2D* NewPortrait)
{
	Portrait = NewPortrait;

	UE_LOG(LogTemp, Warning, TEXT("UpdatePortraitIsCalled"));
}

void UDialogUI::ClearSubtitle()
{
	SubtitleToDisplay = FString("");
}
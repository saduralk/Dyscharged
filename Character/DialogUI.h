// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dialog.h"
#include "Subtitle.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "DialogUI.generated.h"

/**
 * 
 */
UCLASS()
class JUNKYARD_API UDialogUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/*This property will be used in order to bind our subtitles
Binding will make sure to notify the UI if the content of the following
variable change.*/
	UPROPERTY(BlueprintReadOnly)
	FString SubtitleToDisplay;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Portrait;

	UPROPERTY(BlueprintReadOnly)
	FString UppercaseSubtitle;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* LetterSound;


	/*Updates the displayed subtitles based on the given array*/
	UFUNCTION(BlueprintCallable, Category = DialogSystem)
	void UpdateSubtitles(TArray<FSubtitleStructure> Subtitles);

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = DialogSystem)
	//void ForceUpdateQuestions();

	/*This array will populate our buttons from within the show function*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString> Questions;

	/*Adds the widget to our viewport and populates the buttons with the given questions*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = DialogSystem)
	void Show();

	/*Forces an update to the questions variable*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = NewCategory)
	void UpdateQuestionsNow();

	void UpdateQuestions();

	void UpdatePortrait(UTexture2D* NewPortrait);

	void ClearSubtitle();
};

class UMGAsyncTask : public FNonAbandonableTask
{
	/*The subtitles that we're going to display*/
	TArray<FSubtitleStructure> Subs;

	/*UI Reference*/
	UDialogUI* DialogUI;

	TArray<FDialogStructure> Dstructure;

	USoundBase* LetterSound;

public:

	DECLARE_DELEGATE(FOnSubtitleUpdateDelegate);

	//Constructor
	UMGAsyncTask(TArray<FSubtitleStructure>& Subs, UDialogUI* DialogUI, USoundBase* InLetterSound)
	{
		this->Subs = Subs;
		this->DialogUI = DialogUI;
		this->LetterSound = InLetterSound;
	}

	/*Function needed by the UE in order to determine what's the tasks' status*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UMGAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	/*This function executes each time this thread is active - UE4 searches for a function named DoWord() and executes it*/
	void DoWork()
	{
		for (int32 i = 0; i < Subs.Num(); i++)
		{
			FString FullSubtitle = Subs[i].Subtitle;
			int32 Length = FullSubtitle.Len();

			//Sleep means that we pause this thread for the given time
			FPlatformProcess::Sleep(Subs[i].AssociatedTime);

			DialogUI->SubtitleToDisplay = FString("");

			for (int32 j = 0; j < Length; j++)
			{
				DialogUI->SubtitleToDisplay += FullSubtitle[j];

				if (LetterSound)
				{
					UGameplayStatics::PlaySound2D(DialogUI, LetterSound);
				}

				FOnSubtitleUpdateDelegate::CreateLambda([this]()
					{
						DialogUI->UppercaseSubtitle = DialogUI->SubtitleToDisplay.ToUpper();
					}).Execute();
				FPlatformProcess::Sleep(0.05f);
			}

			//Update our subtitles after the thread comes back
//			DialogUI->SubtitleToDisplay = Subs[i].Subtitle;
//			DialogUI->UppercaseSubtitle = Subs[i].Subtitle.ToUpper();
		}

		//Sleep 1 second to let the user read the text
		FPlatformProcess::Sleep(1.f);

		//Clear the subtitle
		DialogUI->SubtitleToDisplay = FString("");
	}
};
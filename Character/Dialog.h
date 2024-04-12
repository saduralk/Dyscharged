// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subtitle.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "Dialog.generated.h"


UCLASS()
class JUNKYARD_API ADialog : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADialog();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

USTRUCT(BlueprintType)
struct FDialogStructure : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/*The question's excerpt - this will be shown inside our UI*/
	UPROPERTY(EditAnywhere)
	FString QuestionExcerpt;

	/*The actual SFX that we're going to play*/
	UPROPERTY(EditAnywhere)
	USoundBase* SFX;

	/*An array of the associated subtitles*/
	UPROPERTY(EditAnywhere)
	TArray<FSubtitleStructure> Subtitles;

	/*True if we want to wait for the AI to respond*/
	UPROPERTY(EditAnywhere)
	bool bShouldAIAnswer;

	UPROPERTY(EditAnywhere)
	bool bLeadsToBranches;

	UPROPERTY(EditAnywhere)
	UDataTable* BranchesAIChar;

	UPROPERTY(EditAnywhere)
	UDataTable* BranchesPlayer;
};
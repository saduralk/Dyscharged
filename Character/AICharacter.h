// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Subtitle.h"
#include "Dialog.h"
#include <Components/BoxComponent.h>
#include "Components/AudioComponent.h"
#include "AICharacter.generated.h"

UCLASS()
class JUNKYARD_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

	bool bStartsTalking = false;

	bool bTimeToChange = false;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex);

	UFUNCTION()
	void Talk(USoundBase* SFX, TArray<FSubtitleStructure> Subs);

	/*Answers to the character after a specified delay*/
	void AnswerToCharacter(FName PlayerLine, TArray<FSubtitleStructure>& SubtitlesToDisplay, float delay);

	/*Retrieves the corresponding character lines*/
	UDataTable* GetPlayerLines() { return PlayerActualeLines; }

	void SetNewLines(UDataTable* NewPlayerLines, UDataTable* NewAILines);

	UPROPERTY(EditAnywhere, Category = DialogSystem)
	UTexture2D* DialogImage;

	//UPROPERTY(EditAnywhere, Category = DialogSystem)
	//FString InitialLine;

	UPROPERTY(EditAnywhere)
	FSubtitleStructure InitialSubtitleData;

	FSubtitleStructure ISD;

	UFUNCTION(BlueprintCallable, Category = "DialogSystem")
	void UpdateSubtitlesWithInitialData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComp;

	/*The audio component responsible for playing the dialog coming from this pawn*/
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComp;

	/*The player lines - each pawn can offer different dialog options for our character*/
	UPROPERTY(EditAnywhere, Category = DialogSystem)
	UDataTable* PlayerActualeLines;

	/*The ai lines*/
	UPROPERTY(EditAnywhere, Category = DialogSystem)
	UDataTable* AIActualeLines;

	UPROPERTY(EditAnywhere, Category = DialogSystem)
	bool isItem;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

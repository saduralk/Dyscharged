// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "AICharacter.h"
#include "Dialog.h"
#include "DialogUI.h"
#include "Subtitle.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "DysPlayer.generated.h"

UCLASS()
class JUNKYARD_API ADysPlayer : public APawn
{
	GENERATED_BODY()

private:
	/*True if the player is currently talking with any pawn*/
	bool bIsTalking;

	/*True if the player is inside a valid range to start talking to a pawn*/
	bool bIsInTalkRange;

	/*Initiates or terminates a conversation*/
	void ToggleTalking(float);

	/*The pawn that the player is currently talking to*/
	AAICharacter* AssociatedPawn;

	/*A reference to our lines - retrieved from the associated pawn*/
	UDataTable* AvailableLines;

	/*Searches in the given row inside the specified table*/
	FDialogStructure* RetrieveDialog(UDataTable* TableToSearch, FName RowName);

	//void UpdateInitialLine();

public:
	// Sets default values for this character's properties
	ADysPlayer();

	/*Generates the player lines*/
	void GeneratePlayerLines(UDataTable& PlayerLines);

	void EmptyQuestions();

	/*This array is essentially an Array of Excerpts from our dialogs!*/
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Questions;

	FSubtitleStructure TheFirstLine;

	/*Performs the actual talking - informs the associated pawn if necessary in order to answer
	The subtitles array contain all the subtitles for this talk - it should be passed to our UI*/
	UFUNCTION(BlueprintCallable, Category = DialogSystem)
	void Talk(FString Excerpt, TArray<FSubtitleStructure>& Subtitles);

	/*Enables / disables our talk ability. The player can't talk if he's not in a valid range*/
	void SetTalkRangeStatus(bool Status) { bIsInTalkRange = Status; }

	/*Sets a new associated pawn*/
	void SetAssociatedPawn(AAICharacter* Pawn) { AssociatedPawn = Pawn; }

	bool CheckBool(bool varToTest);

	/*Retrieves the UI reference*/
	UDialogUI* GetUI() { return UI; }

	/*Opens or closes the conversation UI*/
	UFUNCTION(BlueprintImplementableEvent, Category = DialogSystem)
	void ToggleUI();

	void SwitchUI();

	void SetInitialSubtitle(FSubtitleStructure Subtitle);

	UFUNCTION(BlueprintCallable, Category = "DialogSystem")
	class AAICharacter* GetAICharacter() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*The component responsible for playing our SFX*/
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComp;

	/*UI Reference*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UDialogUI* UI;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Components
	UPROPERTY(EditAnywhere)
	USceneComponent* Root; // Root Component

	UPROPERTY(EditAnywhere)
	UFloatingPawnMovement* Movement; // Allows for movement
	
	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera; // The player camera

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Plane; // Static mesh used for displaying the character sprite

	UPROPERTY(EditAnywhere)
	UTexture2D* DialogImage;

	// Input Handlers
	void HandleForwardInput(float);	// updates values for movement along forward axis
	void HandleSideInput(float);	// updates values for movement along side axis

	// Movement
	void Move();		// executes movement. Called every tick
	float Speed;		// set speed of movement
	bool bMoveForward;	// is moving forward/backward
	bool bMoveRight;	// is moving right/left
	int MoveIterator;	// keeps track of how many cycles of movement are left until movement is over
	int Direction;		// defines direction of movement (equal to input axis value)
	bool bStartsTalking;
};

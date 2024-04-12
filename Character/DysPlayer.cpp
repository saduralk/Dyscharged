// Fill out your copyright notice in the Description page of Project Settings.


#include "DysPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


void ADysPlayer::ToggleTalking(float value)
{

	static bool bHasTriggered = false;

	if (!bHasTriggered && value != 0)
	{

	

	if (value != 0 && !bIsTalking)
	{
		UE_LOG(LogTemp, Warning, TEXT("IA_Talk Triggered!"));

		UE_LOG(LogTemp, Warning, TEXT("ToggleTalking called with value: %f"), value);
		value -= 1;

		if (bIsInTalkRange) {

			value -= 1;
			UE_LOG(LogTemp, Warning, TEXT("Is in talk range!"));
			bIsTalking = !bIsTalking;
			bStartsTalking = true;
			ToggleUI();
			GetUI()->UpdateSubtitles({ TheFirstLine });
			bIsTalking = false;
			if (bIsTalking && AssociatedPawn) 
			{
				FVector Location = AssociatedPawn->GetActorLocation();
				FVector TargetLocation = GetActorLocation();

				//AssociatedPawn->SetActorRotation((TargetLocation - Location).Rotation());
			}
			bHasTriggered = true;
		}
			else
			{
			UE_LOG(LogTemp, Warning, TEXT("Not in talk range."));
			}
		}
	}
	else if (value == 0) {
		bHasTriggered = false;
	}
}

FDialogStructure* ADysPlayer::RetrieveDialog(UDataTable* TableToSearch, FName RowName)
{
	if (!TableToSearch) return nullptr;

	FString ContextString;
	return TableToSearch->FindRow<FDialogStructure>(RowName, ContextString);
}

// Sets default values
ADysPlayer::ADysPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// define movement values
	Speed = 2;
	MoveIterator = 0;
	bStartsTalking = true;
	bMoveForward = false;
	bMoveRight = false;

	bIsTalking = false;
	bIsInTalkRange = false;
	AssociatedPawn = nullptr;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

	// Initializes components in editor component tree
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Root);
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(Root);

	// Movement Component
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
}

void ADysPlayer::GeneratePlayerLines(UDataTable& PlayerLines)
{

	UE_LOG(LogTemp, Warning, TEXT("Ring ring, it's me, the generate player lines function"));

	//Get all the row names of the table
	TArray<FName> PlayerOptions = PlayerLines.GetRowNames();

	//For each row name try to retrieve the contents of the table
	for (auto It : PlayerOptions)
	{
		//Retrieve the contents of the table
		FDialogStructure* Dialog = RetrieveDialog(&PlayerLines, It);

		if (Dialog)
		{
			//We retrieved a valid row - populate the questions array with our excerpts
			Questions.Add(Dialog->QuestionExcerpt);
		}
	}

	//Make sure to create a reference of the available line for later use
	AvailableLines = &PlayerLines;
}

void ADysPlayer::EmptyQuestions()
{
	Questions.Empty();
	UE_LOG(LogTemp, Warning, TEXT("QUESTIONS ARE NOW EMPTY"));
}

void ADysPlayer::Talk(FString Excerpt, TArray<FSubtitleStructure>& Subtitles)
{

	GetUI()->UpdatePortrait(DialogImage);
	//Get all the row names based on our stored lines
	TArray<FName> PlayerOptions = AvailableLines->GetRowNames();

	for (auto It : PlayerOptions)
	{
		//Search inside the available lines table to find the pressed Excerpt from the UI
		FDialogStructure* Dialog = RetrieveDialog(AvailableLines, It);

		if (Dialog && Dialog->QuestionExcerpt == Excerpt)
		{
			//We've found the pressed excerpt - assign our sfx to the audio comp and play it
			AudioComp->SetSound(Dialog->SFX);
			AudioComp->Play();

			//Update the corresponding subtitles
			Subtitles = Dialog->Subtitles;


			if (UI && AssociatedPawn && Dialog->bShouldAIAnswer)
			{
				UE_LOG(LogTemp, Warning, TEXT("Each value is true, therefore the AI should say smth."));

				//Calculate the total displayed time for our subtitles
				//When the subtitles end - the associated pawn will be able to talk to our character

				TArray<FSubtitleStructure> SubtitlesToDisplay;

				float TotalSubsTime = 0.f;

				for (int32 i = 0; i < Subtitles.Num(); i++)
				{
					TotalSubsTime += Subtitles[i].AssociatedTime;
				}

				//Just a hardcoded value in order for the AI not to answer right after our subs.
				//It would be better if we expose that to our editor? Sure!
				TotalSubsTime += 1.f;

				//Tell the associated pawn to answer to our character after the specified time!
				AssociatedPawn->AnswerToCharacter(It, SubtitlesToDisplay, TotalSubsTime);

			}
			else {

				ToggleUI();
				UE_LOG(LogTemp, Warning, TEXT("The AI should not answer this question."));
			}
			if (!Dialog->bShouldAIAnswer) {
				ToggleTalking(1);
				ToggleUI();
			}
			break;

		}
	}
}

bool ADysPlayer::CheckBool(bool varToTest)
{
	return varToTest;
}

void ADysPlayer::SwitchUI()
{
	UE_LOG(LogTemp, Warning, TEXT("SwitchUIISCalled!"));

	if (UI) {
		UI->RemoveFromParent();
	}
}

void ADysPlayer::SetInitialSubtitle(FSubtitleStructure Subtitle)
{
	TheFirstLine = Subtitle;
	//GetUI()->UpdateSubtitles({ Subtitle });
}

AAICharacter* ADysPlayer::GetAICharacter() const
{
	return Cast<AAICharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("Casting to AI Character!"));
}

// Called when the game starts or when spawned
void ADysPlayer::BeginPlay()
{
	Super::BeginPlay();
}

//void ADysPlayer::ToggleUI()
//{

//}

// Called every frame
void ADysPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Checks if character is moving
	Move();
}

// Called to bind functionality to input
void ADysPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("Binding Input..."));

	// Binds movement inputs
	PlayerInputComponent->BindAxis("MoveForward", this, &ADysPlayer::HandleForwardInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADysPlayer::HandleSideInput);
	PlayerInputComponent->BindAxis("Talk", this, &ADysPlayer::ToggleTalking);
}

// Input Handling
void ADysPlayer::HandleForwardInput(float value)
{
	if (value != 0 && !bMoveForward && !bMoveRight)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Before: %s"), *GetActorLocation().ToString());
		bMoveForward = true;	// Sets movement bool to true
		Direction = value;		// Sets direction to correct value
		MoveIterator = 50;		// Sets number of iterations movement at Speed will occur
	}
}

void ADysPlayer::HandleSideInput(float value)
{
	if (value != 0 && !bMoveRight && !bMoveForward)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Before: %s"), *GetActorLocation().ToString());
		bMoveRight = true;	// Sets movement bool to true
		Direction = value;	// Sets direction to correct value
		MoveIterator = 50;	// Sets number of iterations movement at Speed will occur
	}
}

void ADysPlayer::Move()
{
	if (bMoveForward) {
		// Add movement vector to pending
		AddMovementInput(GetRootComponent()->GetRightVector(), Direction * Speed, false);
		// Consume movement vector and update location
		SetActorLocation(GetActorLocation() + ConsumeMovementInputVector());
		MoveIterator--;
		if (MoveIterator == 0)
		{
			bMoveForward = false;
			UE_LOG(LogTemp, Warning, TEXT("Before: %s"), *GetActorLocation().ToString());
		}
	}
	else if (bMoveRight) {
		// Add movement vector to pending
		AddMovementInput(GetRootComponent()->GetForwardVector(), Direction * Speed, false);
		// Consume movement vector and update location
		SetActorLocation(GetActorLocation() + ConsumeMovementInputVector());
		MoveIterator--;
		if (MoveIterator == 0) {
			bMoveRight = false;
			UE_LOG(LogTemp, Warning, TEXT("Before: %s"), *GetActorLocation().ToString());
		}
	}
}



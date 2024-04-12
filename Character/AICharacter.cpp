// Fill out your copyright notice in the Description page of Project Settings.

#include "../Junkyard.h"
#include "AICharacter.h"
#include "DysPlayer.h"

// Sets default values
AAICharacter::AAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->SetupAttachment(GetRootComponent());

	PlayerActualeLines = nullptr;
	AIActualeLines = nullptr;

	ISD = InitialSubtitleData;
}


void AAICharacter::OnBoxOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ADysPlayer>())
	{
		//UpdateLines(AIActualeLines, PlayerActualeLines);

		ADysPlayer* Char = Cast<ADysPlayer>(OtherActor);

		//Char->GetUI()->UpdateSubtitles({ InitialSubtitleData });
		//Char->GetUI()->Show();

		Char->GetUI()->UpdatePortrait(DialogImage);
		Char->SetInitialSubtitle(InitialSubtitleData);
		Char->SetTalkRangeStatus(true);
		Char->GeneratePlayerLines(*PlayerActualeLines);
		Char->SetAssociatedPawn(this);
	}
}

void AAICharacter::OnBoxEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
{
	if (OtherActor->IsA<ADysPlayer>())
	{
		ADysPlayer* Char = Cast<ADysPlayer>(OtherActor);
		Char->SetTalkRangeStatus(false);
		Char->SetAssociatedPawn(nullptr);

		Char->SwitchUI();

		Char->EmptyQuestions();

		if (bStartsTalking)
		{
			Char->SwitchUI();
		}

		//Char->ToggleUI();

		Char->GetUI()->ClearSubtitle();
	}
}

/*void AAICharacter::UpdateLines(UDataTable* NewAILines, UDataTable* NewPlayerLines)
{
	AIActualeLines = NewAILines;
	PlayerActualeLines = NewPlayerLines;
}*/


void AAICharacter::Talk(USoundBase* SFX, TArray<FSubtitleStructure> Subs)
{
	UE_LOG(LogTemp, Warning, TEXT("Now running the talk function!"));

	AudioComp->SetSound(SFX);
	AudioComp->Play();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		ADysPlayer* MyDysPlayer = Cast<ADysPlayer>(PlayerController->GetPawn());
		if (MyDysPlayer)
		{
			UE_LOG(LogTemp, Warning, TEXT("I will update the subtitle"));
			MyDysPlayer->GetUI()->UpdatePortrait(DialogImage);
			MyDysPlayer->GetUI()->UpdateSubtitles(Subs);
			if (bTimeToChange)
			{
				UE_LOG(LogTemp, Warning, TEXT("KRONK PULL THE LEVER"));
				MyDysPlayer->EmptyQuestions();

				FTimerHandle TimerHandle;
				MyDysPlayer->GetWorldTimerManager().SetTimer(TimerHandle, [MyDysPlayer](){MyDysPlayer->GetUI()->UpdateQuestionsNow(); }, 1.0f, false);
				MyDysPlayer->GeneratePlayerLines(*PlayerActualeLines);
				MyDysPlayer->GetUI()->UpdateQuestionsNow();

			}
			//MyDysPlayer->ToggleUI();
		}
	}
}

void AAICharacter::AnswerToCharacter(FName PlayerLine, TArray<FSubtitleStructure>& SubtitlesToDisplay, float delay)
{
	bStartsTalking = true;

	UE_LOG(LogTemp, Warning, TEXT("Answertocharacter function is called."));

	if (!AIActualeLines) return;

	FString ContextString;
	FDialogStructure* Dialog = AIActualeLines->FindRow<FDialogStructure>(PlayerLine, ContextString);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController)
	{
		ADysPlayer* MyDysPlayer = Cast<ADysPlayer>(PlayerController->GetPawn());

		if (MyDysPlayer && Dialog && PlayerActualeLines) {
			UE_LOG(LogTemp, Warning, TEXT("Rest will run."));

			FTimerHandle TimerHandle;
			FTimerDelegate TimerDel;

			TimerDel.BindUFunction(this, FName("Talk"), Dialog->SFX, Dialog->Subtitles);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, delay, false);

			if (Dialog->bLeadsToBranches && Dialog->BranchesAIChar && Dialog->BranchesPlayer)
			{
				UE_LOG(LogTemp, Warning, TEXT("All systems nominal. Changing values now!"));

				PlayerActualeLines = nullptr;
				AIActualeLines = nullptr;

				PlayerActualeLines = Dialog->BranchesPlayer;
				AIActualeLines = Dialog->BranchesAIChar;

				bTimeToChange = true;
			}
		}
	}
}

void AAICharacter::SetNewLines(UDataTable* NewPlayerLines, UDataTable* NewAILines)
{
	UE_LOG(LogTemp, Warning, TEXT("Oh yeah, it's all coming together."));

	PlayerActualeLines = NewPlayerLines;
	AIActualeLines = NewAILines;
}

void AAICharacter::UpdateSubtitlesWithInitialData()
{
	UE_LOG(LogTemp, Warning, TEXT("You know what's causing the issue."));

	//APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	//ADysPlayer* Char = Cast<ADysPlayer>(PlayerController->GetPawn());
	//Char->GetUI()->UpdateSubtitles({ InitialSubtitleData });
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	//Register the begin and end overlap functions
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnBoxOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AAICharacter::OnBoxEndOverlap);
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MyCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/MyItem.h"
#include "Item/Weapon/Weapon.h"
#include "Animation/AnimMontage.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(CharMappingContext, 0);
		}
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyCharacter::MoveForward);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveChAction, ETriggerEvent::Triggered, this, &AMyCharacter::MoveCh);
		EnhancedInputComponent->BindAction(LookChAction, ETriggerEvent::Triggered, this, &AMyCharacter::LookCh);
		//EnhancedInputComponent->BindAction(JumpChAction, ETriggerEvent::Triggered, this, &AMyCharacter::JumpCh);
	}

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AMyCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AMyCharacter::Attack);
}


void AMyCharacter::MoveForward(float Value)
{

	if (Controller && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}
}


void AMyCharacter::MoveCh(const FInputActionValue& Value)
{
	//if (ActionState != EActionState::EASUnoccupited) return;
	const FVector2D MoveVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0);

	if (ActionState == EActionState::EAS_Attacking) return;

	const FVector ForwardDirectional = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirectional, MoveVector.Y);
	const FVector RightDirectional = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirectional, MoveVector.X);

	//if (GetController())
	//{
	//	const FVector Forward = GetActorForwardVector();
	//	AddMovementInput(Forward, MoveVector.Y);
	//	const FVector Right = GetActorRightVector();
	//	AddMovementInput(Right, MoveVector.X);
	//}
}

void AMyCharacter::LookCh(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	
	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}

void AMyCharacter::EKeyPressed()
{
	AWeapon* OverlappinWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappinWeapon)
	{
		OverlappinWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_OneHandEquipped;
		EquippedWeapon = OverlappinWeapon;
		OverlappinWeapon = nullptr;
		OverlappingItem = nullptr;
		
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipMontage(FName ("Unequip")); 
			CharacterState = ECharacterState::ECS_Unquipped;
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_OneHandEquipped;
		}
	}

}

void AMyCharacter::Attack()
{
	
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
	
}

void AMyCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		int32 Selection = FMath::RandRange(0, 1);
		FName SelectionName = FName();
		switch (Selection)
		{
		case 0:
			SelectionName = FName("Attack1");
			break;
		case 1:
			SelectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SelectionName, AttackMontage);
	}
}

void AMyCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AMyCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupited;
}

bool AMyCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupited && CharacterState != ECharacterState::ECS_Unquipped;
}

bool AMyCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupited && 
		CharacterState != ECharacterState::ECS_Unquipped;
}

bool AMyCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupited &&
		CharacterState == ECharacterState::ECS_Unquipped &&
		EquippedWeapon;
}





// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MyItem.h"
#include "DrawDebugHelpers.h"
#include "MyProject/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/MyCharacter.h"

// Sets default values
AMyItem::AMyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMeshComponent"));
	RootComponent = MyItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AMyItem::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Begin play called!"));

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMyItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AMyItem::OnSphereEndOverlap);
}

float AMyItem::TransfomedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConst);
}


float AMyItem::TransfomedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConst);;
}


void AMyItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(OtherActor);
	if (MyCharacter)
	{
		MyCharacter->SetOverlapItem(this);
	}
}
void AMyItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* MyCharacter = Cast<AMyCharacter>(OtherActor);
	if (MyCharacter)
	{
		MyCharacter->SetOverlapItem(nullptr);
	}
}

// Called every frame
void AMyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	const float Z = TransfomedSin();
	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, Z));
	}
	


}


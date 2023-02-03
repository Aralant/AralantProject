// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyItem.generated.h"
class USphereComponent;

enum class EItemState : uint8
{
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	EIS_Equiped UMETA(DisplayName = "Equiped")
};

UCLASS()
class MYPROJECT_API AMyItem : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AMyItem();
// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TimeConst;

	UFUNCTION(BlueprintPure)
	float TransfomedSin();

	UFUNCTION(BlueprintPure)
	float TransfomedCos();

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MyItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;
};


template<typename T>
inline T AMyItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}


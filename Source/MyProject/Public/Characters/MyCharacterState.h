#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unquipped UMETA(DisplayName = "Unquipped"),
	ECS_OneHandEquipped UMETA(DisplayName = "OneHandEquipped"),
	ECS_TwoHandEquipped UMETA(DisplayName = "TwoHandEquipped")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupited UMETA(DisplayName = "Unoccupited"),
	EAS_Attacking UMETA(DisplayName = "Attacking")
};
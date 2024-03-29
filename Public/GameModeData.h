#pragma once
#ifndef GENERATED
#define GENERATED
#include "GameModeData.generated.h"
#endif

UENUM(BlueprintType)
enum class eWaeponFireMode : uint8
{
	SINGLE_FIRE,
	BURST_FIRE,
	FULL_AUTO
};

enum eWeaponMagSize
{
	AR = 30,
	HG = 15,
	SR = 10
};

UENUM(BlueprintType)
enum class eWeaponType : uint8
{
	NONE,
	AR,
	HG,
	SR
};

UENUM(BlueprintType)
enum class eScopeType : uint8
{
	NONE,
	Scope1X,
	Scope2dot5X,
	Scope6X
};

UENUM(BlueprintType)
enum class eGameType : uint8
{
	NONE,
	Battlefield,
	RainbowSix,
	Tarkov
};


UENUM(BlueprintType)
enum class eAimingType : uint8
{
	Hold,
	Toggle
};


USTRUCT(Atomic, BlueprintType)
struct FGameModeData
{
	GENERATED_USTRUCT_BODY()
	//GENERATED_BODY()
	FGameModeData()
	{
		weapon = eWeaponType::NONE;
		scope = eScopeType::NONE;
		game = eGameType::NONE;
	}
	UPROPERTY(EditAnywhere)
	eWeaponType weapon;
	UPROPERTY(EditAnywhere)
	eScopeType scope;
	UPROPERTY(EditAnywhere)
	eGameType game;
};

USTRUCT()
struct FWeaponDamage
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	int32 AR = 23;
	UPROPERTY(EditAnywhere)
	int32 HG = 19;
	UPROPERTY(EditAnywhere)
	int32 SR = 80;

	UPROPERTY(EditAnywhere)
	int32 AR_Min = 18;
	UPROPERTY(EditAnywhere)
	int32 HG_Min = 14;
	UPROPERTY(EditAnywhere)
	int32 SR_Min = 70;
};

USTRUCT()
struct FMouseSensitivity
{
	GENERATED_BODY()
	FMouseSensitivity()
	{
		HipX = 100.0f;
		HipY = 100.0f;

		Scope1X_X = 100.0f;
		Scope1X_Y = 100.0f;

		Scope2dot5X_X = 60.0f;
		Scope2dot5X_Y = 60.0f;

		Scope6X_X = 45.0f;
		Scope6X_Y = 45.0f;
	}
	UPROPERTY(EditAnywhere)
	float HipX;
	UPROPERTY(EditAnywhere)
	float HipY;

	UPROPERTY(EditAnywhere)
	float Scope1X_X;
	UPROPERTY(EditAnywhere)
	float Scope1X_Y;

	UPROPERTY(EditAnywhere)
	float Scope2dot5X_X;
	UPROPERTY(EditAnywhere)
	float Scope2dot5X_Y;

	UPROPERTY(EditAnywhere)
	float Scope6X_X;
	UPROPERTY(EditAnywhere)
	float Scope6X_Y;
};

UENUM(BlueprintType)
enum class eGameModeType : uint8
{
	None,
	Static_ShortRange,
	Static_MidRange,
	Static_LongRange,
	Movable_ShortRange,
	Movable_MidRange,
	Movable_LongRange
};
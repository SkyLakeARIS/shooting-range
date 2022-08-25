// Copyright Epic Games, Inc. All Rights Reserved.
#include "SRProjectile.h"
#include "SRPlayerController.h"
#include "DrawDebugHelpers.h"
#include "SRTargetCharacter.h"
#include "SRPlayerCharacter.h"
#include "UIHUDWidget.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ASRProjectile::ASRProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASRProjectile::OnHit);		// set up a notification for when this component hits something blocking


	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));


	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_TRACER(TEXT("/Game/MilitaryWeapDark/FX/P_Pistol_Tracer_01.P_Pistol_Tracer_01"));
	if(PS_TRACER.Succeeded())
	{
		mBulletTrace = PS_TRACER.Object;
	}

	InitialLifeSpan = 10.0f;
	mbIsCollision = false;
	mbIsTargetHit = false;
	mbDebugMode = false;
}

void ASRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (mbIsCollision == false && mbDebugMode)
	{
		DrawDebugPoint(GetWorld(), GetActorLocation(), 5.0f, FColor::Green, false, 15.0f);
	}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mBulletTrace, FTransform(GetActorRotation(), GetActorLocation()));
}

void ASRProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASRProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	mbIsCollision = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));

	if (Hit.GetActor()->GetName().Contains(FString("TargetCharacter")))
	{
		mbIsTargetHit = true;
		mHitType = EHitType::Hit;
	}

	ASRPlayerCharacter* character = nullptr;
	
	if(mbIsTargetHit)
	{
		character = Cast<ASRPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));


		// 1이 cm라는 가정하에 50미터마다 데미지가 1씩 감소합니다. 데미지 감소는 최솟값 미만이 되지 않습니다.
		const float distanceToHitPoint = (GetActorLocation() - mStartLocation).Size();
		mBulletDamage -= (distanceToHitPoint / 500.0f);
		switch(mBulletType)
		{
			case EWeaponType::AR:
				mBulletDamage =  FMath::Clamp(mBulletDamage, mDamageTable.AR_Min, mDamageTable.AR);
				break;
			case EWeaponType::SR:
				mBulletDamage = FMath::Clamp(mBulletDamage, mDamageTable.SR_Min, mDamageTable.SR);
				break;
			case EWeaponType::HG:
				mBulletDamage = FMath::Clamp(mBulletDamage, mDamageTable.HG_Min, mDamageTable.HG);
				break;
			default:
				UE_LOG(LogTemp, Error, TEXT("ASRProjectile - OnHit - EWeaponType 올바르지 않은 enum 타입입니다."));
				break;
		}

		if (Hit.GetComponent()->GetName().Contains(FString("Head")))
		{
			UE_LOG(LogTemp, Warning, TEXT("headshot!"));
			mBulletDamage *= 1.5f;
			mbIsHeadshot = true;
			mHitType = EHitType::HeadShot;
		}

		int32 getScore = 0;
		const auto targetCharacter = Cast<ASRTargetCharacter>(Hit.GetActor());
		const bool bIsKill = targetCharacter->OnHit(mBulletDamage, &getScore);

		getScore = mbIsHeadshot ? getScore * 1.5f : getScore;

		onHitAndUpdateAcc.Execute();
		mOnUpdateScore.Execute(getScore);
		if(bIsKill)
		{
			mHitType = EHitType::Kill;
			mOnUpdateKill.Execute();
		}
		mHitmark.Execute(mHitType);
	}
	Destroy();
}

/*
 * playerstate와 정보를 바인드합니다.(delegate)
 * 대상 정보: Hit count, accuracy, score, kill count
 */
void ASRProjectile::BindPlayerStateInfo(ASRPlayerState* srPlayerState)
{
	onHitAndUpdateAcc.BindUObject(srPlayerState, &ASRPlayerState::OnHitCount);
	mOnUpdateScore.BindUObject(srPlayerState, &ASRPlayerState::OnAddScore);
	mOnUpdateKill.BindUObject(srPlayerState, &ASRPlayerState::OnKill);
}

/*
 * player controller와 정보를 바인드합니다.(delegate)
 * 대상 정보: HUDWidget
 */
void ASRProjectile::BindHUDWidget(UUIHUDWidget* hud)
{
	mHitmark.BindUObject(hud, &UUIHUDWidget::AddViewPortHitMark);
}

/*
 * 발사된 총알의 타입을 무기타입으로부터 정하는 함수입니다.
 * 총알의 타입을 정하고 그에맞는 총알의 속도와 데미지를 정합니다.
 */
void ASRProjectile::SetBulletType(EWeaponType gunType)
{
	mBulletType = gunType;
	switch(mBulletType)
	{
		case EWeaponType::AR:
			mBulletDamage = mDamageTable.AR;
			break;
		case EWeaponType::HG:
			mBulletDamage = mDamageTable.HG;
			break;
		case EWeaponType::SR:
			mBulletDamage = mDamageTable.SR;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("SetBulletType : gunType - 잘못된 enum 데이터 입니다."));
			_ASSERT(false);
	}
	ProjectileMovement->InitialSpeed = 750.f;
	ProjectileMovement->MaxSpeed = 10000.f;
}

void ASRProjectile::SetStartLocation(FVector location)
{
	mStartLocation = location;
}

void ASRProjectile::SetDebugMode(bool active)
{
	mbDebugMode = active;
}

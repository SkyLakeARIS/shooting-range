﻿// Copyright Epic Games, Inc. All Rights Reserved.
#include "SRProjectile.h"
#include "SRPlayerController.h"
#include "DrawDebugHelpers.h"
#include "SRTargetCharacter.h"
#include "UIHUDWidget.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

bool ASRProjectile::bIsDebugMode = false;

ASRProjectile::ASRProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;

	mCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	mCollisionComp->InitSphereRadius(1.0f);
	mCollisionComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	mCollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	mCollisionComp->OnComponentHit.AddDynamic(this, &ASRProjectile::OnHit);		// set up a notification for when this component hits something blocking

	RootComponent = mCollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	mProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));


	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_TRACER(TEXT("/Game/MilitaryWeapDark/FX/P_Pistol_Tracer_01.P_Pistol_Tracer_01"));
	if(PS_TRACER.Succeeded())
	{
		mBulletTrace = PS_TRACER.Object;
	}

	InitialLifeSpan = 10.0f;
	mbIsCollision = false;
}

void ASRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (mbIsCollision == false && bIsDebugMode)
	{
		DrawDebugPoint(GetWorld(), GetActorLocation(), 5.0f, FColor::Green, false, 15.0f);
	}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mBulletTrace, FTransform(GetActorRotation(), GetActorLocation()));
}

void ASRProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASRProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	mOnHitAndUpdateAcc.Unbind();
	mOnUpdateScore.Unbind();
	mOnUpdateKill.Unbind();
	mShowHitmark.Unbind();
}

void ASRProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	mbIsCollision = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));

	// 타겟에 적중한 경우 외에는 별도의 충돌 처리를 하지 않습니다.
	if (!Hit.GetActor()->GetName().Contains(FString("TargetCharacter")))
	{
		Destroy();
		return;
	}

	EHitType hitType = EHitType::Hit;
	const float distanceToHitPoint = (GetActorLocation() - mStartLocation).Size();

	// 총알 타입에 맞는 데미지 테이블을 참고하여 거리에 따른 최종 데미지를 계산합니다.
	// 1이 cm라는 가정하에 50미터마다 데미지가 1씩 감소합니다. 데미지 감소는 최솟값 미만이 되지 않습니다.
	int32 bulletDamage = 0;
	const FWeaponDamage damageTable;
	switch(mBulletType)
	{
		case eWeaponType::AR:
			bulletDamage = FMath::Clamp(damageTable.AR - static_cast<int32>(distanceToHitPoint / 5000.0f), damageTable.AR_Min, damageTable.AR);
			break;
		case eWeaponType::SR:
			bulletDamage = FMath::Clamp(damageTable.SR - static_cast<int32>(distanceToHitPoint / 5000.0f), damageTable.SR_Min, damageTable.SR);
			break;
		case eWeaponType::HG:
			bulletDamage = FMath::Clamp(damageTable.HG - static_cast<int32>(distanceToHitPoint / 5000.0f), damageTable.HG_Min, damageTable.HG);
			break;
		default:
			checkf(false, TEXT("ASRProjectile - OnHit - eWeaponType 올바르지 않은 enum 타입입니다."));
			break;
	}

	// 헤드샷 여부를 검사합니다.
	bool bIsHeadshot = false;
	if (Hit.GetComponent()->GetName().Contains(FString("Head")))
	{
		bulletDamage *= 1.5f;
		bIsHeadshot = true;
		hitType = EHitType::HeadShot;
		mOnUpdateHeadshotCount.Execute();
	}

	/*
	 *  데미지 계산 후 맞은 타겟에게 적용합니다.
	 *	그리고 점수를 받아옵니다.
	 */
	int32 getScore = 0;
	auto* const targetCharacter = Cast<ASRTargetCharacter>(Hit.GetActor());
	const bool bIsKill = targetCharacter->OnHit(bulletDamage, getScore);

	getScore = bIsHeadshot ? getScore * 1.5f : getScore;
	mOnHitAndUpdateAcc.Execute();

	if(bIsKill)
	{
		// 저격총의 경우에 타겟을 쓰러트렸을 때 거리에 따른 추가 점수를 얻습니다.
		if(mBulletType == eWeaponType::SR)
		{
			getScore += static_cast<int32>(distanceToHitPoint);
		}
		hitType = EHitType::Kill;
		mOnUpdateKill.Execute();
	}

	mOnUpdateScore.Execute(getScore);
	mShowHitmark.Execute(hitType);
	
	Destroy();
}

/*
 * playerstate와 정보를 바인드합니다.(delegate)
 * 대상 정보: Hit count, accuracy, score, kill count
 */
void ASRProjectile::BindPlayerStateInfo(ASRPlayerState& srPlayerState)
{
	mOnHitAndUpdateAcc.BindUObject(&srPlayerState, &ASRPlayerState::OnAddHitCount);
	mOnUpdateScore.BindUObject(&srPlayerState, &ASRPlayerState::OnAddScore);
	mOnUpdateKill.BindUObject(&srPlayerState, &ASRPlayerState::OnAddKill);
	mOnUpdateHeadshotCount.BindUObject(&srPlayerState, &ASRPlayerState::OnAddHeadshotCount);
}

/*
 * player controller와 정보를 바인드합니다.(delegate)
 * 대상 정보: HUDWidget
 */
void ASRProjectile::BindHUDWidget(UUIHUDWidget& hud)
{
	mShowHitmark.BindUObject(&hud, &UUIHUDWidget::AddHitMarkToViewPort);
}

/*
 * 발사된 총알의 타입을 무기타입으로부터 정하는 함수입니다.
 */
void ASRProjectile::SetBulletType(const eWeaponType gunType)
{
	mBulletType = gunType;
}

void ASRProjectile::SetStartLocation(const FVector startLocation)
{
	mStartLocation = startLocation;
}
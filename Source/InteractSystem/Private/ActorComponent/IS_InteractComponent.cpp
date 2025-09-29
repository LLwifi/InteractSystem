// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/IS_InteractComponent.h"
#include "ActorComponent/IS_BeInteractComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UIS_InteractComponent::UIS_InteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}

#if WITH_EDITOR
void UIS_InteractComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	auto Property = PropertyChangedEvent.Property;//�õ��ı������
	//if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UTS_TaskComponent, TaskRole))
	//{
	//	if (TaskRole == ETS_TaskRole::Player)
	//	{
	//		SetIsReplicated(true);
	//	}
	//	else
	//	{
	//		SetIsReplicated(false);
	//	}
	//}
}
#endif

// Called when the game starts
void UIS_InteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//if (UKismetSystemLibrary::IsServer(this))
	//{
	//	InteractCheckStateChange(bBeginPlayIsActiveInteractCheck);
	//}

	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		InteractCheckStateChange(bBeginPlayIsActiveInteractCheck);
	}
}

void UIS_InteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIS_InteractComponent, CurStartInteractComponent);
	//DOREPLIFETIME(UIS_InteractComponent, InteractCheckComponents);
}

// Called every frame
void UIS_InteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FName UIS_InteractComponent::GetRoleSign()
{
	if (RoleSign.IsNone())
	{
		return FName(UKismetSystemLibrary::GetDisplayName(this));
	}
	return RoleSign;
}

bool UIS_InteractComponent::StartInteract(UIS_BeInteractComponent* BeInteractComponent)
{
	if (BeInteractComponent)
	{
		if (CurStartInteractComponent)//��ǰ�Ƿ����Ѿ��ڽ�������һ����Դ
		{
			EndCurInteract();
		}
		CurStartInteractComponent = BeInteractComponent;
		IIS_BeInteractInterface::Execute_InteractStart(BeInteractComponent, this);//֪ͨĿ�꿪ʼ����������
	}
	
	return true;
}

void UIS_InteractComponent::EndCurInteract()
{
	//��Щ����ڽ����������ɾ���ˣ����ܻᵼ����������������
	if (CurStartInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractEnd(CurStartInteractComponent, this);
		CurStartInteractComponent = nullptr;
	}
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::TraceOutHitCheck(EIS_InteractTraceType InteractTraceType, FIS_InteractRayInfo InteractRayInfo, const TArray<FHitResult>& OutHit, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//ȫ���ɱ��������
	for (const FHitResult& HitResult : OutHit)
	{
		UIS_BeInteractComponent* BeInteractComponent = Cast<UIS_BeInteractComponent>(HitResult.GetComponent());
		if (BeInteractComponent)
		{
			bool IsCheck = false;
			FText FailText;
			if (BeInteractComponent->BeInteractInfo.InteractCheckEnterCondition.Contains(InteractTraceType))
			{
				IsCheck = true;
				FIS_BeInteractCheckCondition Condition = BeInteractComponent->BeInteractInfo.InteractCheckEnterCondition[InteractTraceType];
				if (Condition.bIsActiveTraceCheck)
				{
					IsCheck = IIS_BeInteractInterface::Execute_IsInteractActive(BeInteractComponent);
				}
				if (IsCheck && Condition.bIsVerifyTraceCheck)//�Ƿ�������֤
				{
					if(IsCheck && Condition.bIsDistanceVerifyTraceCheck && BeInteractComponent->BeInteractInfo.InteractVerifyInfo.bOverride_InteractDistanceVerify)//�Ƿ��о�����֤���ұ�����Ŀ�������˾�����֤
					{
						IsCheck = BeInteractComponent->BeInteractInfo.InteractVerifyInfo.DistanceVerify(this, BeInteractComponent, FailText);
					}
					if (IsCheck && Condition.bIsAngleVerifyTraceCheck && BeInteractComponent->BeInteractInfo.InteractVerifyInfo.bOverride_InteractAngleVerify)//�Ƿ��нǶ���֤���ұ�����Ŀ�������˽Ƕ���֤
					{
						IsCheck = BeInteractComponent->BeInteractInfo.InteractVerifyInfo.AngleVerify(this, BeInteractComponent, FailText);
					}
				}
				if (IsCheck && Condition.bIsCompareTraceCheck)
				{
					IsCheck = BeInteractComponent->BeInteractInfo.BeCompareInfo.CompareResult(DefaultCompareInfo, FailText);
				}
			}

			if (IsCheck)
			{
				AllBeInteract.Add(BeInteractComponent);
				if (!TopPriorityCom || //Ϊ��ֱ������
					IIS_BeInteractInterface::Execute_GetInteractPriority(BeInteractComponent) > IIS_BeInteractInterface::Execute_GetInteractPriority(TopPriorityCom))
				{
					TopPriorityCom = BeInteractComponent;
				}
			}
		}
	}
	return AllBeInteract;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::GetInteractCheckComponentFromType(EIS_InteractTraceType InteractTraceType, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//ȫ���ɱ��������
	TopPriorityCom = nullptr;
	for (UIS_BeInteractComponent*& BeInteractComponent : InteractCheckComponents)
	{
		if (InteractTraceType == EIS_InteractTraceType::None || BeInteractComponent->BeInteractDynamicInfo.AllEnterTraceType.Contains(InteractTraceType))
		{
			AllBeInteract.Add(BeInteractComponent);
			if (!TopPriorityCom || //Ϊ��ֱ������
				IIS_BeInteractInterface::Execute_GetInteractPriority(BeInteractComponent) > IIS_BeInteractInterface::Execute_GetInteractPriority(TopPriorityCom))
			{
				TopPriorityCom = BeInteractComponent;
			}
		}
	}
	return AllBeInteract;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::CameraTraceGetBeInteractCom(FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//ȫ���ɱ��������
	TopPriorityCom = nullptr;
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->GetController())
	{
		FVector CameraLocation, TraceEndLocation;
		FRotator CameraRotation;
		Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		TraceEndLocation = (UKismetMathLibrary::GetForwardVector(CameraRotation) * TraceDistance) + CameraLocation;

		//�������ģ�������������ʹ��Բ�ε�����
		TArray<FHitResult> OutHit;
		UKismetSystemLibrary::LineTraceMulti(this, CameraLocation, TraceEndLocation, TraceChannel, InteractRayInfo.bTraceComplex, InteractRayInfo.ActorsToIgnore, DrawDebugType, OutHit, InteractRayInfo.bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
		AllBeInteract = TraceOutHitCheck(EIS_InteractTraceType::CameraTrace, InteractRayInfo, OutHit, TopPriorityCom);
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TryTriggerInteract_CameraTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText)
{
	//InteractCheck();//������øú��� �����縴���»ᵼ��������ͬ���˽���ļ�����ͣ�Ȼ����������û������ÿɱ�����������˻ᴥ�����뿪���¼� ���ɡ��뿪���¼�������������
	bool ReturnBool = false;
	AllBeInteract = CameraTraceGetBeInteractCom(InteractRayInfo, TopPriorityCom);
	if (TopPriorityCom && IIS_BeInteractInterface::Execute_CanInteract(TopPriorityCom, this, CompareInfo, FailText))
	{
		StartInteract(TopPriorityCom);
		UpdateInteractTarget(TopPriorityCom, EIS_InteractTraceType::CameraTrace);
		ReturnBool = true;
	}
	return ReturnBool;
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::SphereTraceGetBeInteractCom(FVector Start, FVector End, float Radius, FIS_InteractRayInfo InteractRayInfo, UIS_BeInteractComponent*& TopPriorityCom)
{
	TArray<UIS_BeInteractComponent*> AllBeInteract;//ȫ���ɱ��������
	TopPriorityCom = nullptr;
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->GetController())
	{
		FVector CameraLocation, TraceEndLocation;
		FRotator CameraRotation;
		Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		TraceEndLocation = (UKismetMathLibrary::GetForwardVector(CameraRotation) * TraceDistance) + CameraLocation;

		//�������ģ�������������ʹ��Բ�ε�����
		TArray<FHitResult> OutHit;
		UKismetSystemLibrary::SphereTraceMulti(this, Start, End, Radius, TraceChannel, InteractRayInfo.bTraceComplex, InteractRayInfo.ActorsToIgnore, DrawDebugType, OutHit, InteractRayInfo.bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
		AllBeInteract = TraceOutHitCheck(EIS_InteractTraceType::SphereTrace, InteractRayInfo, OutHit, TopPriorityCom);
	}
	return AllBeInteract;
}

bool UIS_InteractComponent::TryTriggerInteract_SphereTrace(FCC_CompareInfo CompareInfo, FIS_InteractRayInfo InteractRayInfo, FVector Start, FVector End, float Radius, UIS_BeInteractComponent*& TopPriorityCom, TArray<UIS_BeInteractComponent*>& AllBeInteract, FText& FailText)
{
	//InteractCheck();//������øú��� �����縴���»ᵼ��������ͬ���˽���ļ�����ͣ�Ȼ����������û������ÿɱ�����������˻ᴥ�����뿪���¼� ���ɡ��뿪���¼�������������
	bool ReturnBool = false;
	AllBeInteract = SphereTraceGetBeInteractCom(Start, End, Radius, InteractRayInfo, TopPriorityCom);
	if (TopPriorityCom && IIS_BeInteractInterface::Execute_CanInteract(TopPriorityCom, this, CompareInfo, FailText))
	{
		StartInteract(TopPriorityCom);
		UpdateInteractTarget(TopPriorityCom, EIS_InteractTraceType::SphereTrace);
		ReturnBool = true;
	}
	return ReturnBool;
}

void UIS_InteractComponent::InteractCheckStateChange(bool IsActive)
{
	if (IsActive)
	{
		GetWorld()->GetTimerManager().SetTimer(InteractCheckTimeHandle, this, &UIS_InteractComponent::InteractCheck, InteractCheckInterval, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(InteractCheckTimeHandle);
	}
}

void UIS_InteractComponent::InteractCheck()
{
	//Enter���Ա���δ�������ÿ�ּ�������ֻ�ܴ���һ��
	TSet<UIS_BeInteractComponent*> AllBeInteractCom;
	for (EIS_InteractTraceType& TraceType : InteractCheckTypes)
	{
		AllBeInteractCom.Append(InteractCheckFromEnterType(TraceType));
	}

	InteractCheckComponents = AllBeInteractCom.Array();
}

TArray<UIS_BeInteractComponent*> UIS_InteractComponent::InteractCheckFromEnterType(EIS_InteractTraceType InteractTraceType)
{
	UIS_BeInteractComponent* TopPriorityCom = nullptr;
	TArray<UIS_BeInteractComponent*> AllBeInteractCom;
	switch (InteractTraceType)
	{
	case EIS_InteractTraceType::CameraTrace:
	{
		AllBeInteractCom = CameraTraceGetBeInteractCom(InteractCheckRayInfo, TopPriorityCom);
		break;
	}
	case EIS_InteractTraceType::SphereTrace:
	{
		AllBeInteractCom = SphereTraceGetBeInteractCom(GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), SphereDistance, InteractCheckRayInfo, TopPriorityCom);
		break;
	}
	default:
		break;
	}

	TArray<UIS_BeInteractComponent*> LastInteractCheckComponents = InteractCheckComponents;
	for (UIS_BeInteractComponent*& BeInteractCom : LastInteractCheckComponents)
	{
		//BeInteractCom��Ч��ʰȡ���߻��ڽ�����ɾ��Ŀ�꣩ && ��������Ľ���������ڲ�����һ���Ľ�������� && ��������Ƿ�ͨ��
		if (BeInteractCom && !AllBeInteractCom.Contains(BeInteractCom) && BeInteractCom->TraceTypeCheck(InteractTraceType, false))
		{
			//���ڣ������뿪�¼� �뿪��Ҫ������������������뿪��Ҫ�ڷ������ϱ�����
			ServerLeaveInteractCheck(BeInteractCom, InteractTraceType);
			IIS_BeInteractInterface::Execute_InteractLeave(BeInteractCom, this, InteractTraceType);//�ͻ�����Ȼ��Ҫ�Ƴ��ü������
			UpdateInteractTarget(nullptr, InteractTraceType);//�㲥�����¼�
		}
	}

	for (UIS_BeInteractComponent*& BeInteractCom : AllBeInteractCom)
	{
		if (BeInteractCom->TraceTypeCheck(InteractTraceType))
		{
			FIS_BeInteractCheckCondition Condition = BeInteractCom->BeInteractInfo.InteractCheckEnterCondition[InteractTraceType];
			EIS_BeInteractCheckType BeInteractEnterType = Condition.InteractCheckType;
			switch (BeInteractEnterType)//���ڣ����Դ��������¼�
			{
			case EIS_BeInteractCheckType::AnyTrigger:
			{
				IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, InteractTraceType);//�����½���Ŀ��
				UpdateInteractTarget(BeInteractCom, InteractTraceType);//�㲥�����¼�
				break;
			}
			case EIS_BeInteractCheckType::TopPriorityTrigger:
			{
				if (TopPriorityCom == BeInteractCom)
				{
					IIS_BeInteractInterface::Execute_InteractEnter(BeInteractCom, this, InteractTraceType);//�����½���Ŀ��
					UpdateInteractTarget(BeInteractCom, InteractTraceType);//�㲥�����¼�
				}
				break;
			}
			default:
				break;
			}
		}
	}

	return AllBeInteractCom;
}

void UIS_InteractComponent::UpdateInteractTarget_Implementation(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType TraceType)
{
	UpdateInteractEvent.Broadcast(this, BeInteractComponent, TraceType);
}

void UIS_InteractComponent::ServerVerifyCurInteractComplete_Implementation()
{
	if (CurStartInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractComplete(CurStartInteractComponent, this);
	}
}

void UIS_InteractComponent::ServerVerifyCurInteractEnd_Implementation()
{
	if (CurStartInteractComponent)
	{
		CurStartInteractComponent->BeInteractDynamicInfo.bIsVerifying = false;
		IIS_BeInteractInterface::Execute_InteractEnd(CurStartInteractComponent, this);
	}
}

void UIS_InteractComponent::ServerLeaveInteractCheck_Implementation(UIS_BeInteractComponent* BeInteractComponent, EIS_InteractTraceType InteractTraceType)
{
	if (BeInteractComponent)
	{
		IIS_BeInteractInterface::Execute_InteractLeave(BeInteractComponent, this, InteractTraceType);//�Ƴ��ɽ���Ŀ��
	}
}

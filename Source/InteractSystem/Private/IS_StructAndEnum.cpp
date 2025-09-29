#include <IS_StructAndEnum.h>
#include "Library/IS_BlueprintFunctionLibrary.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"

bool FIS_InteractVerifyInfo::Verify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText)
{
	FailText = FText();
	if (DistanceVerify(InteractComponent, BeInteractComponent, FailText) &&
		AngleVerify(InteractComponent, BeInteractComponent, FailText))
	{
		return true;
	}
	return false;
}

bool FIS_InteractVerifyInfo::DistanceVerify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText)
{
	FailText = FText();
	if (InteractComponent && BeInteractComponent)//双方均有效
	{
		if (bOverride_InteractDistanceVerify)
		{
			FVector Dir = InteractComponent->GetOwner()->GetActorLocation() - BeInteractComponent->GetOwner()->GetActorLocation();
			float InteractDistance = UKismetMathLibrary::VSize(Dir);
			if (!InteractDistanceVerify.Contains(InteractDistance))//距离验证
			{
				FailText = InteractDistance_FailText;
				return false;
			}
		}
		return true;
	}
	return false;
}

bool FIS_InteractVerifyInfo::AngleVerify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText)
{
	FailText = FText();
	if (InteractComponent && BeInteractComponent)//双方均有效
	{
		if (bOverride_InteractAngleVerify)
		{
			FVector Dir = InteractComponent->GetOwner()->GetActorLocation() - BeInteractComponent->GetOwner()->GetActorLocation();
			float InteractAngle = UIS_BlueprintFunctionLibrary::GetAngleFromMyForwardAndTargetDir(BeInteractComponent->GetOwner()->GetActorForwardVector(), Dir);
			if (!InteractAngleVerify.Contains(InteractAngle))//角度验证
			{
				FailText = InteractAngle_FailText;
				return false;
			}
		}
		return true;
	}
	return false;
}

void FIS_BeInteractExtend::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
	if (BeInteractExtendClass)
	{
		if (!BeInteractExtend || BeInteractExtend->GetClass() != BeInteractExtendClass)
		{
			BeInteractExtend = NewObject<UIS_BeInteractExtendBase>(const_cast<UDataTable*>(InDataTable), BeInteractExtendClass);
		}
	}
	else
	{
		BeInteractExtend = nullptr;
	}
}

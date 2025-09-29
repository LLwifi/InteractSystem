// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../InteractSystem/Public/Common/IS_BeInteractInterface.h"
#include "IPropertyTypeCustomization.h"



/**
 * 
 */
class INTERACTSYSTEMEDITOR_API IIS_BeInteractInfoHandleCustomization : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new IIS_BeInteractInfoHandleCustomization());
    }

    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

    virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

    //�ı�ѡ��Ļص�
    void OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo);
    //ÿ������ѡ������ʽ
    TSharedRef<SWidget> OnGenerateWidget_RowName(TSharedPtr<FString> InItem);

public:

    //������
    TSharedPtr<IPropertyHandle> RowNameHandle;
    TArray<TSharedPtr<FString>> RowNames;
    TSharedPtr<class SSearchableComboBox> SearchableComboBox_RowName;
    TSharedPtr<class STextBlock> ComboBox_Name_Text;

    //��������Ľṹ������
    FIS_BeInteractInfoHandle* IS_BeInteractInfoHandle;
};


/**
 *
 */
class INTERACTSYSTEMEDITOR_API IIS_BeInteractExtendHandleCustomization : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new IIS_BeInteractExtendHandleCustomization());
    }

    virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

    virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

    //�ı�ѡ��Ļص�
    void OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo);
    //ÿ������ѡ������ʽ
    TSharedRef<SWidget> OnGenerateWidget_RowName(TSharedPtr<FString> InItem);

public:

    //������
    TSharedPtr<IPropertyHandle> RowNameHandle;
    TArray<TSharedPtr<FString>> RowNames;
    TSharedPtr<class SSearchableComboBox> SearchableComboBox_RowName;
    TSharedPtr<class STextBlock> ComboBox_Name_Text;

    //��������Ľṹ������
    FIS_BeInteractExtendHandle* IS_BeInteractExtendHandle;
};
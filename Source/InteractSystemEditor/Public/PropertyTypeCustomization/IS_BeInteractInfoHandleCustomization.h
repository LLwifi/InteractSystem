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

    //改变选择的回调
    void OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo);
    //每个下拉选择框的样式
    TSharedRef<SWidget> OnGenerateWidget_RowName(TSharedPtr<FString> InItem);

public:

    //行名称
    TSharedPtr<IPropertyHandle> RowNameHandle;
    TArray<TSharedPtr<FString>> RowNames;
    TSharedPtr<class SSearchableComboBox> SearchableComboBox_RowName;
    TSharedPtr<class STextBlock> ComboBox_Name_Text;

    //反射出来的结构体数据
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

    //改变选择的回调
    void OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo);
    //每个下拉选择框的样式
    TSharedRef<SWidget> OnGenerateWidget_RowName(TSharedPtr<FString> InItem);

public:

    //行名称
    TSharedPtr<IPropertyHandle> RowNameHandle;
    TArray<TSharedPtr<FString>> RowNames;
    TSharedPtr<class SSearchableComboBox> SearchableComboBox_RowName;
    TSharedPtr<class STextBlock> ComboBox_Name_Text;

    //反射出来的结构体数据
    FIS_BeInteractExtendHandle* IS_BeInteractExtendHandle;
};
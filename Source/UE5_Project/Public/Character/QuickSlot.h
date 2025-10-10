// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Character/ItemBase.h"
#include "QuickSlot.generated.h"

USTRUCT(BlueprintType)
struct FQuickSlotData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AItemBase* ItemRef = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsOccupied = false;
};

/**
 *
 */
UCLASS()
class UE5_PROJECT_API UQuickSlot : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:

    void SetCurrentSlot(int32 NewIndex);
    AItemBase* GetItemAt(int32 Index) const;

    // �� ���Կ� ������ �߰�
    void AddItemToEmptySlot(AItemBase* NewItem);

    // ���� �� á���� Ȯ��
    bool IsFull() const;


private:
    // ���� ������ 2��
    UPROPERTY()
    TArray<FQuickSlotData> Slots;

    // ���� ���õ� ���� �ε���
    int32 CurrentSlotIndex = 0;
    void UpdateSlotHighlight();

    // �������Ʈ ���� ����
    UPROPERTY(meta = (BindWidget))
    UImage* Img_Icon1;

    UPROPERTY(meta = (BindWidget))
    UImage* Img_Icon2;

    // ���� ������ ������Ʈ��
    void UpdateSlotIcon(int32 SlotIndex, UTexture2D* NewIcon);
};
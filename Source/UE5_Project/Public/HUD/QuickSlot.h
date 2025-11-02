#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "QuickSlot.generated.h"


class AItemBase;


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


    // 슬롯 꽉 찼는지 확인
    bool IsFull() const;


    int32 FindSlotIndexByItem(AItemBase* TargetItem) const;
    void UpdateSlotIcon(int32 SlotIndex, UTexture2D* NewIcon);
    void ForceRefreshUI();


    int32 GetCurrentSlotIndex() const { return CurrentSlotIndex; }
    void AssignItemToSlot(int32 Index, AItemBase* NewItem);
    void RemoveItemAt(int32 Index);

private:
    // 슬롯 데이터 2개
    UPROPERTY()
    TArray<FQuickSlotData> Slots;

    // 현재 선택된 슬롯 인덱스
    int32 CurrentSlotIndex = 0;
    void UpdateSlotHighlight();

    // 블루프린트 위젯 연결
    UPROPERTY(meta = (BindWidget))
    UImage* Img_Icon1;

    UPROPERTY(meta = (BindWidget))
    UImage* Img_Icon2;

   
};
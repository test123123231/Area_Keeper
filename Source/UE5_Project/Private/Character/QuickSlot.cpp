#include "Character/QuickSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UQuickSlot::NativeConstruct()
{
    Super::NativeConstruct();

    // 슬롯 2개 초기화
    Slots.SetNum(2);
    for (FQuickSlotData& EachSlot : Slots)
    {
        EachSlot.ItemRef = nullptr;
        EachSlot.Icon = nullptr;
        EachSlot.bIsOccupied = false;
    }

    CurrentSlotIndex = 0;

    // 초기 아이콘 비우기 (UI에 아무것도 표시 안 함)
    if (Img_Icon1) Img_Icon1->SetBrushFromTexture(nullptr);
    if (Img_Icon2) Img_Icon2->SetBrushFromTexture(nullptr);
}


/*void UQuickSlot::AddItemToEmptySlot(AItemBase* NewItem)
{
    if (!NewItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItemToEmptySlot: Invalid item!"));
        return;
    }

    for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (!Slots[i].bIsOccupied)
        {
            Slots[i].ItemRef = NewItem;
            Slots[i].bIsOccupied = true;

            // 아이템 아이콘 설정 (나중에 AItemBase에서 지정 가능)
            // 임시로 하얀색 테스트 텍스처 로드 (없으면 블루프린트에서 설정)
            UTexture2D* IconTex = NewItem->ItemIcon;
            if (!IconTex)
            {
                IconTex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Icons/TestIcon.TestIcon"));
            }
            Slots[i].Icon = IconTex;

            // UI에 반영
            if (i == 0 && Img_Icon1)
            {
                Img_Icon1->SetBrushFromTexture(IconTex);
            }
            else if (i == 1 && Img_Icon2)
            {
                Img_Icon2->SetBrushFromTexture(IconTex);
            }

            UE_LOG(LogTemp, Warning, TEXT("QuickSlot: %s added to slot %d"), *NewItem->GetName(), i);
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("QuickSlot: No empty slot available!"));
}*/

void UQuickSlot::AddItemToEmptySlot(AItemBase* NewItem)
{
    if (!NewItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddItemToEmptySlot: Invalid item!"));
        return;
    }

    // 슬롯 순회하며 빈 공간 찾기
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (!Slots[i].bIsOccupied)
        {
            Slots[i].ItemRef = NewItem;
            Slots[i].Icon = NewItem->ItemIcon;
            Slots[i].bIsOccupied = true;



            UpdateSlotIcon(i, NewItem->ItemIcon);

            UE_LOG(LogTemp, Warning, TEXT("QuickSlot: %s added to slot %d"),
                *NewItem->GetName(), i);
            return;
        }
    }

    // 여기까지 왔다는 건 슬롯이 모두 찼다는 의미
    UE_LOG(LogTemp, Warning, TEXT("QuickSlot: All slots are full!"));
}

// 슬롯이 모두 찼는지 판단
bool UQuickSlot::IsFull() const
{
    return Slots.Num() == 2 && Slots[0].bIsOccupied && Slots[1].bIsOccupied;
}


// 아이콘 업데이트 (공통 함수)
void UQuickSlot::UpdateSlotIcon(int32 SlotIndex, UTexture2D* NewIcon)
{
    if (SlotIndex == 0 && Img_Icon1)
        Img_Icon1->SetBrushFromTexture(NewIcon);
    else if (SlotIndex == 1 && Img_Icon2)
        Img_Icon2->SetBrushFromTexture(NewIcon);
}

void UQuickSlot::SetCurrentSlot(int32 NewIndex)
{
    if (NewIndex < 0 || NewIndex >= Slots.Num())
        return;

    CurrentSlotIndex = NewIndex;
    UpdateSlotHighlight();

    UE_LOG(LogTemp, Warning, TEXT("Current QuickSlot selected: %d"), CurrentSlotIndex);
}

AItemBase* UQuickSlot::GetItemAt(int32 Index) const
{
    if (Index < 0 || Index >= Slots.Num()) return nullptr;
    return Slots[Index].ItemRef;
}

// 하이라이트 색상 변경 (선택된 슬롯만 노랗게)
void UQuickSlot::UpdateSlotHighlight()
{
    FLinearColor NormalColor = FLinearColor::White;
    FLinearColor HighlightColor = FLinearColor::Yellow;

    if (Img_Icon1)
        Img_Icon1->SetColorAndOpacity(CurrentSlotIndex == 0 ? HighlightColor : NormalColor);

    if (Img_Icon2)
        Img_Icon2->SetColorAndOpacity(CurrentSlotIndex == 1 ? HighlightColor : NormalColor);
}
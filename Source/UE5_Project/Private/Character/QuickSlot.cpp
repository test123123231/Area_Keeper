#include "Character/QuickSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UQuickSlot::NativeConstruct()
{
    Super::NativeConstruct();

    // ���� 2�� �ʱ�ȭ
    Slots.SetNum(2);
    for (FQuickSlotData& EachSlot : Slots)
    {
        EachSlot.ItemRef = nullptr;
        EachSlot.Icon = nullptr;
        EachSlot.bIsOccupied = false;
    }

    CurrentSlotIndex = 0;

    // �ʱ� ������ ���� (UI�� �ƹ��͵� ǥ�� �� ��)
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

            // ������ ������ ���� (���߿� AItemBase���� ���� ����)
            // �ӽ÷� �Ͼ�� �׽�Ʈ �ؽ�ó �ε� (������ �������Ʈ���� ����)
            UTexture2D* IconTex = NewItem->ItemIcon;
            if (!IconTex)
            {
                IconTex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Icons/TestIcon.TestIcon"));
            }
            Slots[i].Icon = IconTex;

            // UI�� �ݿ�
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

    // ���� ��ȸ�ϸ� �� ���� ã��
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

    // ������� �Դٴ� �� ������ ��� á�ٴ� �ǹ�
    UE_LOG(LogTemp, Warning, TEXT("QuickSlot: All slots are full!"));
}

// ������ ��� á���� �Ǵ�
bool UQuickSlot::IsFull() const
{
    return Slots.Num() == 2 && Slots[0].bIsOccupied && Slots[1].bIsOccupied;
}


// ������ ������Ʈ (���� �Լ�)
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

// ���̶���Ʈ ���� ���� (���õ� ���Ը� �����)
void UQuickSlot::UpdateSlotHighlight()
{
    FLinearColor NormalColor = FLinearColor::White;
    FLinearColor HighlightColor = FLinearColor::Yellow;

    if (Img_Icon1)
        Img_Icon1->SetColorAndOpacity(CurrentSlotIndex == 0 ? HighlightColor : NormalColor);

    if (Img_Icon2)
        Img_Icon2->SetColorAndOpacity(CurrentSlotIndex == 1 ? HighlightColor : NormalColor);
}
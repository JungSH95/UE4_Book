// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// 블루프린트에서 사용 가능하도록 키워드 추가함. UFUNCTION(BlueprintCallable)
	// NextCharacter() 함수는 블루프린트 로직에서도 호출할 수 있다.
	UFUNCTION(BlueprintCallable)
	void NextCharacter(bool bForward = true);

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 CurrentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxIndex;

	// TWeakObjectPtr<액터 타입> 구문을 사용해 상속받은 액터의 목록을 가져올 수 있다.
	TWeakObjectPtr<USkeletalMeshComponent> TargetComponent;
	
	UPROPERTY()
	class UButton* PrevButton;

	UPROPERTY()
	class UButton* NextButton;

	UPROPERTY()
	class UEditableTextBox* TextBox;

	UPROPERTY()
	class UButton* ConfirmButton;

private:
	UFUNCTION()
	void OnPrevClicked();

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnConfirmClicked();
};

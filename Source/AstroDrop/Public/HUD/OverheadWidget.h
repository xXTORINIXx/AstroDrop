#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

class UTextBlock;

UCLASS()
class ASTRODROP_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

protected:

	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* DisplayText;
};
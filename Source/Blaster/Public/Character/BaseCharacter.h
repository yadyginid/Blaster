// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;
    
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void Turn(float Amount);
    void LookUp(float Amount);

private:
    UPROPERTY(VisibleAnywhere, Category = Camera)
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    UCameraComponent* FollowCamera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* OverheadWidget;
};

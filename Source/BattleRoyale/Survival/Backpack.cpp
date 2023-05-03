// Fill out your copyright notice in the Description page of Project Settings.


#include "Backpack.h"

ABackpack::ABackpack()
{
	Capacity = 100;
}

void ABackpack::Attach()
{
	IsPickedUp = true;
}
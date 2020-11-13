#pragma once

namespace Player
{
	extern CPed* pPedSelf;
	extern bool bExitVehicle;
	extern ULONGLONG dwExitTick;

	void Update();
	void AirBreak();
	void AntiStun();
	void FastRotation();
	void FastSprint();
	void InfiniteRun();
	void InfiniteOxygen();
	void NoFall();
	void MegaJump();
	void ChangeSkin();
	void FastRespawn();
	void StopOnExitVeh();
	void BikeSpam();
}
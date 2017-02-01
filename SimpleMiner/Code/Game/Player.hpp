#pragma once
#include "Engine/Core/Audio.hpp"
#include "Engine/Core/Input.hpp"
#include "Game/Chunk.hpp"
#include "Game/JetPack.hpp"
#ifndef PLAYER_HPP
#define PLAYER_HPP

enum GAME_MODE
{
	NO_CLIP,
	FLYING,
	WALKING,
	NUMBER_OF_MODES
};

enum CAMERA_MODE
{
	FIRST_PERSON,
	FROM_BEHIND,
	FIXED_ANGLE_TRACKING,
	NUMBER_OF_CAMERA_MODES
};

class Player
{
public:
	//constructor
	Player(const Vector3& position = Vector3(0.f, 0.f, 0.f), JetPack* jetPack = nullptr, AudioSystem* audio = nullptr);
	~Player();
	//setters
	void ChangePosition(const Vector3&);
	void SetPosition(const Vector3&);
	//getters
	const Vector3 GetPosition() const { return m_position; }; //returns cameraPosition
	const Vector3 GetPositionOfFeet() const; //treat the x and y of the returned value as the CENTER of the player. z value = where the feet are.
	const float GetCameraZDifference() const { return trueCameraZPosition; };
	const float GetCurrentMovementSpeed() const { return m_movementSpeed; };
	const float GetMinMovementSpeed() const { return s_regularMovementSpeed; };
	const float GetMaxMovementSpeed() const { return s_regularMovementSpeed * s_runningMultiplier; };
	const Vector3 GetCollisionModel() const { return m_collisionWidthLengthHeight; };
	const AABB3 GetCollisionBounds() const;
	const int GetInventoryIndex() const { return m_blockPlacementIndex; };
	const GAME_MODE GetCurrentGameMode() const { return m_GAME_MODE; };
	//update and Render
	void Update(float deltaSeconds, bool& consernWorldWithCollision, Vector3& wantedNewPosition, EulerAngles& anglesFacing, const bool& onGround);
	void Render() const;
	const Vector3 GetForwardXYZ() const;
	const Vector3 GetForwardXY() const;
	const Vector3 GetLeftXY() const;
	const EulerAngles getOrientation() const;
	const CAMERA_MODE GetIsFirstPersonMode() const { return m_CameraMode; };
	const void FixAndClampAngles();
	const void zeroOutXYVelocity() { xyVelocity = 0.f; };
	const void zeroOutZVelocity() { zVelocity = 0.f; };
	const float GetRemainingFuelInJetPack();
	const float GetMaxFuelInJetPack();

private:
	const Vector3 Flying(float deltaSeconds, EulerAngles& camAngles);
	const Vector3 Walking(float deltaSeconds, EulerAngles& camAngles);
	void GeneralUpdate();
	void XYPlaneMovement(const float& deltaSeconds, Vector3& newCamPosition);
	void CameraLookAround();

	//variables
public:
	Vector3 m_collisionWidthLengthHeight;
private:
	GAME_MODE m_GAME_MODE;
	Vector3 m_position;
	EulerAngles m_orientation;
	CAMERA_MODE m_CameraMode;
	JetPack* m_jetPack;
	AudioSystem* m_audio;
	SoundID m_rocketSoundID;
	AudioChannelHandle m_rocketSound;
	float m_movementSpeed;
	float trueCameraZPosition;
	bool isWHeldDown;
	bool isRHeldDown;
	bool isPHeldDown;
	bool isBHeldDown;
	bool isF5HeldDown;
	bool m_onGround;
	bool m_activeDebugColliderLines;
	int m_blockPlacementIndex;
	float zVelocity;
	float xyVelocity;
	float secondsInAir;

	//static variables
	static const Vector2 s_mouseYawPitchSensitivity;
	static const IntVector2 s_centralMousePosition;
	static const float s_regularMovementSpeed;
	static const float s_runningMultiplier;
	static const float gravity;
	static const float horizontalAccel;
	static const float zVelocityCap;
	static const float s_delayBeforeJetPackStart;
};
#endif
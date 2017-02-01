#include "Game/Player.hpp"
#include "Engine/Math/ExtendedMath.hpp"

//static variables
const IntVector2 Player::s_centralMousePosition = IntVector2(800, 700);
const Vector2 Player::s_mouseYawPitchSensitivity = Vector2(0.022f, 0.022f);
const float Player::s_regularMovementSpeed = 4.f;
const float Player::s_runningMultiplier = 12.f;
const float Player::gravity = 5.0f; //1.5f = moon gravity.
const float Player::horizontalAccel = 5.0f;
const float Player::zVelocityCap = 50.f;
const float Player::s_delayBeforeJetPackStart = 0.5f;

//constructors
Player::Player(const Vector3& position, JetPack* jetPack, AudioSystem* audio)
: m_position(position),
m_orientation(0.f, 0.f, 0.f),
zVelocity(0.f),
xyVelocity(0.f),
m_movementSpeed(s_regularMovementSpeed),
m_blockPlacementIndex(GRASS_BLOCK),
m_collisionWidthLengthHeight(0.6f, 0.6f, 1.85f),
trueCameraZPosition(m_collisionWidthLengthHeight.z / 1.141975308641975308641975308642f),
m_GAME_MODE(WALKING), //0 == NO_CLIP
m_activeDebugColliderLines(false),
isWHeldDown(false),
isRHeldDown(false),
isPHeldDown(false),
isBHeldDown(false),
isF5HeldDown(false),
m_onGround(true),
m_CameraMode(FIRST_PERSON),
m_jetPack(jetPack),
secondsInAir(0.f),
m_audio(audio)
{
	if (m_audio != nullptr)
	{
		m_rocketSoundID = m_audio->CreateOrGetSound("Data/SoundEffects/ScatterNoise1.mp3", true);
		m_rocketSound = nullptr;
	}
}
Player::~Player()
{
	if (m_jetPack != nullptr)
	{
		delete m_jetPack;
	}
}
//getters
const Vector3 Player::GetForwardXYZ() const
{
	Vector3 forwardXYZ = GetForwardXY();
	//Cos(theta) = XYlength/ 1.f; // aka, Cos(theta) = XYLength.
	float cosOrien = CosDegrees(m_orientation.m_yPitch);
	forwardXYZ.x = forwardXYZ.x * cosOrien;
	forwardXYZ.y = forwardXYZ.y * cosOrien;
	forwardXYZ.z = SinDegrees(-m_orientation.m_yPitch);
	return forwardXYZ;
}
const Vector3 Player::GetForwardXY() const
{
	float cosYaw = CosDegrees(m_orientation.m_zYaw);
	float sinYaw = SinDegrees(m_orientation.m_zYaw);
	return Vector3(cosYaw, sinYaw, 0.f);
}
const Vector3 Player::GetLeftXY() const
{
	Vector3 forwardXY = GetForwardXY();
	return Vector3(-forwardXY.y, forwardXY.x, 0.f);
}
const Vector3 Player::GetPositionOfFeet() const
{
	Vector3 positionOfFeet = m_position;
	positionOfFeet -= Vector3(0.f, 0.f, trueCameraZPosition);
	return positionOfFeet;
}

const AABB3 Player::GetCollisionBounds() const
{
	Vector3 positionOfFeet = GetPositionOfFeet();
	AABB3 bounds = AABB3(Vector3(positionOfFeet.x - (m_collisionWidthLengthHeight.x / 2.f),
		positionOfFeet.y - (m_collisionWidthLengthHeight.y / 2.f),
		positionOfFeet.z),
		Vector3(positionOfFeet.x + (m_collisionWidthLengthHeight.x / 2.f),
		positionOfFeet.y + (m_collisionWidthLengthHeight.y / 2.f),
		positionOfFeet.z + m_collisionWidthLengthHeight.z));
	return bounds;
}

//setters
void Player::ChangePosition(const Vector3& position)
{
	Vector3 newPosition = position;
	newPosition += Vector3(0.5f, 0.5f, trueCameraZPosition);
	m_position = newPosition;
}

void Player::SetPosition(const Vector3& position)
{
	m_position = position;
}
//updates and render
void Player::Render() const
{
	if (m_activeDebugColliderLines == true)
	{
		AABB3 bounds = GetCollisionBounds();
		g_Renderer->DrawLinedRectangularPrism(bounds, 2.0f, g_Renderer->s_White, false);
	}
}
const void Player::FixAndClampAngles()
{
	if (m_orientation.m_yPitch > 89.f)
	{
		m_orientation.m_yPitch = 89.f;
	}
	else if (m_orientation.m_yPitch < -89.f)
	{
		m_orientation.m_yPitch = -89.f;
	}
	if (m_orientation.m_zYaw > 180.f)
	{
		m_orientation.m_zYaw = m_orientation.m_zYaw - 360.f;
	}
	else if (m_orientation.m_zYaw < -180.f)
	{
		m_orientation.m_zYaw = m_orientation.m_zYaw + 360.f;
	}
	if (m_orientation.m_xRoll > 180.f)
	{
		m_orientation.m_xRoll = m_orientation.m_xRoll - 360.f;
	}
	else if (m_orientation.m_xRoll < -180.f)
	{
		m_orientation.m_xRoll = m_orientation.m_xRoll + 360.f;
	}
}
void Player::Update(float deltaSeconds, bool& consernWorldWithCollision, Vector3& wantedNewPosition, EulerAngles& anglesFacing, const bool& onGround)
{
	m_onGround = onGround;
	if (isBHeldDown == false && g_input->isKeyDown('B') == true)
	{
		isBHeldDown = true;
		m_activeDebugColliderLines = !m_activeDebugColliderLines;
	}
	if (isF5HeldDown == false && g_input->isKeyDown(VK_F5) == true)
	{
		isF5HeldDown = true;
		m_CameraMode = (CAMERA_MODE)(m_CameraMode + 1);
		if (m_CameraMode >= NUMBER_OF_CAMERA_MODES)
		{
			m_CameraMode = (CAMERA_MODE)0;
		}
	}
	if (isPHeldDown == false && g_input->isKeyDown('P') == true)
	{
		isPHeldDown = true;
		m_GAME_MODE = (GAME_MODE)(m_GAME_MODE + 1);
		if (m_GAME_MODE >= NUMBER_OF_MODES)
		{
			m_GAME_MODE = NO_CLIP;
		}
	}
	if (m_GAME_MODE == NO_CLIP)
	{
		anglesFacing = EulerAngles();
		wantedNewPosition = Flying(deltaSeconds, anglesFacing);
		consernWorldWithCollision = false;
	}
	else if (m_GAME_MODE == FLYING)
	{
		anglesFacing = EulerAngles();
		wantedNewPosition = Flying(deltaSeconds, anglesFacing);
		consernWorldWithCollision = true;
	}
	else if (m_GAME_MODE == WALKING)
	{
		anglesFacing = EulerAngles();
		wantedNewPosition = Walking(deltaSeconds, anglesFacing);
		consernWorldWithCollision = true;
	}

	if (g_input->isKeyDown('P') == false)
	{
		isPHeldDown = false;
	}
	if (g_input->isKeyDown('B') == false)
	{
		isBHeldDown = false;
	}
	if (g_input->isKeyDown(VK_F5) == false)
	{
		isF5HeldDown = false;
	}
}
const EulerAngles Player::getOrientation() const
{
	return m_orientation;
}

void Player::CameraLookAround()
{
	//Mouse movement controls
	IntVector2 mousePosition;
	g_input->GetFirstCursorPosition(mousePosition);
	IntVector2 cursorDelta = mousePosition - s_centralMousePosition;
	g_input->setCursorPosition(s_centralMousePosition);
	g_input->setCursorShow(false);

	float camRoll = 0.f;
	float camYaw = m_orientation.m_zYaw - ((float)cursorDelta.x * s_mouseYawPitchSensitivity.x); //180.f + degreesIs
	float camPitch = m_orientation.m_yPitch + ((float)cursorDelta.y * s_mouseYawPitchSensitivity.y);

	m_orientation = EulerAngles(camRoll, camPitch, camYaw);
	FixAndClampAngles();
}
void Player::GeneralUpdate()
{

	if (g_input->isKeyDown(VK_SHIFT) == true)
	{
		m_movementSpeed = s_regularMovementSpeed * s_runningMultiplier;
	}
	else
	{
		m_movementSpeed = s_regularMovementSpeed;
	}
	for (size_t i = 0; i <= 9; i++)
	{
		unsigned char number = (unsigned char)('0' + i);
		bool isKeyDown = g_input->isKeyDown(number);
		if (isKeyDown == true)
		{
			m_blockPlacementIndex = GRASS_BLOCK + i;
		}
	}
	if (isWHeldDown == false && (g_input->isKeyDown('W') == true || g_input->IsWheelScrollDown() == true))
	{
		isWHeldDown = true;
		m_blockPlacementIndex -= 1;
		if (m_blockPlacementIndex <= GRASS_BLOCK)
		{
			m_blockPlacementIndex = GRASS_BLOCK + 9;
		}
	}
	if (isRHeldDown == false && (g_input->isKeyDown('R') == true || g_input->IsWheelScrollUp() == true))
	{
		isRHeldDown = true;
		m_blockPlacementIndex += 1;
		if (m_blockPlacementIndex > (GRASS_BLOCK + 9))
		{
			m_blockPlacementIndex = GRASS_BLOCK;
		}
	}
	if (g_input->isKeyDown('R') == false)
	{
		isRHeldDown = false;
	}
	if (g_input->isKeyDown('W') == false)
	{
		isWHeldDown = false;
	}
}
void Player::XYPlaneMovement(const float& deltaSeconds, Vector3& newCamPosition)
{

	Vector3 forwardXY = GetForwardXY();
	if (g_input->isKeyDown('E') == true && g_input->isKeyDown('D') == false)
	{
		xyVelocity += horizontalAccel * deltaSeconds;
		if (xyVelocity > m_movementSpeed)
		{
			xyVelocity = m_movementSpeed;
		}
		newCamPosition += forwardXY * (xyVelocity*deltaSeconds);
	}
	else if (g_input->isKeyDown('D') == true && g_input->isKeyDown('E') == false)
	{
		xyVelocity -= horizontalAccel * deltaSeconds;
		if (xyVelocity > m_movementSpeed)
		{
			xyVelocity = m_movementSpeed;
		}
		newCamPosition += forwardXY * (xyVelocity*deltaSeconds);
		
	}
	else if (g_input->isKeyDown('D') == false && g_input->isKeyDown('E') == false)
	{
		if (xyVelocity != 0.f)
		{
			float neg = xyVelocity / abs(xyVelocity);
			xyVelocity -= horizontalAccel * horizontalAccel * deltaSeconds * neg;
			if ((xyVelocity / abs(xyVelocity)) != neg)
			{
				xyVelocity = 0.f;
			}
		}
		newCamPosition += forwardXY * (xyVelocity * deltaSeconds);
	}

	if (g_input->isKeyDown('S') == true && g_input->isKeyDown('F') == false)
	{
		Vector3 leftXY = GetLeftXY();
		newCamPosition += leftXY * (m_movementSpeed * deltaSeconds);
	}
	else if (g_input->isKeyDown('S') == false && g_input->isKeyDown('F') == true)
	{
		Vector3 leftXY = GetLeftXY();
		newCamPosition -= leftXY * (m_movementSpeed * deltaSeconds);
	}
}
const Vector3 Player::Flying(float deltaSeconds, EulerAngles& camAngles)
{
	Vector3 newCamPosition = m_position;
	CameraLookAround();
	GeneralUpdate();
	if (g_input->isKeyDown(VK_SPACE) == true && g_input->isKeyDown('Z') == false)
	{
		newCamPosition += (Vector3(0.f, 0.f, 1.f) * (m_movementSpeed *deltaSeconds));
	}
	else if (g_input->isKeyDown(VK_SPACE) == false && g_input->isKeyDown('Z') == true)
	{
		newCamPosition -= (Vector3(0.f, 0.f, 1.f) * (m_movementSpeed *deltaSeconds));
	}
	XYPlaneMovement(deltaSeconds, newCamPosition);

	camAngles = m_orientation;
	return newCamPosition;
}

const Vector3 Player::Walking(float deltaSeconds, EulerAngles& camAngles)
{
	Vector3 newCamPosition = m_position;
	CameraLookAround();
	GeneralUpdate();
	if (m_onGround == true)
	{
		zVelocity = 0.f;
		secondsInAir = 0.f;
		if (m_jetPack != nullptr)
		{
			m_jetPack->RecoverFuel(deltaSeconds);
		}
	}
	else if (m_onGround == false)
	{
		zVelocity -= (gravity * deltaSeconds);
		secondsInAir += deltaSeconds;
		if (abs(zVelocity) > zVelocityCap)
		{
			zVelocity = zVelocityCap * (abs(zVelocity) / zVelocity);
		}
	}
	
	if (m_onGround == true && g_input->isKeyDown(VK_SPACE) == true)
	{
		zVelocity = gravity * 0.2f * 3.f;
	}
	else if (m_jetPack != nullptr && secondsInAir >= s_delayBeforeJetPackStart && m_onGround == false && g_input->isKeyDown(VK_SPACE) == true)
	{
		float rocketPropulsion = (m_jetPack->PropelUpwards(deltaSeconds).z);
		zVelocity += rocketPropulsion;
		if (rocketPropulsion > 0.f && m_audio != nullptr && m_rocketSound == nullptr)
		{
			m_rocketSound = m_audio->PlaySound(m_rocketSoundID, 1.0f);
		}
		else if (rocketPropulsion <= 0.f && m_audio != nullptr && m_rocketSound != nullptr)
		{
			m_rocketSound->stop();
			m_rocketSound = nullptr;
		}
	}
	else
	{
		if (m_audio != nullptr)
		{
			if (m_rocketSound != nullptr)
			{
				m_rocketSound->stop();
			}
			m_rocketSound = nullptr;
		}
	}
	newCamPosition.z += (zVelocity * deltaSeconds);
	XYPlaneMovement(deltaSeconds, newCamPosition);
	camAngles = m_orientation;
	return newCamPosition;
}
const float Player::GetRemainingFuelInJetPack()
{
	if (m_jetPack == nullptr)
	{
		return 0.f;
	}
	return m_jetPack->GetRemainingFuel();
}
const float Player::GetMaxFuelInJetPack()
{
	if (m_jetPack == nullptr)
	{
		return 0.f;
	}
	return m_jetPack->GetMaxFuel();
}
#pragma once
#include <deque>
#include "Game/Player.hpp"
#include "Engine/Core/Camera3D.hpp"

#ifndef WORLD_HPP
#define WORLD_HPP

class World
{
public:
	//constructor and destructor
	World(SpriteSheet* spriteSheet = nullptr, AudioSystem* audio = nullptr, const int& viewRadius = 7, const int& farViewRadius = 7, const int& distanceToDelete = 15);
	~World();
	//update and render
	void Update(float deltaSeconds, const bool& focused);
	const void Render() const;
	const void RenderPlayerLine() const;
	const bool IsPlayerInWater() const;
	const Vector3 GetPlayerPosition() const { return m_player->GetPosition(); };
	const GAME_MODE GetGameMode() const { return m_player->GetCurrentGameMode(); };
	void CheckDirtyChunks();
	void SetUpThirdPersonCamera(const float& deltaSeconds, const Vector3& playerPos);
	void SetUpThirdPersonCameraFixedAngle(const float& deltaSeconds, const Vector3& playerPos);

	//block type and sprite sheet related
	void setSpriteSheet(SpriteSheet* spriteSheet);
	const int GetBlockPlacementIndex() const {return m_player->GetInventoryIndex();};
	const IntVector2 GetBlockSideTexCoordsForIndex(const BLOCK_TYPE& index) const { return g_blockInfoContainers[index].sideTextureCoords; };
	const float GetRemainingPlayerRocketFuel() { return m_player->GetRemainingFuelInJetPack(); };
	const float GetMaximumPlayerRocketFuel() { return m_player->GetMaxFuelInJetPack(); };
private:
	void UpdateAudio(float deltaSeconds);
	void RenderChunks() const;
	void SetUpBlockDefinitions();
	void SetUpSongList();
	void UpdateLighting();
	unsigned char getHighestLightValueOfNeighbors(const BlockInfo& blockInfo);
	void getHigherBlockLighting(BlockInfo& blockInfo, unsigned char& idealLighting);
	void dirtyAllNeighbors(const BlockInfo& blockInfo);
	void UpdatingLightingForBlock(Block& block, BlockInfo& blockInfo);
	void markBlockDirty(BlockInfo& blockInfo);
	//Player specific
	void CenterPlayerOnStartChunk();
	const bool determinePositionInAir(const Vector3& position, Block_Hit& hit);
	void determinePlayWalkSound(const bool& testPlayWalk, const Vector3& wantedPosition, const Vector3& tempStart, const bool& inAir);
	void determinePlayLandSound(const bool& inAir);
	void MouseClickControls(const float& deltaSeconds);
	void CameraControls(float deltaSeconds);
	void newPhysics(float dSeconds, Vector3& wantedPosition, Vector3 playerStartPosition, const Vector3& collisionModel);
	void correctivePhysics(Vector3& wantedPosition, const Vector3& collisionModel);

	const float generalComparison(const float& toCompare, const float& toReturn) const;
	const float GetSmallestNonZero(const float& eastPush, const float& westPush, const float& northPush, const float& southPush, const float& upPush, const float& downPush) const;
	void get8PositionsOfPlayerCollision(std::vector<Vector3>& startPositions, std::vector<Vector3>& wantedPositions, Vector3 playerStartPosition, Vector3& wantedPosition, const Vector3& collisionModel);
	void getSingle8PositionsOfPlayerCollision(std::vector<Vector3>& positions, const Vector3& start, const Vector3& collisionModel);
	void getSurrounding12PositionsOfPlayerCollision(std::vector<BlockInfo>& positions, Vector3& start, const Vector3& collisionModel);
	void deleteABlockInFrontOfPlayer(float deltaSeconds);
	void changeABlockTypeInFrontOfPlayer();
	void updateWhereLookingAt();
	//Fast Line Of Sight testing
	const Block_Hit SlowHasLineOfSightChunkTest(const Vector3& start, const Vector3& end, bool& hasLineOfSight);
	const Block_Hit FastHasLineOfSightChunkTest(const Vector3& start, const Vector3& end, bool& hasLineOfSight);
	Chunk* GetChunkForWorldCoords(const Vector3& worldPos) const;
	

	//chunk generation, saving, and loading
	void TestChunksAroundPlayerForGenerationOrLoad();
	void ChunkGeneration(const IntVector2& coordsToGenerate);
	void GenerateNewChunk(const IntVector2& newChunkPosition);
	const bool GenerateChunkFromFile(const IntVector2& loadChunkPosition);
	const bool GetHighestPriorityMissingChunk(IntVector2& outChunkCoords) const;
	IntVector2 GetChunkCoordsForWorldCoords(const Vector3& worldPos) const;
	Chunk* GetFurthestUnneededChunk(IntVector2& chunkLocation);

	std::map<IntVector2, Chunk*> m_activeChunks;
	std::deque<BlockInfo> m_dirtyBlocks;
	SpriteSheet* m_spriteSheet;
	SpriteAnim* m_BlockBreakingAnim;
	IntVector2 m_playerChunk;
	IntVector2 TileCoordsForPlayerTileInRangeLookAt;
	std::vector<Vector3> WhereToDrawLookAt;
	std::vector<Vector3> blockBreakDraw;
	std::vector<AABB2> whereLookAtCoords;
	int m_worldLighting;
	
	bool isLeftClickHeldDown;
	bool isRightClickHeldDown;

	float m_distanceFromPlayerThatCanDestroyPlaceBlocks;

	Block_Hit lastLeftClickHit;
	float m_leftClickHeldForSeconds;

	Player* m_player;
	Camera3D m_mainCamera;
	AudioSystem* m_audio;
	AudioChannelHandle m_currentSound;
	std::vector<SoundID> m_musicIDs;
	AudioChannelHandle m_musicHandle;
	Vector3 testPos;
	bool m_playingSong;
	bool m_justStartedPlaying;
	float m_timeSinceLastPlayedSong;
	int m_songIndexToPlayNext;
	static const float m_timeBeforePlaySong;
	float amountOfSecondsSinceLastWalkSound;
	float amountOfSecondsBetweenWalkSounds;
	float amountOfSecondsBetweenWalkSoundsVariance;
	float amountOfSecondsSinceLastBreakingSound;
	float amountOfSecondsBetweenBreakingSounds;
	
	static const float s_thirdPersonDistance;
	static const EulerAngles s_fixedAngle;


	int m_viewRadius;
	int m_farViewRadius;
	float m_backwardFarViewRadius;
	int m_distanceUntilSaveAndDelete;
public:
	std::string toShowInDebug;
};

#endif
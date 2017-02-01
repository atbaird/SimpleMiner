#include "Game/World.hpp"

//static variables
const float World::s_thirdPersonDistance = 5.0f;
const EulerAngles World::s_fixedAngle(0.f,25.f,45.f); // roll, pitch, yaw
const float World::m_timeBeforePlaySong = 90.f;

//constructor and destructor
World::World(SpriteSheet* spriteSheet, AudioSystem* audio, const int& viewRadius, const int& farViewRadius, const int& distanceToDelete)
: m_viewRadius(viewRadius),
lastLeftClickHit(Block_Hit()),
m_leftClickHeldForSeconds(0.f),
m_mainCamera(Vector3(1.f,-5.f,1.f), EulerAngles(0.f, 0.f, 0.f), true),
m_spriteSheet(spriteSheet),
m_farViewRadius(farViewRadius),
m_backwardFarViewRadius(0.5f),
m_distanceUntilSaveAndDelete(distanceToDelete),
m_playerChunk(0,0),
m_distanceFromPlayerThatCanDestroyPlaceBlocks(7.f),
TileCoordsForPlayerTileInRangeLookAt(5, 13),
isRightClickHeldDown(false),
isLeftClickHeldDown(false),
m_worldLighting(15),
m_audio(audio),
amountOfSecondsSinceLastWalkSound(0.f),
amountOfSecondsBetweenWalkSounds(0.6f),
amountOfSecondsBetweenWalkSoundsVariance(0.2f),
amountOfSecondsSinceLastBreakingSound(0.f),
amountOfSecondsBetweenBreakingSounds(0.3f),
m_songIndexToPlayNext(0),
m_timeSinceLastPlayedSong(0.f),
m_playingSong(false),
m_justStartedPlaying(false)
{
	whereLookAtCoords.insert(whereLookAtCoords.begin() + whereLookAtCoords.size(), spriteSheet->GetTexCoordsForSpriteCoords(TileCoordsForPlayerTileInRangeLookAt));
	m_player = new Player(Vector3(1.f, -5.f, 1.f), new JetPack(), m_audio);
	SetUpBlockDefinitions();
	SetUpSongList();
	CenterPlayerOnStartChunk();
	m_BlockBreakingAnim = new SpriteAnim(spriteSheet, 1.f, SPRITE_ANIM_MODE_PLAY_TO_END,
		spriteSheet->GetSpriteIndexForSpriteCoords(IntVector2(0, 14)), spriteSheet->GetSpriteIndexForSpriteCoords(IntVector2(9, 14)));
	toShowInDebug = "";
}
World::~World()
{
	if (m_BlockBreakingAnim != nullptr)
	{
		delete m_BlockBreakingAnim;
	}
	if (m_player != nullptr)
	{
		delete m_player;
	}
	for (std::map<IntVector2, Chunk*>::iterator it = m_activeChunks.begin(); it != m_activeChunks.end(); it++)
	{
		if (it->second != nullptr)
		{
			it->second->saveChunk();
			delete it->second;
			it->second = nullptr;
		}
	}
}

//update and render
void World::Update(float deltaSeconds, const bool& focused)
{
	UpdateAudio(deltaSeconds);

	if (focused == true)
	{
		TestChunksAroundPlayerForGenerationOrLoad();
		CameraControls(deltaSeconds);
		Vector3 playerPos = m_player->GetPosition();
		CAMERA_MODE mode = m_player->GetIsFirstPersonMode();
		m_playerChunk = GetChunkCoordsForWorldCoords(m_player->GetPosition());
		if (mode == FIRST_PERSON)
		{
			m_mainCamera.CameraUpdate(deltaSeconds, playerPos, m_player->getOrientation());
		}
		else if (mode == FROM_BEHIND)
		{
			SetUpThirdPersonCamera(deltaSeconds, playerPos);
		}
		else if (mode == FIXED_ANGLE_TRACKING)
		{
			SetUpThirdPersonCameraFixedAngle(deltaSeconds, playerPos);
		}
		m_mainCamera.FixAndClampAngles();
		updateWhereLookingAt();
		UpdateLighting();
		CheckDirtyChunks();
	}

}

void World::UpdateAudio(float deltaSeconds)
{
	m_audio->Update(deltaSeconds);
	if (m_musicIDs.size() > 0)
	{
		if (m_playingSong == false)
		{
			m_justStartedPlaying = true;
			m_timeSinceLastPlayedSong += deltaSeconds;
			if (m_timeSinceLastPlayedSong >= m_timeBeforePlaySong)
			{
				m_musicHandle = m_audio->PlaySound(m_musicIDs[m_songIndexToPlayNext], 1.0f);

				m_timeSinceLastPlayedSong = 0.f;
				m_playingSong = true;
				m_songIndexToPlayNext++;
				if ((size_t)m_songIndexToPlayNext >= m_musicIDs.size())
				{
					m_songIndexToPlayNext = 0;
				}
			}
		}
		else
		{
			if (m_justStartedPlaying == false && m_musicHandle != nullptr && m_audio->GetIsPlaying(m_musicHandle) == false)
			{
				m_playingSong = false;
				m_musicHandle = nullptr;
			}
			if (m_audio->GetIsPlaying(m_musicHandle) == true && m_justStartedPlaying == true)
			{
				m_justStartedPlaying = false;
			}
		}
	}

}
void World::SetUpThirdPersonCamera(const float& deltaSeconds, const Vector3& playerPos)
{
	Vector3 backwardXYZ = m_player->GetForwardXYZ() * -1.f;
	Vector3 wantedEndCameraPosition = backwardXYZ * s_thirdPersonDistance;
	bool hasLineOfSight = false;
	Block_Hit hit = SlowHasLineOfSightChunkTest(playerPos, playerPos + wantedEndCameraPosition, hasLineOfSight);
	if (hasLineOfSight == false)
	{
		Vector3 hitPos = hit.m_worldPosHit;
		float percentage = (playerPos - hitPos).GetLength() / (wantedEndCameraPosition).GetLength();
		wantedEndCameraPosition = wantedEndCameraPosition * percentage;
	}
	m_mainCamera.CameraUpdate(deltaSeconds, playerPos + wantedEndCameraPosition, m_player->getOrientation());
}
void World::SetUpThirdPersonCameraFixedAngle(const float& deltaSeconds, const Vector3& playerPos)
{
	m_mainCamera.ChangePosition(playerPos);
	m_mainCamera.ChangeOrientation(s_fixedAngle);
	Vector3 backwards = m_mainCamera.GetForwardXYZ() * -1.f;
	Vector3 wantedEndCameraPosition = backwards * s_thirdPersonDistance;
	bool hasLineOfSight = false;
	Block_Hit hit = SlowHasLineOfSightChunkTest(playerPos, playerPos + wantedEndCameraPosition, hasLineOfSight);
	if (hasLineOfSight == false)
	{
		Vector3 hitPos = hit.m_worldPosHit;
		float percentage = (playerPos - hitPos).GetLength() / (wantedEndCameraPosition).GetLength();
		wantedEndCameraPosition = wantedEndCameraPosition * percentage;
	}
	m_mainCamera.CameraUpdate(deltaSeconds, playerPos + wantedEndCameraPosition, s_fixedAngle);
}
void World::CheckDirtyChunks()
{
	for (std::map<IntVector2, Chunk*>::const_iterator it = m_activeChunks.begin(); it != m_activeChunks.end(); it++)
	{
		if (it->second != nullptr)
		{
			it->second->UpdateQuadVertices(m_spriteSheet);
		}
	}
}

const void World::RenderPlayerLine() const
{
	//g_Renderer->DepthTestingTrueFalse(false);
	Vector3 playerPosition = m_player->GetPosition();
	Vector3 playerForward = m_player->GetForwardXYZ();
	playerForward = playerPosition + (playerForward * m_distanceFromPlayerThatCanDestroyPlaceBlocks);
	g_Renderer->DrawLine(playerPosition, (playerForward), 10.f, Rgba(1.0f, 0.0f, 1.0f, 1.f));
	//g_Renderer->DepthTestingTrueFalse(true);
}

const bool World::IsPlayerInWater() const
{
	Vector3 playerPos = m_player->GetPosition();
	Chunk* playerChunk = GetChunkForWorldCoords(playerPos);
	if (playerChunk == nullptr
		|| playerPos.z >= (Chunk::BLOCKS_WIDE_Z)
		|| playerPos.z < 0.f)
	{
		return false;
	}
	BLOCK_TYPE type = playerChunk->getBlockTypeAtWorldCoords(playerPos);
	return (type == WATER_BLOCK);
}
const void World::Render() const
{
	m_mainCamera.Render();
	m_player->Render();
	std::vector<AABB2> temp;
	temp.insert(temp.begin() + temp.size(), m_BlockBreakingAnim->GetCurrentTexCoords());
	g_Renderer->DrawSquaresIn3D(blockBreakDraw, temp, m_BlockBreakingAnim->GetTexture());
	g_Renderer->DrawSquaresIn3D(WhereToDrawLookAt, whereLookAtCoords, m_spriteSheet->getSpriteSheetTexture());
	g_Renderer->DrawLine(m_player->GetPosition(), testPos, 5.0f);
	CAMERA_MODE mode = m_player->GetIsFirstPersonMode();
	if (mode == FROM_BEHIND || mode == FIXED_ANGLE_TRACKING)
	{
		RenderPlayerLine();
	}

	RenderChunks();
}

void World::RenderChunks() const
{

	Vector3 camForward = m_mainCamera.GetForwardXYZ();
	Vector3 camPos = m_mainCamera.m_position;
	Vector2 ChunkForward = Vector2(camForward.x, camForward.y);

	std::map<float, Chunk*> toRender;

	for (std::map<IntVector2, Chunk*>::const_iterator it = m_activeChunks.begin(); it != m_activeChunks.end(); it++)
	{
		float distanceFromCenter = (it->second->GetChunkCenter() - camPos).GetLength();
		Vector3 minPosition = it->second->getStartPosition() - camPos;
		Vector3 minPos2 = Vector3(minPosition.x, minPosition.y + Chunk::BLOCKS_WIDE_Y, minPosition.z);
		Vector3 minPos3 = Vector3(minPosition.x + Chunk::BLOCKS_WIDE_X, minPosition.y, minPosition.z);
		Vector3 minPos4 = Vector3(minPosition.x + Chunk::BLOCKS_WIDE_X, minPosition.y + Chunk::BLOCKS_WIDE_Y, minPosition.z);

		Vector3 maxPosition = it->second->getEndPosition() - camPos;
		Vector3 maxPos2 = Vector3(maxPosition.x, maxPosition.y - Chunk::BLOCKS_WIDE_Y, maxPosition.z);
		Vector3 maxPos3 = Vector3(maxPosition.x - Chunk::BLOCKS_WIDE_X, maxPosition.y, maxPosition.z);
		Vector3 maxPos4 = Vector3(maxPosition.x - Chunk::BLOCKS_WIDE_X, maxPosition.y - Chunk::BLOCKS_WIDE_Y, maxPosition.z);

		float dirMin1 = minPosition.dotProduct(camForward);
		float dirMin2 = minPos2.dotProduct(camForward);
		float dirMin3 = minPos3.dotProduct(camForward);
		float dirMin4 = minPos4.dotProduct(camForward);
		float dirMax1 = maxPosition.dotProduct(camForward);
		float dirMax2 = maxPos2.dotProduct(camForward);
		float dirMax3 = maxPos3.dotProduct(camForward);
		float dirMax4 = maxPos4.dotProduct(camForward);

		IntVector2 displacement = it->first - m_playerChunk;

		if ((dirMin1 >= 0.f || dirMin2 >= 0.f || dirMin3 >= 0.f || dirMin4 >= 0.f || dirMax1 >= 0.f || dirMax2 >= 0.f || dirMax3 >= 0.f || dirMax4 >= 0.f)
			&& displacement.getLength() <= m_farViewRadius)
		{
			Chunk* chunk = it->second;
			if (chunk != nullptr)
			{
				toRender.insert(std::pair<float, Chunk*>(distanceFromCenter, chunk));
			}
		}
	}
	for (std::map<float, Chunk*>::const_iterator it = --toRender.end(); it != --toRender.begin(); --it)
	{
		if (it->second != nullptr)
		{
			it->second->Render(m_spriteSheet);
		}
	}
}

//block type and sprite sheet related
void World::setSpriteSheet(SpriteSheet* spriteSheet)
{
	m_spriteSheet = nullptr;
	m_spriteSheet = spriteSheet;
}

void World::SetUpBlockDefinitions()
{
	g_blockInfoContainers.resize(NUM_OF_BLOCK_TYPES);

	SoundID grassWalk = (SoundID)-1;
	SoundID grassPlaced = (SoundID)-1;
	SoundID grassBreaking = (SoundID)-1;
	SoundID grassBreak = (SoundID)-1;

	SoundID stoneWalk = (SoundID)-1;
	SoundID stonePlaced = (SoundID)-1;
	SoundID stoneBreaking = (SoundID)-1;
	SoundID stoneBreak = (SoundID)-1;

	SoundID sandWalk = (SoundID)-1;
	SoundID sandPlaced = (SoundID)-1;
	SoundID sandBreaking = (SoundID)-1;
	SoundID sandBreak = (SoundID)-1;

	SoundID waterWalk = (SoundID)-1;
	SoundID waterPlaced = (SoundID)-1;
	SoundID waterBreaking = (SoundID)-1;
	SoundID waterBreak = (SoundID)-1;
	if (m_audio != nullptr)
	{
		sandWalk = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-SandL1.wav", false);
		sandPlaced = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-SandL2.wav", false);
		sandBreaking = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-SandL3.wav", false);
		sandBreak = sandPlaced;

		grassWalk = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-SandR1.wav", false);
		grassPlaced = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-SandR2.wav", false);
		grassBreaking = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-SandR3.wav", false);
		grassBreak = grassPlaced;

		stoneWalk = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-StoneR1.wav", false);
		stonePlaced = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-StoneR2.wav", false);
		stoneBreaking = m_audio->CreateOrGetSound("Data/SoundEffects/Fantozzi-StoneR3.wav", false);
		stoneBreak = stonePlaced;
	}

	g_blockInfoContainers[AIR_BLOCK] = BlockTextureContainer(IntVector2(0, 0), IntVector2(0, 0), IntVector2(0, 0), false, false, 0, false, 0.f, (SoundID)-1, (SoundID)-1, (SoundID)-1, (SoundID)-1);

	g_blockInfoContainers[WATER_BLOCK] = BlockTextureContainer(IntVector2(15, 11), IntVector2(15, 11), IntVector2(15, 11), false, false, 0, false, 0.f, waterWalk, waterPlaced, waterBreaking, waterBreak);

	
	g_blockInfoContainers[GRASS_BLOCK] = BlockTextureContainer(IntVector2(9, 8), IntVector2(8, 8), IntVector2(7, 8), true, false, 0, true, 1.f, grassWalk, grassPlaced, grassBreaking, grassBreak);

	g_blockInfoContainers[DIRT_BLOCK] = BlockTextureContainer(IntVector2(7, 8), IntVector2(7, 8), IntVector2(7, 8), true, false, 0, true, 1.f, grassWalk, grassPlaced, grassBreaking, grassBreak);

	g_blockInfoContainers[COBBLE_STONE_BLOCK] = BlockTextureContainer(IntVector2(5, 10), IntVector2(5, 10), IntVector2(5, 10), true, false, 0, true, 2.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[STONE_BLOCK] = BlockTextureContainer(IntVector2(2, 10), IntVector2(2, 10), IntVector2(2, 10), true, false, 0, true, 2.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[SAND_BLOCK] = BlockTextureContainer(IntVector2(1, 8), IntVector2(1, 8), IntVector2(1, 8), true, false, 0, true, 1.f, sandWalk, sandPlaced, sandBreaking, sandBreak);

	g_blockInfoContainers[SAND_STONE_BLOCK] = BlockTextureContainer(IntVector2(2, 8), IntVector2(2, 8), IntVector2(2, 8), true, false, 0, true, 2.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[BRICK_BLOCK] = BlockTextureContainer(IntVector2(3, 11), IntVector2(3, 11), IntVector2(3, 11), true, false, 0, true, 3.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[GOLD_BRICK_BLOCK] = BlockTextureContainer(IntVector2(6, 11), IntVector2(6, 11), IntVector2(6, 11), true, false, 0, true, 4.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[GLOW_STONE_BLOCK] = BlockTextureContainer(IntVector2(4, 11), IntVector2(4, 11), IntVector2(4, 11), true, true, 12, true, 3.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[OBSIDIAN_BLOCK] = BlockTextureContainer(IntVector2(9, 11), IntVector2(9, 11), IntVector2(9, 11), true, false, 0, true, 5.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[HORROR_STONE_BLOCK] = BlockTextureContainer(IntVector2(0, 12), IntVector2(0, 12), IntVector2(0, 12), true, false, 0, true, 5.f, stoneWalk, stonePlaced, stoneBreaking, stoneBreak);

	g_blockInfoContainers[LAVA_BLOCK] = BlockTextureContainer(IntVector2(2, 12), IntVector2(2, 12), IntVector2(2, 12), true, true, 8, true, 5.f, waterWalk, waterPlaced, waterBreaking, waterBreak);
}

void World::SetUpSongList()
{
	m_musicIDs = std::vector<SoundID>();
	m_musicHandle = nullptr;

	m_musicIDs.push_back(m_audio->CreateOrGetSound("Data/Music/caveexplorer_hq.mp3", false));
	m_musicIDs.push_back(m_audio->CreateOrGetSound("Data/Music/Neostead - Breathing.mp3", false));

}
void World::UpdateLighting()
{
	while (m_dirtyBlocks.empty() == false)
	{
		BlockInfo dirtyBlock = m_dirtyBlocks.front();
		m_dirtyBlocks.pop_front();

		Block* block = dirtyBlock.GetBlock();
		if (block == nullptr)
		{
			continue; //continue; = skip to next loop iteration.
		}
		block->setDirtyBit(false);
		UpdatingLightingForBlock(*block, dirtyBlock);
	}
}
void World::markBlockDirty(BlockInfo& blockInfo)
{
	if (blockInfo.m_chunk != nullptr)
	{
		Block* block = blockInfo.GetBlock();
		block->setDirtyBit(true);
		blockInfo.m_chunk->m_chunkDirty = true;
		m_dirtyBlocks.push_back(blockInfo);
	}
}
void World::getHigherBlockLighting(BlockInfo& blockInfo, unsigned char& idealLighting)
{
	if (blockInfo.m_chunk != nullptr)
	{
		Block* block = blockInfo.GetBlock();
		bool isOpaque = block->isOpaqueSet();
		bool isLightSource = g_blockInfoContainers[block->getBlockType()].m_lightSource;
		unsigned char blockInfoLighting = blockInfo.GetBlock()->getLightValue();

		if (blockInfoLighting > idealLighting && (isOpaque == false || isLightSource == true))
		{
			idealLighting = blockInfoLighting;
		}
	}
}
unsigned char World::getHighestLightValueOfNeighbors(const BlockInfo& blockInfo)
{
	unsigned char idealLighting = 0;
	BlockInfo up = blockInfo.GetNeighborAbove();
	BlockInfo down = blockInfo.GetNeighborBelow();
	BlockInfo south = blockInfo.GetNeighborSouth();
	BlockInfo north = blockInfo.GetNeighborNorth();
	BlockInfo west = blockInfo.GetNeighborWest();
	BlockInfo east = blockInfo.GetNeighborEast();

	getHigherBlockLighting(up, idealLighting);
	getHigherBlockLighting(down, idealLighting);
	getHigherBlockLighting(south, idealLighting);
	getHigherBlockLighting(north, idealLighting);
	getHigherBlockLighting(west, idealLighting);
	getHigherBlockLighting(east, idealLighting);
	return idealLighting;
}

void World::dirtyAllNeighbors(const BlockInfo& blockInfo)
{
	BlockInfo up = blockInfo.GetNeighborAbove();
	BlockInfo down = blockInfo.GetNeighborBelow();
	BlockInfo south = blockInfo.GetNeighborSouth();
	BlockInfo north = blockInfo.GetNeighborNorth();
	BlockInfo west = blockInfo.GetNeighborWest();
	BlockInfo east = blockInfo.GetNeighborEast();

	markBlockDirty(up);
	markBlockDirty(down);
	markBlockDirty(south);
	markBlockDirty(north);
	markBlockDirty(west);
	markBlockDirty(east);
}
void World::UpdatingLightingForBlock(Block& block, BlockInfo& blockInfo)
{
	//test lighting at correct value.
	//if it is, just remove it from the list.
	//else correct it, add neighbors to dirtyList.
	unsigned char idealLighting = (unsigned char)g_blockInfoContainers[block.getBlockType()].m_lightRadius;
	bool isBlockOpague = block.isOpaqueSet();

	if (block.isSkySet() == true && (unsigned char)m_worldLighting > idealLighting)
	{
		idealLighting = (unsigned char)m_worldLighting;
	}
	unsigned char currentLighting = block.getLightValue();
	
	unsigned char highestNeighborLighting = getHighestLightValueOfNeighbors(blockInfo);
	if (highestNeighborLighting > (idealLighting + 1))
	{
		idealLighting = highestNeighborLighting - 1;
	}
	if (isBlockOpague == true)
	{
		idealLighting = (unsigned char)g_blockInfoContainers[block.getBlockType()].m_lightRadius;
	}
	if (idealLighting != currentLighting)
	{
		blockInfo.GetBlock()->setLightValue(idealLighting);
		dirtyAllNeighbors(blockInfo);
	}


}
//player specific
void World::CenterPlayerOnStartChunk()
{
	IntVector2 startChunkPosition = IntVector2(0, 0);
	std::map<IntVector2, Chunk*>::iterator it = m_activeChunks.find(startChunkPosition);
	if (it == m_activeChunks.end())
	{
		ChunkGeneration(startChunkPosition);
		CenterPlayerOnStartChunk();
	}
	else
	{
		if (it->second != nullptr)
		{
			Vector3 position = it->second->GetChunkCenterGroundLevel();
			m_player->ChangePosition(position);
			m_mainCamera.ChangePosition(m_player->GetPosition());
		}
	}
}
void World::determinePlayWalkSound(const bool& testPlayWalk, const Vector3& wantedPosition, const Vector3& tempStart, const bool& inAir)
{
	if (testPlayWalk == true)
	{
		float minMovement = (m_player->GetMinMovementSpeed());
		float maxMovement = (m_player->GetMaxMovementSpeed());
		float speed = ((m_player->GetCurrentMovementSpeed() - minMovement) / (maxMovement - minMovement)) * 0.2f;
		float moveSpeedWalkSoundVariance = amountOfSecondsBetweenWalkSounds - speed;
		if (amountOfSecondsSinceLastWalkSound >= moveSpeedWalkSoundVariance)
		{
			amountOfSecondsSinceLastWalkSound = 0.f;
			if (wantedPosition.z >= 1.f && wantedPosition.z <= Chunk::BLOCKS_WIDE_Z)
			{
				BLOCK_TYPE types[] = {
					GetChunkForWorldCoords(Vector3(wantedPosition.x - 0.5f, wantedPosition.y, wantedPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(wantedPosition.x - 0.5f, wantedPosition.y, wantedPosition.z - 1.f)),
					GetChunkForWorldCoords(Vector3(wantedPosition.x + 0.5f, wantedPosition.y, wantedPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(wantedPosition.x + 0.5f, wantedPosition.y, wantedPosition.z - 1.f)),
					GetChunkForWorldCoords(Vector3(wantedPosition.x, wantedPosition.y - 0.5f, wantedPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(wantedPosition.x, wantedPosition.y - 0.5f, wantedPosition.z - 1.f)),
					GetChunkForWorldCoords(Vector3(wantedPosition.x, wantedPosition.y + 0.5f, wantedPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(wantedPosition.x, wantedPosition.y + 0.5f, wantedPosition.z - 1.f))
				};
				int ranIndex = rand() % 4;

				if ((tempStart.x - wantedPosition.x != 0.f || tempStart.y - wantedPosition.y != 0.f)
					&& g_blockInfoContainers[types[ranIndex]].m_walk != -1
					&& (!inAir) == true)
				{
					m_audio->PlaySound(g_blockInfoContainers[types[ranIndex]].m_walk, 0.5f);
				}
			}
		}
	}
	else
	{
		amountOfSecondsSinceLastWalkSound = 0.f;
	}
}
void World::determinePlayLandSound(const bool& inAir)
{
	bool nowInAir = true;
	Block_Hit hit;
	Vector3 playerStartPosition = m_player->GetPositionOfFeet();
	nowInAir = determinePositionInAir(playerStartPosition, hit);
	if (nowInAir == false && inAir == true)
	{
		BLOCK_TYPE types[] = {
			GetChunkForWorldCoords(Vector3(playerStartPosition.x - 0.5f, playerStartPosition.y, playerStartPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(playerStartPosition.x - 0.5f, playerStartPosition.y, playerStartPosition.z - 1.f)),
			GetChunkForWorldCoords(Vector3(playerStartPosition.x + 0.5f, playerStartPosition.y, playerStartPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(playerStartPosition.x + 0.5f, playerStartPosition.y, playerStartPosition.z - 1.f)),
			GetChunkForWorldCoords(Vector3(playerStartPosition.x, playerStartPosition.y - 0.5f, playerStartPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(playerStartPosition.x, playerStartPosition.y - 0.5f, playerStartPosition.z - 1.f)),
			GetChunkForWorldCoords(Vector3(playerStartPosition.x, playerStartPosition.y + 0.5f, playerStartPosition.z - 1.f))->getBlockTypeAtWorldCoords(Vector3(playerStartPosition.x, playerStartPosition.y + 0.5f, playerStartPosition.z - 1.f))
		};
		int ranIndex = rand() % 4;

		if (g_blockInfoContainers[types[ranIndex]].m_walk != -1)
		{
			m_audio->PlaySound(g_blockInfoContainers[types[ranIndex]].m_walk, 0.5f);
		}
	}
}
void World::MouseClickControls(const float& deltaSeconds)
{

	//if (isLeftClickHeldDown == false && g_input->isKeyDown(VK_LBUTTON) == true && g_input->isKeyDown(VK_RBUTTON) == false)
	if (g_input->isKeyDown(VK_LBUTTON) == true && g_input->isKeyDown(VK_RBUTTON) == false)
	{
		deleteABlockInFrontOfPlayer(deltaSeconds);
		isLeftClickHeldDown = true;
	}
	else
	{
		m_leftClickHeldForSeconds = 0.f;
		lastLeftClickHit = Block_Hit();
		m_BlockBreakingAnim->Reset();
	}
	if (isRightClickHeldDown == false && g_input->isKeyDown(VK_RBUTTON) == true && g_input->isKeyDown(VK_LBUTTON) == false)
	{
		changeABlockTypeInFrontOfPlayer();
		isRightClickHeldDown = true;
	}


	if (g_input->isKeyDown(VK_RBUTTON) == false)
	{
		isRightClickHeldDown = false;
	}
	if (g_input->isKeyDown(VK_LBUTTON) == false)
	{
		isLeftClickHeldDown = false;
		m_leftClickHeldForSeconds = 0.f;
	}
}
const bool World::determinePositionInAir(const Vector3& position, Block_Hit& hit)
{
	Vector3 endGroundFallTest = Vector3(0.0f, 0.0f, -0.1f);
	endGroundFallTest += position;
	bool inAir = true;
	hit = SlowHasLineOfSightChunkTest(position, endGroundFallTest, inAir);
	return inAir;
}
void World::CameraControls(float deltaSeconds)
{
	bool consernWithNewPlayerPosition = false;
	Vector3 wantedPosition;
	EulerAngles facing;
	bool inAir = true;
	Vector3 playerStartPosition = m_player->GetPositionOfFeet();
	Block_Hit hitGround;
	inAir = determinePositionInAir(playerStartPosition, hitGround);
	Vector3 tempStart = playerStartPosition;
	if (inAir == false && GetChunkForWorldCoords(hitGround.m_worldPosHit) != nullptr)
	{
		Vector3 whereHit = hitGround.m_worldPosHit;
		if ((playerStartPosition - whereHit).GetLength() > 1.1f)
		{
			inAir = true;
		}
	}

	m_player->Update(deltaSeconds, consernWithNewPlayerPosition, wantedPosition, facing, (!inAir));
	bool testPlayWalk = false;

	if (consernWithNewPlayerPosition == true)
	{
		float camZ = m_player->GetCameraZDifference();
		playerStartPosition = m_player->GetPositionOfFeet();
		Vector3 positionTemp = playerStartPosition;
		wantedPosition -= Vector3(0.f, 0.f, camZ);
		Vector3 collisionModel = m_player->GetCollisionModel();
		if ((wantedPosition - playerStartPosition).GetLength() > 0.f)
		{
			testPlayWalk = true;
		}
		//float dSeconds = deltaSeconds;
		correctivePhysics(wantedPosition, collisionModel);
		//newPhysics(dSeconds, wantedPosition, playerStartPosition, collisionModel);
		determinePlayWalkSound(testPlayWalk, wantedPosition, tempStart, inAir);
		wantedPosition += Vector3(0.f, 0.f, camZ);
		amountOfSecondsSinceLastWalkSound += deltaSeconds;
	}
	m_player->SetPosition(wantedPosition);
	determinePlayLandSound(inAir);
	MouseClickControls(deltaSeconds);
}
void World::newPhysics(float dSeconds, Vector3& wantedPosition, Vector3 playerStartPosition, const Vector3& collisionModel)
{
	//playerStartPosition and WantedPosition already start at middle bottom of the collision model.
	std::vector<Vector3> startPositions;
	std::vector<Vector3> wantedPositions;
	get8PositionsOfPlayerCollision(startPositions, wantedPositions, playerStartPosition, wantedPosition, collisionModel);
	
	
	while (dSeconds > 0.0f)
	{
		if (dSeconds < 0.0000001f || wantedPosition == playerStartPosition)
		{
			//if dSeconds is really really small or wantedPosition == playerStartPosition, then lets just quit now.
			wantedPosition = playerStartPosition;
			break;
		}
		std::vector<Block_Hit> blocksHit;
		std::vector<bool> hasLineOfSights;

		//Run all 8 raycast tests.

		//check if any of them equals false, find one that's closest.
		int index = -1;
		for (size_t i = 0; i < startPositions.size(); i++)
		{
			bool tempLineOfSight = true;
			blocksHit.push_back(SlowHasLineOfSightChunkTest(startPositions[i], wantedPositions[i], tempLineOfSight));
			hasLineOfSights.push_back(tempLineOfSight);
			if (hasLineOfSights[i] == false)
			{
				if (index == -1)
				{
					index = i;
				}
				else
				{
					//blocksHit[index];
					//blocksHit[i];
					Vector3 indexDist = startPositions[index] - blocksHit[index].m_worldPosHit;
					Vector3 iDist = startPositions[i] - blocksHit[i].m_worldPosHit;
					if (iDist.GetLength() < indexDist.GetLength())
					{
						index = i;
					}
				}
			}
		}
		if (index == -1)
		{
			//if index == -1, then it's safe to let entity move to the wanted position.
			break;
		}
		if (index != -1)
		{
			//so as to simply true meaning, pulled out the world positions of the hit block and the last known air block.
			Vector3 posHit = blocksHit[index].m_worldPosHit;
			Vector3 airBlock = blocksHit[index].m_worldPosBeforeHit;
			Vector3 hitVsAir = posHit - airBlock; // hit vs air is used to tell which direction we hit the block from.
			if (hitVsAir.x == 0.f && hitVsAir.y == 0.f && hitVsAir.z == 0.f)
			{
				//failure of preventative physics! already in solid block!
				wantedPosition = playerStartPosition;
				break;
			}

			Vector3 wantedVsStart = wantedPosition - playerStartPosition;
			Vector3 wantedVsStartingPosition = wantedPositions[index] - startPositions[index];
			Vector3 airVsStartingPosition = airBlock - startPositions[index];
			Vector3 rndPosHit = posHit;
			rndPosHit.roundAll();
			Vector3 hitVsStartingPosition = rndPosHit - startPositions[index];

			float percentage = (airVsStartingPosition.GetLength() / wantedVsStartingPosition.GetLength()); //how far of our end goal did we go before hitting a block?

			//check z first, because falling constantly occurs.
			if (hitVsAir.z != 0.f)
			{
				if (hitVsAir.z <= 0.f && wantedVsStartingPosition.z <= 0.f)
				{
					playerStartPosition.x = playerStartPosition.x + (((wantedVsStart.x) * percentage));
					playerStartPosition.y = playerStartPosition.y + (((wantedVsStart.y) * percentage));
					playerStartPosition.z = playerStartPosition.z + (((wantedVsStart.z) * percentage) + 0.02f);// +1.07f;
					wantedPosition.z = playerStartPosition.z;
					m_player->zeroOutZVelocity();
					for (size_t i = 0; i < wantedPositions.size(); i++)
					{
						startPositions[i].x = startPositions[i].x + (((wantedPositions[i].x - startPositions[i].x) * percentage));// + 1.07f;
						startPositions[i].y = startPositions[i].y + (((wantedPositions[i].y - startPositions[i].y) * percentage));
						startPositions[i].z = startPositions[i].z + (((wantedPositions[i].z - startPositions[i].z) * percentage) + 0.02f);
						wantedPositions[i].z = startPositions[i].z;
					}
				}
				else if (hitVsAir.z > 0.f && wantedVsStartingPosition.z > 0.f)
				{
					playerStartPosition.x = playerStartPosition.x + (((wantedVsStart.x) * percentage));
					playerStartPosition.y = playerStartPosition.y + (((wantedVsStart.y) * percentage));
					playerStartPosition.z = playerStartPosition.z + (((wantedVsStart.z) * percentage) - (wantedVsStart.z * 0.01f));// +1.07f;
					wantedPosition.z = playerStartPosition.z;
					m_player->zeroOutZVelocity();
					for (size_t i = 0; i < wantedPositions.size(); i++)
					{
						startPositions[i].x = startPositions[i].x + (((wantedPositions[i].x - startPositions[i].x) * percentage));// + 1.07f;
						startPositions[i].y = startPositions[i].y + (((wantedPositions[i].y - startPositions[i].y) * percentage));
						startPositions[i].z = startPositions[i].z + (((wantedPositions[i].z - startPositions[i].z) * percentage) - 0.02f);
						wantedPositions[i].z = startPositions[i].z;
					}
				}
				dSeconds -= (dSeconds * percentage);
				percentage = 0.0f;
			}
			if (hitVsAir.x != 0.f)
			{
				if (hitVsAir.x <= 0.f && wantedVsStartingPosition.x <= 0.f)
				{
					playerStartPosition.x = playerStartPosition.x + (((wantedVsStart.x) * percentage) + 0.02f);// +1.0f + (collisionModel.m_x / 2.f));// + 1.07f;
					playerStartPosition.y = playerStartPosition.y + (((wantedVsStart.y) * percentage));
					playerStartPosition.z = playerStartPosition.z + (((wantedVsStart.z) * percentage));
					wantedPosition.x = playerStartPosition.x;
					m_player->zeroOutXYVelocity();
					for (size_t i = 0; i < wantedPositions.size(); i++)
					{
						startPositions[i].x = startPositions[i].x + (((wantedPositions[i].x - startPositions[i].x) * percentage) + 0.02f);// +(collisionModel.m_x / 2.f));// + 1.07f;
						startPositions[i].y = startPositions[i].y + (((wantedPositions[i].y - startPositions[i].y) * percentage));
						startPositions[i].z = startPositions[i].z + (((wantedPositions[i].z - startPositions[i].z) * percentage));
						wantedPositions[i].x = startPositions[i].x;
					}
				}
				else if (hitVsAir.x > 0.f && wantedVsStartingPosition.x > 0.f)
				{
					playerStartPosition.x = playerStartPosition.x + (((wantedVsStart.x) * percentage) - 0.02f);// -(collisionModel.m_x / 2.f));// -0.07f;
					playerStartPosition.y = playerStartPosition.y + (((wantedVsStart.y) * percentage));
					playerStartPosition.z = playerStartPosition.z + (((wantedVsStart.z) * percentage));
					wantedPosition.x = playerStartPosition.x;
					m_player->zeroOutXYVelocity();
					for (size_t i = 0; i < wantedPositions.size(); i++)
					{
						startPositions[i].x = startPositions[i].x + (((wantedPositions[i].x - startPositions[i].x) * percentage) - 0.02f);// -(collisionModel.m_x / 2.f));// -0.07f;
						startPositions[i].y = startPositions[i].y + (((wantedPositions[i].y - startPositions[i].y) * percentage));
						startPositions[i].z = startPositions[i].z + (((wantedPositions[i].z - startPositions[i].z) * percentage));
						wantedPositions[i].x = startPositions[i].x;
					}
				}
				dSeconds -= (dSeconds * percentage);
				percentage = 0.0f;
			}
			if (hitVsAir.y != 0.f)
			{	
				if (hitVsAir.y <= 0.f && wantedVsStartingPosition.y <= 0.f)
				{
					playerStartPosition.x = playerStartPosition.x - (((wantedVsStart.x)* percentage));
					playerStartPosition.y = playerStartPosition.y - (((wantedVsStart.y) * percentage) + 0.02f);// +(collisionModel.m_x / 2.f));// +1.07f;
					playerStartPosition.z = playerStartPosition.z + (((wantedVsStart.z)* percentage));
					wantedPosition.y = playerStartPosition.y;
					m_player->zeroOutXYVelocity();
					for (size_t i = 0; i < wantedPositions.size(); i++)
					{
						startPositions[i].x = startPositions[i].x - (((wantedPositions[i].x - startPositions[i].x) * percentage));// + 1.07f;
						startPositions[i].y = startPositions[i].y - (((wantedPositions[i].y - startPositions[i].y) * percentage) + 0.03f);// +(collisionModel.m_x / 2.f));
						startPositions[i].z = startPositions[i].z + (((wantedPositions[i].z - startPositions[i].z) * percentage));
						wantedPositions[i].y = startPositions[i].y;
					}
				}
				else if (hitVsAir.y > 0.f && wantedVsStartingPosition.y > 0.f)
				{
					playerStartPosition.x = playerStartPosition.x + (((wantedVsStart.x)* percentage));
					playerStartPosition.y = playerStartPosition.y + (((wantedVsStart.y) * percentage) - 0.02f);// -0.07f;
					playerStartPosition.z = playerStartPosition.z + (((wantedVsStart.z)* percentage));
					wantedPosition.y = playerStartPosition.y;
					m_player->zeroOutXYVelocity();
					for (size_t i = 0; i < wantedPositions.size(); i++)
					{
						startPositions[i].x = startPositions[i].x + (((wantedPositions[i].x - startPositions[i].x) * percentage));// + 1.07f;
						startPositions[i].y = startPositions[i].y + (((wantedPositions[i].y - startPositions[i].y) * percentage) - 0.02f);// -(collisionModel.m_x / 2.f));
						startPositions[i].z = startPositions[i].z + (((wantedPositions[i].z - startPositions[i].z) * percentage));
						wantedPositions[i].y = startPositions[i].y;
					}
				}
				dSeconds -= (dSeconds * percentage);
				percentage = 0.0f;
			}
		}
	}
}

void World::correctivePhysics(Vector3& wantedPosition, const Vector3& collisionModel)
{
	for (int iterateNTimes = 0; iterateNTimes < 12; iterateNTimes++)
	{
		std::vector<BlockInfo> positionsToTest;
		//wantedPosition still the foot, center.
		getSurrounding12PositionsOfPlayerCollision(positionsToTest, wantedPosition, collisionModel);
		Vector3 currentAmountToShift = Vector3();
		Vector3 playerMax = Vector3(wantedPosition.x + (collisionModel.x / 2.f), wantedPosition.y + (collisionModel.y / 2.f), wantedPosition.z + collisionModel.z);
		Vector3 playerMin = Vector3(wantedPosition.x - (collisionModel.x / 2.f), wantedPosition.y - (collisionModel.y / 2.f), wantedPosition.z);
		//temporarily shift wantedPosition to the west and south.
		for (size_t posTestIndex = 0; posTestIndex < positionsToTest.size(); posTestIndex++)
		{
			Vector3 positionToTest = positionsToTest[posTestIndex].m_chunk->GetWorldCoordsForIndex(positionsToTest[posTestIndex].m_blockIndex); // swBottom
			Vector3 positionToTestMax = Vector3(positionToTest.x + 1.f, positionToTest.y + 1.f, positionToTest.z + 1.f); //neTop

			float shoveDown = (playerMax.z - positionToTest.z);
			float shoveUp = (positionToTestMax.z - playerMin.z);

			float shoveSouth = (playerMax.y - positionToTest.y);
			float shoveNorth = (positionToTestMax.y - playerMin.y);

			float shoveWest = (playerMax.x - positionToTest.x); // player max - box mins.
			float shoveEast = (positionToTestMax.x - playerMin.x); //block maxs - player mins.

			float minCorrection = GetSmallestNonZero(shoveEast, shoveWest, shoveNorth, shoveSouth, shoveUp, shoveDown);

			if (minCorrection == shoveUp)
			{
				currentAmountToShift = Vector3(0.f, 0.f, minCorrection);
				m_player->zeroOutZVelocity();
			}
			else if (minCorrection == shoveDown)
			{
				currentAmountToShift = Vector3(0.f, 0.f, -minCorrection);
				m_player->zeroOutZVelocity();
			}
			else if (minCorrection == shoveEast)
			{
				currentAmountToShift = Vector3(minCorrection, 0.f, 0.f);
				m_player->zeroOutXYVelocity();
			}
			else if (minCorrection == shoveWest)
			{
				currentAmountToShift = Vector3(-minCorrection, 0.f, 0.f);
				m_player->zeroOutXYVelocity();
			}
			else if (minCorrection == shoveNorth)
			{
				currentAmountToShift = Vector3(0.f, minCorrection, 0.f);
				m_player->zeroOutXYVelocity();
			}
			else if (minCorrection == shoveSouth)
			{
				currentAmountToShift = Vector3(0.f, -minCorrection, 0.f);
				m_player->zeroOutXYVelocity();
			}

			if (minCorrection != 0.f)
			{
				wantedPosition += currentAmountToShift;
				positionsToTest.clear();
				getSurrounding12PositionsOfPlayerCollision(positionsToTest, wantedPosition, collisionModel);
			}

		}
		if (currentAmountToShift.GetLength() == 0.f)
		{
			break;
		}

	}

}
const float World::generalComparison(const float& toCompare, const float& toReturn) const
{
	float returnMe = toReturn;
	if (toCompare > 0.f)
	{
		if (toReturn == 0.f || toCompare < toReturn)
		{
			returnMe = toCompare;
		}
	}
	return returnMe;
}
const float World::GetSmallestNonZero(const float& eastPush, const float& westPush, const float& northPush, const float& southPush, const float& upPush, const float& downPush) const
{
	float toReturn = 0.f;
	toReturn = generalComparison(upPush, toReturn);
	toReturn = generalComparison(downPush, toReturn);
	toReturn = generalComparison(eastPush, toReturn);
	toReturn = generalComparison(westPush, toReturn);
	toReturn = generalComparison(northPush, toReturn);
	toReturn = generalComparison(southPush, toReturn);
	return toReturn;
}

void World::getSurrounding12PositionsOfPlayerCollision(std::vector<BlockInfo>& positions, Vector3& start, const Vector3& collisionModel)
{
	Vector3 playerMins = Vector3(start.x - (collisionModel.x / 2.f), start.y - (collisionModel.y / 2.f), start.z);
	Vector3 stepAmount = collisionModel / 3.f;
	Vector3 playerMaxs = Vector3(playerMins.x + collisionModel.x, playerMins.y + collisionModel.y, start.z + collisionModel.z + 0.5f);
/*	Vector3 searchVolumeMaxs = Vector3(playerMins.m_x + xMax, playerMins.m_y + yMax, start.m_z + zMax);*/
	//BlockInfos? (only has chunk and block id.)
	AABB3 playerBounds(playerMins, playerMaxs);

	for (float z = playerMins.z; z <= playerMaxs.z; z += stepAmount.z)
	{
		for (float y = playerMins.y; y <= playerMaxs.y; y += stepAmount.y)
		{
			for (float x = playerMins.x; x <= playerMaxs.x; x += stepAmount.x)
			{
				Vector3 worldPos = Vector3(x, y, z);
				Chunk* chunk = GetChunkForWorldCoords(worldPos);
				if (chunk != nullptr)
				{
					BLOCK_TYPE type = chunk->getBlockTypeAtWorldCoords(worldPos);
					if (g_blockInfoContainers[type].m_solid == true)
					{
						positions.push_back(BlockInfo(chunk, chunk->GetBlockIndexForWorldCoords(worldPos)));
					}
				}
			}
		}
	}
}
void World::get8PositionsOfPlayerCollision(std::vector<Vector3>& startPositions, std::vector<Vector3>& wantedPositions, Vector3 playerStartPosition, Vector3& wantedPosition, const Vector3& collisionModel)
{
	//startPositions
	getSingle8PositionsOfPlayerCollision(startPositions, playerStartPosition,collisionModel);

	//wantedPositions
	getSingle8PositionsOfPlayerCollision(wantedPositions, wantedPosition, collisionModel);
}
void World::getSingle8PositionsOfPlayerCollision(std::vector<Vector3>& positions, const Vector3& start, const Vector3& collisionModel)
{
	//startPositions
	positions.push_back(Vector3(start.x - (collisionModel.x / 2.f), start.y - (collisionModel.y / 2.f), start.z)); // swBottom
	positions.push_back(Vector3(positions[0].x + collisionModel.x, positions[0].y, positions[0].z)); //seBottom
	positions.push_back(Vector3(positions[0].x, positions[0].y + collisionModel.y, positions[0].z)); // nwBottom
	positions.push_back(Vector3(positions[0].x + collisionModel.x, positions[0].y + collisionModel.y, positions[0].z)); //neBottom
								
	positions.push_back(Vector3(positions[0].x, positions[0].y, positions[0].z + (collisionModel.z / 2.f))); //swMiddle
	positions.push_back(Vector3(positions[1].x, positions[1].y, positions[1].z + (collisionModel.z / 2.f))); //seMiddle
	positions.push_back(Vector3(positions[2].x, positions[2].y, positions[2].z + (collisionModel.z / 2.f))); //nwMiddle
	positions.push_back(Vector3(positions[3].x, positions[3].y, positions[3].z + (collisionModel.z / 2.f))); //neMiddle

	positions.push_back(Vector3(positions[0].x, positions[0].y, positions[0].z + collisionModel.z)); //swUpper
	positions.push_back(Vector3(positions[1].x, positions[1].y, positions[1].z + collisionModel.z)); //seUpper
	positions.push_back(Vector3(positions[2].x, positions[2].y, positions[2].z + collisionModel.z)); //nwUpper
	positions.push_back(Vector3(positions[3].x, positions[3].y, positions[3].z + collisionModel.z)); //neUpper
}

void World::deleteABlockInFrontOfPlayer(float deltaSeconds)
{
	
	if (m_activeChunks.find(m_playerChunk) != m_activeChunks.end())
	{
		m_activeChunks[m_playerChunk]->m_chunkDirty = true;
		Vector3 playerPosition = m_player->GetPosition();
		Vector3 playerForward = m_player->GetForwardXYZ();
		Vector3 endPlayerForward = playerPosition + (playerForward * m_distanceFromPlayerThatCanDestroyPlaceBlocks);
		bool hasLineOfSight = true;
		Block_Hit hit = SlowHasLineOfSightChunkTest(playerPosition, endPlayerForward, hasLineOfSight);
		hit.m_worldPosBeforeHit.floorAll();
		hit.m_worldPosHit.floorAll();

		if (hasLineOfSight == false)
		{
			Chunk* chunkWhereHit = GetChunkForWorldCoords(hit.m_worldPosHit);
			if (chunkWhereHit != nullptr
				&& chunkWhereHit->getBlockTypeAtWorldCoords(hit.m_worldPosHit) == HORROR_STONE_BLOCK)
			{
				if (isLeftClickHeldDown == false)
				{
					m_currentSound->stop();
					m_currentSound = m_audio->PlaySound(g_blockInfoContainers[HORROR_STONE_BLOCK].m_breaking, 0.5f);
				}
				return;
			}
			if (chunkWhereHit != nullptr 
				&& (GetChunkForWorldCoords(lastLeftClickHit.m_worldPosHit) == nullptr || lastLeftClickHit.m_worldPosHit != hit.m_worldPosHit))
			{
				lastLeftClickHit = hit;
				m_leftClickHeldForSeconds = 0.f;
				amountOfSecondsSinceLastBreakingSound = 0.f;
				m_BlockBreakingAnim->Reset();
			}
			else if (chunkWhereHit != nullptr
				&& GetChunkForWorldCoords(lastLeftClickHit.m_worldPosHit) != nullptr && lastLeftClickHit.m_worldPosHit == hit.m_worldPosHit)
			{
				amountOfSecondsSinceLastBreakingSound -= deltaSeconds;
				BLOCK_TYPE type = chunkWhereHit->getBlockTypeAtWorldCoords(hit.m_worldPosHit);
				if (amountOfSecondsSinceLastBreakingSound <= 0.f && g_blockInfoContainers[type].m_breaking != -1)
				{
					amountOfSecondsSinceLastBreakingSound = amountOfSecondsBetweenBreakingSounds;
					m_currentSound->stop();
					m_currentSound = m_audio->PlaySound(g_blockInfoContainers[type].m_breaking, 0.5f);
				}
				float toughness = g_blockInfoContainers[GetChunkForWorldCoords(hit.m_worldPosHit)->getBlockTypeAtWorldCoords(hit.m_worldPosHit)].m_toughness;
				m_BlockBreakingAnim->setDurationSeconds(toughness);
				lastLeftClickHit = hit;
				m_leftClickHeldForSeconds += deltaSeconds; 
				m_BlockBreakingAnim->Update(deltaSeconds);
				if (m_leftClickHeldForSeconds >= toughness)
				{
					if (g_blockInfoContainers[type].m_broken != -1)
					{
						m_currentSound->stop();
						m_currentSound = m_audio->PlaySound(g_blockInfoContainers[type].m_broken, 0.5f);
					}
					Chunk* whereHit = GetChunkForWorldCoords(hit.m_worldPosHit);
					whereHit->convertBlockToAir(whereHit->GetLocalCoordsForWorldCoords(hit.m_worldPosHit));
					BlockInfo block = BlockInfo(whereHit, whereHit->GetBlockIndexForWorldCoords(hit.m_worldPosHit));
					markBlockDirty(block);
					m_leftClickHeldForSeconds = 0.f;
					amountOfSecondsSinceLastBreakingSound = 0.f;
					lastLeftClickHit = Block_Hit();
					m_BlockBreakingAnim->Reset();
				}
			}
		}
		else
		{
			m_leftClickHeldForSeconds = 0.f;
			amountOfSecondsSinceLastBreakingSound = 0.f;
			lastLeftClickHit = Block_Hit();
			m_BlockBreakingAnim->Reset();
		}
	}
}
void World::changeABlockTypeInFrontOfPlayer()
{
	if (m_activeChunks.find(m_playerChunk) != m_activeChunks.end())
	{
		Vector3 playerPosition = m_player->GetPosition();
		Vector3 playerForward = m_player->GetForwardXYZ();
		Vector3 endPlayerForward = playerPosition + (playerForward * m_distanceFromPlayerThatCanDestroyPlaceBlocks);
		bool hasLineOfSight = true;
		Block_Hit hit = SlowHasLineOfSightChunkTest(playerPosition, endPlayerForward, hasLineOfSight);
		if (hasLineOfSight == false)
		{
			if (hit.m_worldPosBeforeHit.x != -1 && hit.m_worldPosBeforeHit.y != -1 && hit.m_worldPosBeforeHit.z != -1 && GetChunkForWorldCoords(hit.m_worldPosBeforeHit) != nullptr)
			{
				AABB3 blockCoords = AABB3(Vector3(hit.m_worldPosBeforeHit.x - 1.f, hit.m_worldPosBeforeHit.y - 1.f, hit.m_worldPosBeforeHit.z - 1.f),
					Vector3(hit.m_worldPosBeforeHit.x + 1.f, hit.m_worldPosBeforeHit.y + 1.f, hit.m_worldPosBeforeHit.z + 1.f));
				AABB3 playerCollision = m_player->GetCollisionBounds();

				if (playerCollision.doesAABB3Overlap(blockCoords) == false)
				{
					BLOCK_TYPE type = (BLOCK_TYPE)m_player->GetInventoryIndex();
					Chunk* airChunk = GetChunkForWorldCoords(hit.m_worldPosBeforeHit);
					airChunk->convertBlockToBlockType(airChunk->GetLocalCoordsForWorldCoords(hit.m_worldPosBeforeHit), type);
					BlockInfo block = BlockInfo(airChunk, airChunk->GetBlockIndexForWorldCoords(hit.m_worldPosBeforeHit));
					markBlockDirty(block);
					if (g_blockInfoContainers[type].m_placed != -1)
					{
						m_currentSound->stop();
						m_currentSound = m_audio->PlaySound(g_blockInfoContainers[type].m_placed, 0.5f);
					}
				}
			}
		}
	}
}

void World::updateWhereLookingAt()
{
	WhereToDrawLookAt.erase(WhereToDrawLookAt.begin(), WhereToDrawLookAt.end());
	blockBreakDraw.erase(blockBreakDraw.begin(), blockBreakDraw.end());
	if (m_activeChunks.find(m_playerChunk) != m_activeChunks.end())
	{
		Vector3 playerPosition = m_player->GetPosition();
		Vector3 playerForward = m_player->GetForwardXYZ();
		Vector3 endPlayerForward = playerPosition + (playerForward * m_distanceFromPlayerThatCanDestroyPlaceBlocks);
		bool hasLineOfSight = true;
		Block_Hit hit = SlowHasLineOfSightChunkTest(playerPosition, endPlayerForward, hasLineOfSight);
		testPos = hit.m_worldPosHit;
		if (hasLineOfSight == false && GetChunkForWorldCoords(hit.m_worldPosBeforeHit) != nullptr && GetChunkForWorldCoords(hit.m_worldPosHit) != nullptr)
		{
			Vector3 dirtBlockPosition = Vector3(floor(hit.m_worldPosHit.x), floor(hit.m_worldPosHit.y), floor(hit.m_worldPosHit.z));
			Vector3 airBlockPosition = Vector3(floor(hit.m_worldPosBeforeHit.x), floor(hit.m_worldPosBeforeHit.y), floor(hit.m_worldPosBeforeHit.z));
			Vector3 vectorDif = dirtBlockPosition - airBlockPosition;
			if (vectorDif.z > 0)
			{
				//bottom side
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), dirtBlockPosition);
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y + 1.f, dirtBlockPosition.z));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y + 1.f, dirtBlockPosition.z));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y, dirtBlockPosition.z));
			}
			else if (vectorDif.z < 0)
			{
				//top side
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y + 1.f, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y + 1.f, dirtBlockPosition.z + 1.f));
			}
			else if (vectorDif.y < 0)
			{
				// north side
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y + 1.f, dirtBlockPosition.z));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y + 1.f, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y + 1.f, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y + 1.f, dirtBlockPosition.z));
			}
			else if (vectorDif.y > 0)
			{
				// south side
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y, dirtBlockPosition.z));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), dirtBlockPosition);
			}
			else if (vectorDif.x > 0)
			{
				//West side
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), dirtBlockPosition);
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y + 1.f, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x, dirtBlockPosition.y + 1.f, dirtBlockPosition.z));
			}
			else if (vectorDif.x < 0)
			{
				// east side
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y + 1.f, dirtBlockPosition.z));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y + 1.f, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y, dirtBlockPosition.z + 1.f));
				WhereToDrawLookAt.insert(WhereToDrawLookAt.begin() + WhereToDrawLookAt.size(), Vector3(dirtBlockPosition.x + 1.f, dirtBlockPosition.y, dirtBlockPosition.z));
			}
		}
		if (GetChunkForWorldCoords(lastLeftClickHit.m_worldPosBeforeHit) != nullptr && GetChunkForWorldCoords(lastLeftClickHit.m_worldPosHit) != nullptr)
		{
			Vector3 dirtBlock = Vector3(floor(lastLeftClickHit.m_worldPosHit.x), floor(lastLeftClickHit.m_worldPosHit.y), floor(lastLeftClickHit.m_worldPosHit.z));
			Vector3 airBlock = Vector3(floor(lastLeftClickHit.m_worldPosBeforeHit.x), floor(lastLeftClickHit.m_worldPosBeforeHit.y), floor(lastLeftClickHit.m_worldPosBeforeHit.z));
			Vector3 blockHitAndAirDiff = dirtBlock - airBlock;
			//m_leftClickHeldForSeconds = 0.f;
			//lastLeftClickHit = Block_Hit();
			//m_BlockBreakingAnim->Reset();
			if (blockHitAndAirDiff.z > 0)
			{
				//bottom side
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), dirtBlock);
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y + 1.f, dirtBlock.z));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y + 1.f, dirtBlock.z));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y, dirtBlock.z));
			}
			else if (blockHitAndAirDiff.z < 0)
			{
				//top side
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y + 1.f, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y + 1.f, dirtBlock.z + 1.f));
			}
			else if (blockHitAndAirDiff.y < 0)
			{
				// north side
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y + 1.f, dirtBlock.z));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y + 1.f, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y + 1.f, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y + 1.f, dirtBlock.z));
			}
			else if (blockHitAndAirDiff.y > 0)
			{
				// south side
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y, dirtBlock.z));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), dirtBlock);
			}
			else if (blockHitAndAirDiff.x > 0)
			{
				//West side
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), dirtBlock);
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y + 1.f, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x, dirtBlock.y + 1.f, dirtBlock.z));
			}
			else if (blockHitAndAirDiff.x < 0)
			{
				// east side
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y + 1.f, dirtBlock.z));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y + 1.f, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y, dirtBlock.z + 1.f));
				blockBreakDraw.insert(blockBreakDraw.begin() + blockBreakDraw.size(), Vector3(dirtBlock.x + 1.f, dirtBlock.y, dirtBlock.z));
			}
		}
	}
}

//Fast Line Of Sight testing
const Block_Hit World::SlowHasLineOfSightChunkTest(const Vector3& start, const Vector3& end, bool& hasLineOfSight)
{
	hasLineOfSight = true;
	Block_Hit toReturn;
	if (m_activeChunks.find(m_playerChunk) == m_activeChunks.end())
	{
		return toReturn;
	}
	Chunk* currentChunk = m_activeChunks[m_playerChunk];
	int numOfTimesToRun = 1000;
	Vector3 difference = end - start;
	Vector3 step = difference / (float)numOfTimesToRun;
	Vector3 current = start;
	toReturn.m_worldPosBeforeHit = start;

	for (int i = 0; i < numOfTimesToRun; i++)
	{
		if ((int)floor(current.x) >= currentChunk->getEndPosition().x)
		{
			Chunk* temp = currentChunk;
			currentChunk = nullptr;
			currentChunk = temp->EastChunk;
			if (currentChunk == nullptr)
			{
				break;
			}
		}
		else if ((int)floor(current.x) < currentChunk->getStartPosition().x)
		{
			Chunk* temp = currentChunk;
			currentChunk = nullptr;
			currentChunk = temp->WestChunk;
			if (currentChunk == nullptr)
			{
				break;
			}
		}
		if ((int)floor(current.y) >= currentChunk->getEndPosition().y)
		{
			Chunk* temp = currentChunk;
			currentChunk = nullptr;
			currentChunk = temp->NorthChunk;

			if (currentChunk == nullptr)
			{
				break;
			}
		}
		else if ((int)floor(current.y) < currentChunk->getStartPosition().y)
		{
			Chunk* temp = currentChunk;
			currentChunk = nullptr;
			currentChunk = temp->SouthChunk;
			if (currentChunk == nullptr)
			{
				break;
			}
		}

		IntVector3 blockLocalCoords = currentChunk->GetLocalCoordsForWorldCoords(current);
		if (blockLocalCoords.z < 0 || blockLocalCoords.z >= currentChunk->BLOCKS_WIDE_Z)
		{
			hasLineOfSight = true;
			break;
		}
		if (currentChunk->getBlockTypeAtLocalCoords(blockLocalCoords) != AIR_BLOCK && currentChunk->getBlockTypeAtLocalCoords(blockLocalCoords) != WATER_BLOCK)
		{
			/*toReturn.m_HitBlockInfo.m_blockIndex = blockIndex;
			toReturn.m_HitBlockInfo.m_chunk = nullptr;
			toReturn.m_HitBlockInfo.m_chunk = currentChunk;*/
			toReturn.m_worldPosHit = current;
			Vector3 expected = end - start;
			Vector3 actual = current - start;
			toReturn.m_fractionCompleted = actual.GetLength() / expected.GetLength();
			hasLineOfSight = false;
			break;
		}
		else
		{
			/*
			toReturn.m_AirBlockInfo.m_blockIndex = blockIndex;
			toReturn.m_AirBlockInfo.m_chunk = nullptr;
			toReturn.m_AirBlockInfo.m_chunk = currentChunk;*/
			toReturn.m_worldPosBeforeHit = current;
		}
		current += step;
	}
	return toReturn;
}
void SetUpDefaultsForFastLineOfSightChunkTest(const Vector3& start, Vector3& tDelta,Vector3& rayDisplacement, IntVector3& tileStep, IntVector3& tStart, IntVector3& offsetToLeadeingEdge, 
	Vector3& firstVerticalIntersection, Vector3& tOfNextCrossing)
{
	//x
	tDelta.x = 1.f / rayDisplacement.x;
	tileStep.x = 1;
	if (rayDisplacement.x < 0.f)
	{
		tileStep.x = -1;
	}
	offsetToLeadeingEdge.x = (tileStep.x + 1) / 2;
	firstVerticalIntersection.x = (float)(tStart.x + offsetToLeadeingEdge.x);
	tOfNextCrossing.x = abs(firstVerticalIntersection.x - start.x) * tDelta.x;

	//y
	tDelta.y = 1.f / rayDisplacement.y;
	tileStep.y = 1;
	if (rayDisplacement.y < 0.f)
	{
		tileStep.y = -1;
	}
	offsetToLeadeingEdge.y = (tileStep.y + 1) / 2;
	firstVerticalIntersection.y = (float)(tStart.y + offsetToLeadeingEdge.y);
	tOfNextCrossing.y = abs(firstVerticalIntersection.y - start.y) * tDelta.y;

	//z
	tDelta.z = 1.f / rayDisplacement.z;
	tileStep.z = 1;
	if (rayDisplacement.z < 0.f)
	{
		tileStep.z = -1;
	}
	offsetToLeadeingEdge.z = (tileStep.z + 1) / 2;
	firstVerticalIntersection.z = (float)(tStart.z + offsetToLeadeingEdge.z);
}
const Block_Hit World::FastHasLineOfSightChunkTest(const Vector3& start, const Vector3& end, bool& hasLineOfSight)
{
	Block_Hit hit;
	if (m_activeChunks.find(m_playerChunk) == m_activeChunks.end())
	{
		return hit;
	}
	Chunk* currentChunk = m_activeChunks[m_playerChunk];
	IntVector3 tStart = currentChunk->GetLocalCoordsForWorldCoords(start);
	if (currentChunk->isLocalCoordsInChunk(tStart) == true && currentChunk->getBlockTypeAtLocalCoords(tStart) != AIR_BLOCK)
	{
		hasLineOfSight = false;
		Block_Hit hit = Block_Hit(start);
		return hit;
	}
	IntVector3 tEnd = currentChunk->getBlockTypeAtWorldCoords(end);
	Vector3 rayDisplacement = end - start;
	Vector3 tDelta = Vector3(0.f, 0.f, 0.f);
	IntVector3 tileStep = IntVector3(0, 0, 0);
	IntVector3 offsetToLeadeingEdge = IntVector3(0, 0, 0);
	Vector3 firstVerticalIntersection = Vector3(0.f, 0.f, 0.f);
	Vector3 tOfNextCrossing = Vector3(0.f, 0.f, 0.f);

	SetUpDefaultsForFastLineOfSightChunkTest(start, tDelta, rayDisplacement, tileStep, tStart, offsetToLeadeingEdge,
		firstVerticalIntersection, tOfNextCrossing);

	IntVector3 tCurrent = tStart;
	Vector3 current = start;

	while (tCurrent != tEnd)
	{
		if (abs(tOfNextCrossing.x) < abs(tOfNextCrossing.y) && abs(tOfNextCrossing.x) < abs(tOfNextCrossing.z))
		{
			if (current.x >= currentChunk->getEndPosition().x)
			{
				Chunk* temp = currentChunk;
				currentChunk = nullptr;
				currentChunk = temp->WestChunk;
			}
			else if (current.x < currentChunk->getStartPosition().x)
			{
				Chunk* temp = currentChunk;
				currentChunk = nullptr;
				currentChunk = temp->EastChunk;
			}
			if (abs(tOfNextCrossing.x) > 1.f)
			{
				break;
			}
			tCurrent.x += tileStep.x;
			current.x += (float)tileStep.x;
			if (tCurrent.z >= 0.f && tCurrent < Chunk::BLOCKS_WIDE_Z
				&&currentChunk->getBlockTypeAtLocalCoords(tCurrent) != AIR_BLOCK
				&& currentChunk->getBlockTypeAtLocalCoords(tCurrent) != WATER_BLOCK)
			{
				hasLineOfSight = false;
				hit.m_worldPosBeforeHit = current;
				break;
			}
			tOfNextCrossing.x += tDelta.x;
		}
		else if (abs(tOfNextCrossing.y) <= abs(tOfNextCrossing.x) && abs(tOfNextCrossing.y) < abs(tOfNextCrossing.z))
		{
			if (current.y >= currentChunk->getEndPosition().y)
			{
				Chunk* temp = currentChunk;
				currentChunk = nullptr;
				currentChunk = temp->NorthChunk;
			}
			else if (current.y < currentChunk->getStartPosition().y)
			{
				Chunk* temp = currentChunk;
				currentChunk = nullptr;
				currentChunk = temp->SouthChunk;
			}
			if (abs(tOfNextCrossing.y) > 1.f)
			{
				break;
			}
			tCurrent.y += tileStep.y;
			current.y += (float)tileStep.y;
			if (tCurrent.z >= 0.f && tCurrent < Chunk::BLOCKS_WIDE_Z 
				&& currentChunk->getBlockTypeAtLocalCoords(tCurrent)!= AIR_BLOCK
				&& currentChunk->getBlockTypeAtLocalCoords(tCurrent) != WATER_BLOCK)
			{
				hasLineOfSight = false;
				hit.m_worldPosHit = current;
				break;
			}
			tOfNextCrossing.y += tDelta.y;
		}
		else if (abs(tOfNextCrossing.z) <= abs(tOfNextCrossing.x) && abs(tOfNextCrossing.z) <= abs(tOfNextCrossing.y))
		{
			if (abs(tOfNextCrossing.z) > 1.f)
			{
				break;
			}
			tCurrent.z += tileStep.z;
			current.z += (float)tileStep.z;
			if (tCurrent.z < 0 || tCurrent.z >= currentChunk->BLOCKS_WIDE_Z)
			{
				break;
			}
			else if (tCurrent.z >= 0.f && tCurrent < Chunk::BLOCKS_WIDE_Z
				&& currentChunk->getBlockTypeAtLocalCoords(tCurrent) != AIR_BLOCK
				&& currentChunk->getBlockTypeAtLocalCoords(tCurrent) != WATER_BLOCK)
			{
				hasLineOfSight = false;
				hit.m_worldPosHit= current;
				break;
			}
			tOfNextCrossing.z += tDelta.z;
		}
		if (currentChunk->getBlockTypeAtLocalCoords(tCurrent) != AIR_BLOCK
			|| currentChunk->getBlockTypeAtLocalCoords(tCurrent) != WATER_BLOCK)
		{
			hit.m_worldPosBeforeHit = current;
		}
	}
	hasLineOfSight = true;
	return hit;
}

//chunk generation, saving, and loading
void World::TestChunksAroundPlayerForGenerationOrLoad()
{
	IntVector2 coordsToGenerate;
	bool needToGenerate = GetHighestPriorityMissingChunk(coordsToGenerate);
	if (needToGenerate == true)
	{
		ChunkGeneration(coordsToGenerate);
	}
	IntVector2 chunkLocation;
	Chunk* toErase = GetFurthestUnneededChunk(chunkLocation);
	if (toErase != nullptr)
	{
		toErase->saveChunk();
		Chunk* EastChunk = toErase->EastChunk;
		if (EastChunk != nullptr)
		{
			EastChunk->WestChunk = nullptr;
		}
		Chunk* WestChunk = toErase->WestChunk;
		if (WestChunk != nullptr)
		{
			WestChunk->EastChunk = nullptr;
		}
		Chunk* SouthChunk = toErase->SouthChunk;
		if (SouthChunk != nullptr)
		{
			SouthChunk->NorthChunk = nullptr;
		}
		Chunk* NorthChunk = toErase->NorthChunk;
		if (NorthChunk != nullptr)
		{
			NorthChunk->SouthChunk = nullptr;
		}
		m_activeChunks.erase(chunkLocation);
	}
}
Chunk* World::GetChunkForWorldCoords(const Vector3& worldPos) const
{
	IntVector2 chunkCoords = IntVector2((int)floor(worldPos.x / 16.f), (int)floor(worldPos.y / 16.f));
	if (m_activeChunks.find(chunkCoords) != m_activeChunks.end())
	{
		return m_activeChunks.at(chunkCoords);
	}
	return nullptr;
}
void World::ChunkGeneration(const IntVector2& coordsToGenerate)
{
	bool doesThisChunkHaveASaveFile = GenerateChunkFromFile(coordsToGenerate);
	if (doesThisChunkHaveASaveFile == false)
	{
		GenerateNewChunk(coordsToGenerate);
	}

	Chunk* loadedChunk = m_activeChunks.find(coordsToGenerate)->second;
	std::vector<BlockInfo> skyBlocks = loadedChunk->getAllSkyBlockInfos();
	for (size_t index = 0; index < skyBlocks.size(); index++)
	{
		dirtyAllNeighbors(skyBlocks[index]);
	}
	std::vector<BlockInfo> opaqueEdgeBlocks = loadedChunk->getAllNonOpaqueEdgeBlocks();
	for (size_t index = 0; index < opaqueEdgeBlocks.size(); index++)
	{
		markBlockDirty(opaqueEdgeBlocks[index]);
	}
	loadedChunk->m_chunkDirty = true;
}
void World::GenerateNewChunk(const IntVector2& newChunkPosition)
{
	Chunk* newChunk = new Chunk(Vector3(newChunkPosition.x * 16.f, newChunkPosition.y * 16.f, 0.f), IntVector3(16, 16, 128), m_worldLighting);
	m_activeChunks.insert(std::pair<IntVector2, Chunk*>(newChunkPosition, newChunk));
	IntVector2 west = IntVector2(newChunkPosition.x - 1, newChunkPosition.y);
	if (m_activeChunks.find(west) != m_activeChunks.end() && m_activeChunks.at(west) != nullptr)
	{
		m_activeChunks[west]->EastChunk = m_activeChunks[newChunkPosition];
		m_activeChunks[newChunkPosition]->WestChunk = m_activeChunks[west];
		m_activeChunks[west]->m_chunkDirty = true;
	}
	IntVector2 east = IntVector2(newChunkPosition.x + 1, newChunkPosition.y);
	if (m_activeChunks.find(east) != m_activeChunks.end() && m_activeChunks.at(east) != nullptr)
	{
		m_activeChunks[east]->WestChunk = m_activeChunks[newChunkPosition];
		m_activeChunks[newChunkPosition]->EastChunk = m_activeChunks[east];
		m_activeChunks[east]->m_chunkDirty = true;
	}
	IntVector2 south = IntVector2(newChunkPosition.x, newChunkPosition.y - 1);
	if (m_activeChunks.find(south) != m_activeChunks.end() && m_activeChunks.at(south) != nullptr)
	{
		m_activeChunks[south]->NorthChunk = m_activeChunks[newChunkPosition];
		m_activeChunks[newChunkPosition]->SouthChunk = m_activeChunks[south];
		m_activeChunks[south]->m_chunkDirty = true;
	}
	IntVector2 north = IntVector2(newChunkPosition.x, newChunkPosition.y + 1);
	if (m_activeChunks.find(north) != m_activeChunks.end() && m_activeChunks.at(north) != nullptr)
	{
		m_activeChunks[north]->SouthChunk = m_activeChunks[newChunkPosition];
		m_activeChunks[newChunkPosition]->NorthChunk = m_activeChunks[north];
		m_activeChunks[north]->m_chunkDirty = true;
	}
}
const bool World::GenerateChunkFromFile(const IntVector2& loadChunkPosition)
{
	Chunk* loadedChunk = Chunk::loadChunk(loadChunkPosition, m_worldLighting);
	if (loadedChunk == nullptr)
	{
		return false;
	}
	loadedChunk->m_chunkDirty = true;
	m_activeChunks.insert(std::pair<IntVector2, Chunk*>(loadChunkPosition, loadedChunk));

	IntVector2 west = IntVector2(loadChunkPosition.x - 1, loadChunkPosition.y);
	if (m_activeChunks.find(west) != m_activeChunks.end() && m_activeChunks.at(west) != nullptr)
	{
		m_activeChunks[west]->EastChunk = m_activeChunks[loadChunkPosition];
		m_activeChunks[loadChunkPosition]->WestChunk = m_activeChunks[west];
		m_activeChunks[west]->m_chunkDirty = true;
	}
	IntVector2 east = IntVector2(loadChunkPosition.x + 1, loadChunkPosition.y);
	if (m_activeChunks.find(east) != m_activeChunks.end() && m_activeChunks.at(east) != nullptr)
	{
		m_activeChunks[east]->WestChunk = m_activeChunks[loadChunkPosition];
		m_activeChunks[loadChunkPosition]->EastChunk = m_activeChunks[east];
		m_activeChunks[east]->m_chunkDirty = true;
	}
	IntVector2 south = IntVector2(loadChunkPosition.x, loadChunkPosition.y - 1);
	if (m_activeChunks.find(south) != m_activeChunks.end() && m_activeChunks.at(south) != nullptr)
	{
		m_activeChunks[south]->NorthChunk = m_activeChunks[loadChunkPosition];
		m_activeChunks[loadChunkPosition]->SouthChunk = m_activeChunks[south];
		m_activeChunks[south]->m_chunkDirty = true;
	}
	IntVector2 north = IntVector2(loadChunkPosition.x, loadChunkPosition.y + 1);
	if (m_activeChunks.find(north) != m_activeChunks.end() && m_activeChunks.at(north) != nullptr)
	{
		m_activeChunks[north]->SouthChunk = m_activeChunks[loadChunkPosition];
		m_activeChunks[loadChunkPosition]->NorthChunk = m_activeChunks[north];
		m_activeChunks[north]->m_chunkDirty = true;
	}
	return true;
}
const bool World::GetHighestPriorityMissingChunk(IntVector2& outChunkCoords) const
{
	if ((m_activeChunks.size() == 0) 
		|| (m_activeChunks.find(m_playerChunk) == m_activeChunks.end()) 
		|| (m_activeChunks.at(m_playerChunk) == nullptr))
	{
		outChunkCoords = m_playerChunk;
		return true;
	}
	Chunk* temp = m_activeChunks.begin()->second;
	Vector3 blockSizes = temp->GetBlockSizes();

	bool checkedAllSides = false;
	int radiusTestOutwards = 1;
	while (checkedAllSides == false && radiusTestOutwards <= m_viewRadius)
	{
		for (int i = (m_playerChunk.x - radiusTestOutwards); i <= (m_playerChunk.x + radiusTestOutwards); i++)
		{
			for (int j = (m_playerChunk.y - radiusTestOutwards); j <= (m_playerChunk.y + radiusTestOutwards); j++)
				{
				if (i != m_playerChunk.x || j != m_playerChunk.y)
					{
						IntVector2 thisLoc = IntVector2(i, j);
						if ((m_activeChunks.find(thisLoc) == m_activeChunks.end()))
						{
							outChunkCoords = thisLoc;
							return true;
						}
					}
				}
		}
		radiusTestOutwards++;
		if (radiusTestOutwards > m_viewRadius)
		{
			checkedAllSides = true;
		}
	}
	return false;
}

IntVector2 World::GetChunkCoordsForWorldCoords(const Vector3& worldPos) const
{
	return IntVector2((int)floor(worldPos.x / Chunk::BLOCKS_WIDE_X), (int)floor(worldPos.y / Chunk::BLOCKS_WIDE_Y));
}
Chunk* World::GetFurthestUnneededChunk(IntVector2& chunkLocation)
{

	for (std::map<IntVector2, Chunk*>::const_iterator it = m_activeChunks.begin(); it != m_activeChunks.end(); it++)
	{
		IntVector2 temp = (it->first - m_playerChunk);
		if (temp.getLength() > m_distanceUntilSaveAndDelete)
		{
			chunkLocation = it->first;
			return it->second;
		}
	}
	return nullptr;
}
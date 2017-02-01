#include "Game/Chunk.hpp"
#include "Engine/Math/ExtendedMath.hpp"
#include <fstream>

//static variables
const Vector3 Chunk::m_blockSize = Vector3(1.f, 1.f, 1.f);
const std::string Chunk::s_saveFolderLocation = "Data/save01/";
const int Chunk::s_SandLevel = 55;
const int Chunk::s_seaLevel = 64;
const int Chunk::s_DirtLevelAboveSand = 5;
const int Chunk::s_GrassLevelBelowTop = 1;

const int Chunk::s_NumOfAdditionBlocksBeforeGenerateSand = 50;
const int Chunk::s_NumOfAdditionalBlocksBeforeGenerateDirt = 9;
const int Chunk::s_NumOfAdditionalBlocksBeforeGenerateGrass = 1;
const int Chunk::s_NumOfAdditionalBlocksBeforeGenerateAir = 1;
const int Chunk::s_RandomAdditionalBlocksBeforeDirt = 3;
const int Chunk::s_RandomAdditionalBlocksBeforeGrass = 4;

//Chunk Bit related.
const int Chunk::CHUNK_BITS_X = 4;
const int Chunk::BLOCKS_WIDE_X = 1 << CHUNK_BITS_X;
const int Chunk::LOCAL_X_MASK = BLOCKS_WIDE_X - 1;

const int Chunk::CHUNK_BITS_Y = 4;
const int Chunk::BITS_XY = CHUNK_BITS_X + CHUNK_BITS_Y;
const int Chunk::BLOCKS_WIDE_Y = 1 << CHUNK_BITS_Y;
const int Chunk::LOCAL_Y_MASK = (BLOCKS_WIDE_Y - 1) << CHUNK_BITS_X;
const int Chunk::BLOCKS_PER_LAYER = BLOCKS_WIDE_X * BLOCKS_WIDE_Y;

const int Chunk::CHUNK_BITS_Z = 7;
const int Chunk::BLOCKS_WIDE_Z = 1 << CHUNK_BITS_Z;
const int Chunk::LOCAL_Z_MASK = BLOCKS_WIDE_Z - 1;
const int Chunk::BLOCKS_PER_CHUNK = (BLOCKS_PER_LAYER * BLOCKS_WIDE_Z);


//constructors
Chunk::Chunk()
: m_startPosition(0.f, 0.f, 0.f), m_endPosition(16.f, 16.f, 128.f),
EastChunk(nullptr), WestChunk(nullptr), SouthChunk(nullptr), NorthChunk(nullptr),
m_chunkDirty(true), m_worldLighting(15), m_vboID(0)
{
}
Chunk::Chunk(const Vector3& startPosition, const IntVector3& chunkSize, int worldLighting)
: m_startPosition(startPosition),
m_endPosition(startPosition.x + (float)chunkSize.x, startPosition.y + (float)chunkSize.y, startPosition.z + (float)chunkSize.z),
EastChunk(nullptr), WestChunk(nullptr), SouthChunk(nullptr), NorthChunk(nullptr), m_chunkDirty(true), m_worldLighting(worldLighting), m_vboID(0)
{
	GenerateChunkPerlinNoise();
}
Chunk::~Chunk()
{
	//g_Renderer->DeleteBuffers(m_vboID);
	g_Renderer->DeleteVBO_ID(m_vboID);
}

//Chunk Generation
void Chunk::GenerateChunkFlat()
{
	m_blocks.resize(BLOCKS_PER_CHUNK);
	for (int x = 0; x < BLOCKS_WIDE_X; x++)
	{
		for (int y = 0; y < BLOCKS_WIDE_Y; y++)
		{
			BLOCK_TYPE type = STONE_BLOCK;
			float heightToReachBeforeChangeToSand = (float)s_NumOfAdditionBlocksBeforeGenerateSand;
			float HeightToReachBeforeChangeToDirt = heightToReachBeforeChangeToSand + (float)(s_NumOfAdditionalBlocksBeforeGenerateDirt + (s_RandomAdditionalBlocksBeforeDirt));
			float HeightToReachBeforeChangeToGrass = HeightToReachBeforeChangeToDirt + s_NumOfAdditionalBlocksBeforeGenerateGrass + (s_RandomAdditionalBlocksBeforeGrass);
			float HeightToReachBeforeChangeToAir = HeightToReachBeforeChangeToGrass + s_NumOfAdditionalBlocksBeforeGenerateGrass;
			for (int z = 0; z < HeightToReachBeforeChangeToAir; z++)
			{
				if (z > HeightToReachBeforeChangeToGrass)
				{
					type = GRASS_BLOCK;
				}
				else if (z >(HeightToReachBeforeChangeToDirt))
				{
					type = DIRT_BLOCK;
				}

				m_blocks[(z * BLOCKS_PER_LAYER) + (y * BLOCKS_WIDE_X) + x].setBlockType(type);
			}
		}
	}
	reloadAllLightingBits();
	m_numOfVertices = 0;
}

void Chunk::GenerateChunkPerlinNoise()
{
	m_blocks.resize(BLOCKS_PER_CHUNK);
	//float ComputePerlinNoise2D(const Vector2& position, float perlinNoiseGridCellSize, int numOctaves, float persistance);
	for (int x = 0; x < BLOCKS_WIDE_X; x++)
	{
		for (int y = 0; y < BLOCKS_WIDE_Y; y++)
		{
			Vector2 truePosition = Vector2(m_startPosition.x + x, m_startPosition.y + y);
			float thisSpotsZHeightBeforeNothingButAir = ComputePerlinNoise2D(truePosition, 1000.f, 3, 100.f, 20.f, 5.f);
			float LavaChanceInPillarXY = ComputePerlinNoise2D(truePosition, 100.f, 5, 50.f, 20.f, 0.f);
			BLOCK_TYPE type = STONE_BLOCK;
			float HeightToReachBeforeChangeToAir = s_SandLevel + s_DirtLevelAboveSand + thisSpotsZHeightBeforeNothingButAir;
			for (int z = 0; z < HeightToReachBeforeChangeToAir; z++)
			{
				float LavaChanceInPillar = ComputePerlinNoise2D(Vector2(truePosition.x, m_startPosition.z + z), 100.f, 5, 50.f, 20.f, 0.f);
				//const Vector2& position, const float& perlinNoiseGridCellSize, const int& numOctaves,const float& persistance, const float& amplitude = 1.f, const float& add = 0.f
				type = STONE_BLOCK;
				if (z > (s_SandLevel + s_DirtLevelAboveSand))
				{
					type = DIRT_BLOCK;
				}
				else if (z > s_SandLevel)
				{
					type = SAND_BLOCK;
				}
				else if (z > s_SandLevel - s_DirtLevelAboveSand)
				{
					type = DIRT_BLOCK;
				}
				if (z > (HeightToReachBeforeChangeToAir - 1) && type == DIRT_BLOCK)
				{
					type = GRASS_BLOCK;
				}
				if (z > 0 && z < ((s_SandLevel - s_DirtLevelAboveSand) * (3.f/4.f)) - 1.f)
				{
					if (LavaChanceInPillar > 0.f && LavaChanceInPillarXY > 0.f)
					{
						type = LAVA_BLOCK;
					}
				}
				if (z == 0)
				{
					type = HORROR_STONE_BLOCK;
				}

				
				m_blocks[((z << BITS_XY) + (y << CHUNK_BITS_X) + x)].setBlockType(type);
			}
			for (int z = (int)floor(HeightToReachBeforeChangeToAir); z < s_seaLevel; z++)
			{
				m_blocks[((z << BITS_XY) + (y << CHUNK_BITS_X) + x)].setBlockType(WATER_BLOCK);
			}
		}
	}
	std::vector<Cave*> nearbyCaves; // these are the cave centers
	FindAndDefineNearbyCaves(nearbyCaves);

	while (nearbyCaves.empty() == false)
	{
		BuildCave(*nearbyCaves.at(0));
		delete nearbyCaves[0];
		nearbyCaves.erase(nearbyCaves.begin());
	}
	reloadAllLightingBits();
	m_numOfVertices = 0;
}
void Chunk::FindAndDefineNearbyCaves(std::vector<Cave*> & nearbyCaves)
{
	IntVector2 myChunkCoords = IntVector2((int)floor(m_startPosition.x / 16.f), (int)floor(m_startPosition.y / 16.f));
	IntVector2 minChunkCoords = IntVector2(myChunkCoords.x - Cave::s_distanceOutToTestChunks, myChunkCoords.y - Cave::s_distanceOutToTestChunks);
	IntVector2 maxChunkCoords = IntVector2(myChunkCoords.x + Cave::s_distanceOutToTestChunks, myChunkCoords.y + Cave::s_distanceOutToTestChunks);
// 	int xDiff = maxChunkCoords.x - minChunkCoords.x;
// 	int yDiff = maxChunkCoords.y - minChunkCoords.y;
	std::map<IntVector2, float> perlinNoiseMap;

	for (int x = minChunkCoords.x - 1; x <= maxChunkCoords.x + 1; x++)
	{
		for (int y = minChunkCoords.y - 1; y <= maxChunkCoords.y + 1; y++)
		{
			perlinNoiseMap.insert(std::pair<IntVector2, float>(IntVector2(x, y), Cave::GetCavePerlinNoise(((float)x + 0.5f), ((float)y + 0.5f))));
		}
	}
	for (int x = minChunkCoords.x; x <= maxChunkCoords.x; x++)
	{
		for (int y = minChunkCoords.y; y <= maxChunkCoords.y; y++)
		{
			float northPerlin = perlinNoiseMap.at(IntVector2(x, y + 1));
			float southPerlin = perlinNoiseMap.at(IntVector2(x, y - 1));
			float eastPerlin = perlinNoiseMap.at(IntVector2(x + 1, y));
			float westPerlin = perlinNoiseMap.at(IntVector2(x - 1, y));
			float thisPerlin = perlinNoiseMap.at(IntVector2(x, y));
			if (thisPerlin > northPerlin
				&& thisPerlin > southPerlin
				&& thisPerlin > eastPerlin
				&& thisPerlin > westPerlin)
			{
				Cave* cave = Cave::GenerateCave((x + 0.5f) * BLOCKS_WIDE_X, (y + 0.5f) * BLOCKS_WIDE_Y);
				if (cave != nullptr)
				{
					nearbyCaves.push_back(cave); //these are the cave centers.
				}
			}
		}
	}
}
void Chunk::BuildCave(Cave& cave)
{
	//Cave::s_caveMaxRadius
	//Cave::s_minNumCavePoints
	//Cave::s_maxAdditiveCaveWidth
	IntVector2 myChunkCoords = IntVector2((int)floor(m_startPosition.x / Chunk::BLOCKS_WIDE_X), (int)floor(m_startPosition.y / Chunk::BLOCKS_WIDE_Y));
	IntVector2 minChunkCoordsToCheck = IntVector2(myChunkCoords.x - 1, myChunkCoords.y - 1);
	IntVector2 maxChunkCoordsToCheck = IntVector2(myChunkCoords.x + 1, myChunkCoords.y + 1);
	for (size_t i = 0; i < cave.m_cavePath.size(); i++)
	{
		CavePoint point = cave.m_cavePath.at(i);
		IntVector2 chunkCordsOfCavePoint = IntVector2((int)floor(point.m_position.x / Chunk::BLOCKS_WIDE_X), (int)floor(point.m_position.y / Chunk::BLOCKS_WIDE_Y));
		if (chunkCordsOfCavePoint >= minChunkCoordsToCheck
			&& chunkCordsOfCavePoint <= maxChunkCoordsToCheck)
		{
			CavePoint backwardPoint = point;
			if (i != 0)
			{
				backwardPoint = cave.m_cavePath[i - 1];
			}
			CavePoint forwardPoint = point;
			if (i != (cave.m_cavePath.size() - 1))
			{
				forwardPoint = cave.m_cavePath[i + 1];
			}

			if (backwardPoint != point)
			{
				RegenBlocksBasedOnCavePoints(backwardPoint, point);
			}
			if (forwardPoint != point)
			{
				RegenBlocksBasedOnCavePoints(point, forwardPoint);
			}
		}
	}
}

void Chunk::RegenBlocksBasedOnCavePoints(const CavePoint& point1, const CavePoint& point2)
{
	//int numOfTimesToRun = 1000;
	int numOfTimesToRun = (int)ceil(Cave::s_distanceOutToMoveFromCavePointMin);
	Vector3 difference = point2.m_position - point1.m_position;
	float lowestRadius = point1.m_caveWidthHere;
	float biggestRadius = point2.m_caveWidthHere;
	if (point1.m_caveWidthHere > point2.m_caveWidthHere)
	{
		lowestRadius = point2.m_caveWidthHere;
		biggestRadius = point1.m_caveWidthHere;
	}

	EulerAngles angularDisplacement = EulerAngles(0.f,0.f,0.f);
	angularDisplacement.m_yPitch = -asin(difference.z);
	angularDisplacement.m_zYaw = difference.y;
	if (angularDisplacement.m_yPitch != 0.f)
	{
		angularDisplacement.m_zYaw = (angularDisplacement.m_zYaw / cos(angularDisplacement.m_yPitch));
	}
	angularDisplacement.m_zYaw = asin(angularDisplacement.m_zYaw);

	Vector3 step = difference / (float)numOfTimesToRun;
	Vector3 current = point2.m_position;
	for (int i = 0; i < numOfTimesToRun; i++)
	{
		IntVector3 blockLocalCoords = GetLocalCoordsForWorldCoords(current);
		float radiusDif = (biggestRadius - lowestRadius);
		radiusDif = radiusDif * ((current - point1.m_position).GetLength() / (difference).GetLength());
		ChangeBlocksAtPositionOutByLengthInSpheres(current, lowestRadius + radiusDif);
		current += step;
	}
}
void Chunk::ChangeBlocksAtPositionOutByLengthInSpheres(const Vector3& position, const float& lengthOut)
{
	//need to get an up and down circle's worth of blocks out from position at a distance of lengthOut.
	for (size_t blockIndex = 0; blockIndex < m_blocks.size(); blockIndex++)
	{
		Vector3 blockPosition = GetWorldCoordsForIndex(blockIndex);
		Vector3 difference = blockPosition - position;
		if (blockPosition.z > 0 
			&& difference.GetLength() <= lengthOut
			&& m_blocks[blockIndex].isOpaqueSet() == true)
		{
			convertBlockToAir(GetLocalCoordsForIndex(blockIndex));
		}
	}
}

//block type conversion related
void Chunk::convertBlockToAir(const IntVector3& localCoords)
{
	convertBlockToBlockType(localCoords, AIR_BLOCK);
}
void Chunk::convertBlockToBlockType(const IntVector3& localCoords, const BLOCK_TYPE& type)
{
	if (EastChunk != nullptr && localCoords.x == (BLOCKS_WIDE_X - 1))
	{
		EastChunk->m_chunkDirty = true;
	}
	else if (WestChunk != nullptr && localCoords.x == 0)
	{
		WestChunk->m_chunkDirty = true;
	}
	if (NorthChunk != nullptr && localCoords.y == (BLOCKS_WIDE_Y - 1))
	{
		NorthChunk->m_chunkDirty = true;
	}
	else if (SouthChunk != nullptr && localCoords.y == 0)
	{
		SouthChunk->m_chunkDirty = true;
	}
	m_blocks[GetBlockIndexForLocalCoords(localCoords)].setBlockType(type);
	bool canBeSky = verifyCanBeSky(localCoords);
	this->dirtyBlocksBelow(localCoords, canBeSky);
}

const bool Chunk::verifyCanBeSky(const IntVector3& localCoords) const
{
	IntVector3 positionAbove = localCoords;
	positionAbove.z += 1;
	int topLayerZ = Chunk::BLOCKS_WIDE_Z - 1;
	if (localCoords.z < topLayerZ)
	{
		if (m_blocks[GetBlockIndexForLocalCoords(positionAbove)].isSkySet() == true
			&& m_blocks[GetBlockIndexForLocalCoords(localCoords)].isOpaqueSet() == false)
		{
			return true;
		}
	}
	else if (localCoords.z == topLayerZ)
	{
		if (m_blocks[GetBlockIndexForLocalCoords(localCoords)].isOpaqueSet() == false)
		{
			return true;
		}
	}
	return false;
}
void Chunk::dirtyBlocksBelow(const IntVector3& localCoords, const bool& isSky)
{
	int x = localCoords.x;
	int y = localCoords.y;
	for (int z = localCoords.z; z >= 0; z--)
	{
		IntVector3 local = IntVector3(x, y, z);
		int index = GetBlockIndexForLocalCoords(local);
		if (z != localCoords.z && m_blocks[index].isOpaqueSet() == true)
		{
			break;
		}
		m_blocks[index].setDirtyBit(true);
		m_blocks[index].setSkyValue(isSky);
	}
}
const unsigned char Chunk::getLightingAtWorldCoords(const Vector3& worldCoords) const
{
	int index = GetBlockIndexForWorldCoords(worldCoords);
	return m_blocks[index].getLightValue();
}

void Chunk::setLightingAtWorldCoords(const Vector3& worldCoords, const unsigned char& lighting)
{
	int index = GetBlockIndexForWorldCoords(worldCoords);
	m_chunkDirty = true;
	m_blocks[index].setLightValue(lighting);
}
void Chunk::setSkyAtWorldCoords(const Vector3& worldCoords, const bool& isSky)
{
	int index = GetBlockIndexForWorldCoords(worldCoords);
	m_chunkDirty = true;
	m_blocks[index].setSkyValue(isSky);
}
const bool Chunk::getIsSkyAtWorldCoords(const Vector3& worldCoords) const
{
	int index = GetBlockIndexForWorldCoords(worldCoords);
	return m_blocks[index].isSkySet();
}
std::vector<BlockInfo> Chunk::getAllNonOpaqueEdgeBlocks() const
{
	std::vector<BlockInfo> opaqueEdgeBlocks;
	for (int z = BLOCKS_WIDE_Z - 1; z >= 0; z--)
	{
		for (int y = BLOCKS_WIDE_Y - 1; y >= 0; y--)
		{
// 			if ((y != 0 && y != (BLOCKS_WIDE_Y - 1)))
// 			{
// 				continue;
// 			}
			for (int x = BLOCKS_WIDE_X - 1; x >= 0; x--)
			{
// 				if ((x != 0 && x != (BLOCKS_WIDE_X - 1)))
// 				{
// 					continue;
// 				}
				int blockIndex = GetBlockIndexForLocalCoords(IntVector3(x,y,z));
				if (m_blocks[blockIndex].isOpaqueSet() == false)
				{
					opaqueEdgeBlocks.push_back(BlockInfo(this, blockIndex));
				}
			}
		}
	}
	return opaqueEdgeBlocks;
}
std::vector<BlockInfo> Chunk::getAllSkyBlockInfos() const
{
	std::vector<BlockInfo> skyBlocks;
	for (int blockIndex = (Chunk::BLOCKS_PER_CHUNK - 1); blockIndex >= 0; blockIndex-= 1)
	{
		if (m_blocks[blockIndex].isSkySet() == true || g_blockInfoContainers[m_blocks[blockIndex].getBlockType()].m_lightSource == true)
		{
			skyBlocks.push_back(BlockInfo(this, blockIndex));
		}
	}
	return skyBlocks;
}
void Chunk::reloadAllLightingBits() 
{
	for (int x = 0; x < BLOCKS_WIDE_X; x++)
	{
		for (int y = 0; y < BLOCKS_WIDE_Y; y++)
		{
			bool encounteredSolid = false;
			for (int z = BLOCKS_WIDE_Z - 1; z >= 0; z--)
			{
				int i = GetBlockIndexForLocalCoords(IntVector3(x, y, z));
				bool opaque = m_blocks[i].isOpaqueSet();
				if (opaque == true)
				{
					m_blocks[i].setSkyValue(false);
					encounteredSolid = true;
				}
				else if (opaque == false && encounteredSolid == false)
				{
					m_blocks[i].setSkyValue(true);
				}
				else if (opaque == false && encounteredSolid == true)
				{
					m_blocks[i].setSkyValue(false);
				}

			}
		}
	}
	m_chunkDirty = true;
}

//Render

void Chunk::Render(const SpriteSheet* spriteSheet) const
{
	g_Renderer->BindTexture(spriteSheet->getSpriteSheetTexture());
	g_Renderer->DrawVBO_PCT(m_vboID, m_numOfVertices, PRIMITIVE_QUADS, true);
	g_Renderer->DisableTexture();
	//g_Renderer->DrawVertexArray(m_quadVerticesToRender, PRIMITIVE_QUADS, spriteSheet->getSpriteSheetTexture());
}
void Chunk::GetQuadEWDir(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos, 
	const int& index, const Vector3& worldvsStartDiff)
{
	//if want to try and generalize further, try using (void (Chunk::*pmemfn)(vars)) as a parameter for a method.
	//west side
	if (worldvsStartDiff.x == 0)
	{
		if (WestChunk != nullptr)
		{
			Vector3 westWorld = Vector3(worldPos.x - 1.f, worldPos.y, worldPos.z);
			BLOCK_TYPE westChunkBlockType = WestChunk->getBlockTypeAtWorldCoords(westWorld);
			if (westChunkBlockType == AIR_BLOCK || (westChunkBlockType == WATER_BLOCK && type != WATER_BLOCK))
			{
				unsigned char light = WestChunk->getLightingAtWorldCoords(westWorld);
				float trueLight = (1.f / 16.f) + (light / 16.f);
				Rgba lighting = Rgba(trueLight, trueLight, trueLight);
				getWestVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
			}
		}
	}
	else if ((m_blocks[index - 1].getBlockType() == AIR_BLOCK || (type != WATER_BLOCK && m_blocks[index - 1].getBlockType() == WATER_BLOCK)))
	{
		Vector3 westWorld = GetWorldCoordsForIndex(index - 1);
		unsigned char light = getLightingAtWorldCoords(westWorld);
		float trueLight = (1.f / 16.f) + (light / 16.f);
		Rgba lighting = Rgba(trueLight, trueLight, trueLight);
		getWestVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
	}


	//east side
	if (worldvsStartDiff.x == (BLOCKS_WIDE_X - 1))
	{
		if (EastChunk != nullptr)
		{
			Vector3 eastWorld = Vector3(worldPos.x + 1.f, worldPos.y, worldPos.z);
			BLOCK_TYPE eastChunkBlockType = EastChunk->getBlockTypeAtWorldCoords(eastWorld);
			if (eastChunkBlockType == AIR_BLOCK || (eastChunkBlockType == WATER_BLOCK && type != WATER_BLOCK))
			{
				unsigned char light = EastChunk->getLightingAtWorldCoords(eastWorld);
				float trueLight = (1.f / 16.f) + (light / 16.f);
				Rgba lighting = Rgba(trueLight, trueLight, trueLight);
				getEastVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
			}
		}
	}
	else if (m_blocks[index + 1].getBlockType() == AIR_BLOCK || (type != WATER_BLOCK && m_blocks[index + 1].getBlockType() == WATER_BLOCK))
	{
		Vector3 eastWorld = GetWorldCoordsForIndex(index + 1);
		unsigned char light = getLightingAtWorldCoords(eastWorld);
		float trueLight = (1.f / 16.f) + (light / 16.f);
		Rgba lighting = Rgba(trueLight, trueLight, trueLight);
		getEastVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
	}
}

void Chunk::GetQuadNSDir(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos,
	const int& index, const Vector3& worldvsStartDiff)
{

	//south side
	if (worldvsStartDiff.y == 0.f)
	{
		if (SouthChunk != nullptr)
		{
			Vector3 southWorld = Vector3(worldPos.x, worldPos.y - 1.f, worldPos.z);
			BLOCK_TYPE southChunkBlockType = SouthChunk->getBlockTypeAtWorldCoords(southWorld);
			if (southChunkBlockType == AIR_BLOCK || (southChunkBlockType == WATER_BLOCK && type != WATER_BLOCK))
			{
				unsigned char light = SouthChunk->getLightingAtWorldCoords(southWorld);
				float trueLight = (1.f / 16.f) + (light / 16.f);
				Rgba lighting = Rgba(trueLight, trueLight, trueLight);
				getSouthVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
			}
		}
	}
	else if ((m_blocks[index - BLOCKS_WIDE_X].getBlockType() == AIR_BLOCK || (type != WATER_BLOCK && m_blocks[index - BLOCKS_WIDE_X].getBlockType() == WATER_BLOCK)))
	{
		Vector3 southWorld = GetWorldCoordsForIndex(index - BLOCKS_WIDE_X);
		unsigned char light = getLightingAtWorldCoords(southWorld);
		float trueLight = (1.f / 16.f) + (light / 16.f);
		Rgba lighting = Rgba(trueLight, trueLight, trueLight);
		getSouthVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
	}


	//north side
	if (worldvsStartDiff.y == (BLOCKS_WIDE_Y - 1))
	{
		if (NorthChunk != nullptr)
		{
			Vector3 northWorld = Vector3(worldPos.x, worldPos.y + 1.f, worldPos.z);
			BLOCK_TYPE northChunkBlockType = NorthChunk->getBlockTypeAtWorldCoords(northWorld);
			if (northChunkBlockType == AIR_BLOCK || (northChunkBlockType == WATER_BLOCK && type != WATER_BLOCK))
			{
				unsigned char light = NorthChunk->getLightingAtWorldCoords(northWorld);
				float trueLight = (1.f / 16.f) + (light / 16.f);
				Rgba lighting = Rgba(trueLight, trueLight, trueLight);
				getNorthVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
			}
		}
	}
	else if ((m_blocks[index + BLOCKS_WIDE_X].getBlockType() == AIR_BLOCK || (type != WATER_BLOCK && m_blocks[index + BLOCKS_WIDE_X].getBlockType() == WATER_BLOCK)))
	{
		Vector3 northWorld = Vector3(worldPos.x, worldPos.y + 1.f, worldPos.z);
		unsigned char light = getLightingAtWorldCoords(northWorld);
		float trueLight = (1.f / 16.f) + (light / 16.f);
		Rgba lighting = Rgba(trueLight, trueLight, trueLight);
		getNorthVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].sideTextureCoords), lighting);
	}
}
void Chunk::GetQuadTBDir(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos,
	const int& index, const Vector3& worldvsStartDiff)
{
	//top
	if ((worldvsStartDiff.z + 1 < BLOCKS_WIDE_Z
		&& (m_blocks[index + BLOCKS_PER_LAYER].getBlockType() == AIR_BLOCK
		|| (type != WATER_BLOCK && m_blocks[index + BLOCKS_PER_LAYER].getBlockType() == WATER_BLOCK)))
		|| ((worldvsStartDiff.z) == (float)(BLOCKS_WIDE_Z - 1.f)))
	{
		float trueLight = 1.f;
		if (worldvsStartDiff.z != (float)(BLOCKS_WIDE_Z - 1.f))
		{
			trueLight = (1.f + getLightingAtWorldCoords(Vector3(worldPos.x, worldPos.y, worldPos.z + 1.f))) / 16.f;
		}
		Rgba lighting = Rgba(trueLight, trueLight, trueLight);
		getTopVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].topTextureCoords), lighting);
		if (type == WATER_BLOCK && m_blocks[index + BLOCKS_PER_LAYER].getBlockType() != WATER_BLOCK)
		{
			getTopUpsideDownVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].topTextureCoords), lighting);
		}
	}

	//bottom
	if ((worldvsStartDiff.z) > 0
		&& (m_blocks[index - BLOCKS_PER_LAYER].getBlockType() == AIR_BLOCK || (type != WATER_BLOCK && m_blocks[index - BLOCKS_PER_LAYER].getBlockType() == WATER_BLOCK)))
	{
		float trueLight = 1.f;
		if (worldvsStartDiff.z != 0.f)
		{
			trueLight = (1.f + getLightingAtWorldCoords(Vector3(worldPos.x, worldPos.y, worldPos.z - 1.f))) / 16.f;
		}
		Rgba lighting = Rgba(trueLight, trueLight, trueLight);
		getBottomVertices(quadVerticesToRender, worldPos, spriteSheet->GetTexCoordsForSpriteCoords(g_blockInfoContainers[type].bottomTextureCoords), lighting);
	}
}
void Chunk::GetQuadVertsAtBlock(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos, const int& index)
{

	IntVector3 localCoords = GetLocalCoordsForIndex(index);
	Vector3 worldvsStartDiff = worldPos - m_startPosition;

	if (type != AIR_BLOCK)
	{
		GetQuadEWDir(quadVerticesToRender, spriteSheet, type, worldPos, index, worldvsStartDiff);
		GetQuadNSDir(quadVerticesToRender, spriteSheet, type, worldPos, index, worldvsStartDiff);
		GetQuadTBDir(quadVerticesToRender, spriteSheet, type, worldPos, index, worldvsStartDiff);
	}
}
void Chunk::UpdateQuadVertices(const SpriteSheet* spriteSheet)
{
	if (m_chunkDirty == true)
	{
		std::vector<Vertex_PCT> quadVerticesToRender;
		for (int i = 0; i < BLOCKS_PER_CHUNK; i++)
		{
			BLOCK_TYPE type = m_blocks[i].getBlockType();
			Vector3 worldPos = GetWorldCoordsForIndex(i);
			GetQuadVertsAtBlock(quadVerticesToRender, spriteSheet, type, worldPos, i);
		}
		m_chunkDirty = false;
		m_numOfVertices = quadVerticesToRender.size();
		g_Renderer->GenerateOrUpdateVBO_ID(quadVerticesToRender, m_vboID);
	}
}
inline void Chunk::getBottomVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords, const Rgba& lightValOfNeighborBelow) const
{
	quadVerticesToRender.push_back(Vertex_PCT(position, lightValOfNeighborBelow, texCoords.mins));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y + m_blockSize.y, position.z), lightValOfNeighborBelow, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y + m_blockSize.y, position.z), lightValOfNeighborBelow, texCoords.maxs));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y, position.z), lightValOfNeighborBelow, Vector2(texCoords.maxs.x, texCoords.mins.y)));
}
inline void Chunk::getTopVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords, const Rgba& lightValOfNeighborAbove) const
{
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y, position.z + m_blockSize.z), lightValOfNeighborAbove, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y, position.z + m_blockSize.z), lightValOfNeighborAbove, texCoords.maxs));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborAbove, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborAbove, texCoords.mins));
}

inline void Chunk::getTopUpsideDownVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords, const Rgba& lightValOfNeighborAbove) const
{
	//meant to be used with water while player is underwater.
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborAbove, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborAbove, texCoords.maxs));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y, position.z + m_blockSize.z), lightValOfNeighborAbove, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y, position.z + m_blockSize.z), lightValOfNeighborAbove, texCoords.mins));
}
inline void Chunk::getNorthVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords, const Rgba& lightValOfNeighborNorth) const
{
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y + m_blockSize.y, position.z), lightValOfNeighborNorth, texCoords.maxs));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborNorth, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborNorth, texCoords.mins));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y + m_blockSize.y, position.z), lightValOfNeighborNorth, Vector2(texCoords.mins.x, texCoords.maxs.y)));
}
inline void Chunk::getSouthVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords, const Rgba& lightValOfNeighborSouth) const
{
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y, position.z), lightValOfNeighborSouth, texCoords.maxs));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y, position.z + m_blockSize.z), lightValOfNeighborSouth, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y, position.z + m_blockSize.z), lightValOfNeighborSouth, texCoords.mins));
	quadVerticesToRender.push_back(Vertex_PCT(position, lightValOfNeighborSouth, Vector2(texCoords.mins.x, texCoords.maxs.y)));
}
inline void Chunk::getWestVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords, const Rgba& lightValOfNeighborWest) const
{
	quadVerticesToRender.push_back(Vertex_PCT(position, lightValOfNeighborWest, texCoords.maxs));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y, position.z + m_blockSize.z), lightValOfNeighborWest, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborWest, texCoords.mins));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x, position.y + m_blockSize.y, position.z), lightValOfNeighborWest, Vector2(texCoords.mins.x, texCoords.maxs.y)));
}
inline void Chunk::getEastVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords, const Rgba& lightValOfNeighborEast) const
{
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y + m_blockSize.y, position.z), lightValOfNeighborEast, texCoords.maxs));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y + m_blockSize.y, position.z + m_blockSize.z), lightValOfNeighborEast, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y, position.z + m_blockSize.z), lightValOfNeighborEast, texCoords.mins));
	quadVerticesToRender.push_back(Vertex_PCT(Vector3(position.x + m_blockSize.x, position.y, position.z), lightValOfNeighborEast, Vector2(texCoords.mins.x, texCoords.maxs.y)));
}
//fast has line of sight testing
const Vector3 Chunk::getStartPosition() const
{
	return m_startPosition;
}
const Vector3 Chunk::getEndPosition() const
{
	return m_endPosition;
}


//getters

const int Chunk::GetBlockIndexForWorldCoords(const Vector3& worldCoords) const
{
	Vector3 relativeCoords = worldCoords - m_startPosition;
	int localX = (int)floor(relativeCoords.x);
	int localY = (((int)floor(relativeCoords.y)) << CHUNK_BITS_X);
	int localZ = (((int)floor(relativeCoords.z)) << BITS_XY);
	return (localZ | localY | localX);
}
const int Chunk::GetBlockIndexForLocalCoords(const IntVector3& localCoords) const
{
	return (localCoords.z << BITS_XY) | (localCoords.y << CHUNK_BITS_X) | localCoords.x;
	//return localCoords.m_x | (localCoords.m_y << BITS_X) | (localCoords.m_z << BITS_XY);
}
const IntVector3 Chunk::GetLocalCoordsForIndex(const int& index) const
{
	IntVector3 localCoords;
	localCoords.z = (index >> BITS_XY);
	localCoords.y = (index & LOCAL_Y_MASK) >> CHUNK_BITS_X;
	localCoords.x = (index & LOCAL_X_MASK);
	return localCoords;
}
const IntVector3 Chunk::GetLocalCoordsForWorldCoords(const Vector3& worldCoords) const
{
	IntVector3 localCoords;
	localCoords.x = (int)floor(worldCoords.x - m_startPosition.x);
	localCoords.y = (int)floor(worldCoords.y - m_startPosition.y);
	localCoords.z = (int)floor(worldCoords.z - m_startPosition.z);
	return localCoords;
}
const Vector3 Chunk::GetWorldCoordsForIndex(const int& index) const
{
	Vector3 worldCoords;
	worldCoords.z = m_startPosition.z + (float)((index >> BITS_XY));
	worldCoords.y = m_startPosition.y + (float)((index & LOCAL_Y_MASK) >> CHUNK_BITS_X);
	worldCoords.x = m_startPosition.x + (float)(index & LOCAL_X_MASK);
	return worldCoords;
}

const Vector3 Chunk::GetWorldCoordsForLocalCoords(const IntVector3& localCoords) const
{
	Vector3 worldCoords;
	worldCoords.z = m_startPosition.z + localCoords.z;
	worldCoords.y = m_startPosition.y + localCoords.y;
	worldCoords.x = m_startPosition.x + localCoords.x;
	return worldCoords;
}
const Vector3 Chunk::GetChunkCenter() const
{
	return ((m_startPosition + m_endPosition) / 2.f); 
}
const Vector3 Chunk::GetChunkCenterGroundLevel() const
{
	float highestZValueThatsAboveGround = 0.f;
	bool lastBlockCheckedIsSolid = false;
	Vector3 center = ((m_startPosition + m_endPosition) / 2.f);
	for (int z = 0; z < BLOCKS_WIDE_Z; z++)
	{
		int index = GetBlockIndexForWorldCoords(Vector3(center.x, center.y, (float)z));
		if (m_blocks[index].getBlockType() != AIR_BLOCK)
		{
			lastBlockCheckedIsSolid = true;
		}
		else if (lastBlockCheckedIsSolid == true && m_blocks[index].getBlockType() == false)
		{
			highestZValueThatsAboveGround = (float)z;
			lastBlockCheckedIsSolid = false;
		}
		else
		{
			lastBlockCheckedIsSolid = false;
		}
	}

	if (highestZValueThatsAboveGround == 0.f && m_blocks[GetBlockIndexForWorldCoords(Vector3(center.x, center.y, (float)BLOCKS_WIDE_Z - 1))].getBlockType() != AIR_BLOCK)
	{
		highestZValueThatsAboveGround = (float)BLOCKS_WIDE_Z - 1.f;
	}
	center.z = highestZValueThatsAboveGround;
	return center;
}


//testers
const bool Chunk::isLocalCoordsInChunk(const IntVector3& localCoords) const
{
	if (localCoords.z >= 0 && localCoords.z < m_endPosition.z
		&& localCoords.y >= 0 && localCoords.y < m_endPosition.y
		&& localCoords.x >= 0 && localCoords.x < m_endPosition.x)
	{
		return true;
	}
	return false;
}
const bool Chunk::testDirectoryExists() const
{
	if (GetFileAttributes((LPCWSTR)s_saveFolderLocation.c_str()) == ((DWORD)-1))
	{
		return false;
	}
	return true;
}

//Save and Load related
const bool Chunk::saveChunk() const
{
	IntVector2 chunkCell = IntVector2((int)floor(m_startPosition.x / BLOCKS_WIDE_X), (int)floor(m_startPosition.y / BLOCKS_WIDE_Y));
	std::string fileName = Chunk::GetFileName(chunkCell);
	
	FILE* file = nullptr;
	fopen_s(&file, fileName.c_str(), "wb");
	if (file == nullptr)
	{
		return false;
	}
	unsigned char currentType = (unsigned char) AIR_BLOCK;
	unsigned char prevType = (unsigned char) AIR_BLOCK;
	int typeCount = 0;
	int endCount = 0;
	for (int i = 0; i < BLOCKS_PER_CHUNK; i++)
	{
		currentType = m_blocks[i].getBlockTypeAsChar();
		if (i == 0)
		{
			typeCount += 1;
			prevType = currentType;
		}
		else if (currentType == prevType)
		{
			typeCount += 1;
			if (typeCount == 255)
			{
				fwrite(&prevType, sizeof(unsigned char), 1, file);
				fwrite(&typeCount, sizeof(unsigned char), 1, file);
				typeCount = 0;
			}
		}
		else if (currentType != prevType)
		{
			fwrite(&prevType, sizeof(unsigned char), 1, file);
			fwrite(&typeCount, sizeof(unsigned char), 1, file);
			prevType = currentType;
			typeCount = 1;
		}
		endCount++;
	}
	if (typeCount > 0)
	{
		if ((typeCount + endCount) >= BLOCKS_PER_CHUNK)
		{
			typeCount = BLOCKS_PER_CHUNK - endCount;
		}
		fwrite(&prevType, sizeof(unsigned char), 1, file);
		fwrite(&typeCount, sizeof(unsigned char), 1, file);
	}

	fclose(file);
	return true;
}
Chunk* Chunk::loadChunk(const IntVector2& chunkCell, int worldLighting)
{
	std::string fileName = Chunk::GetFileName(chunkCell);
	FILE* file = nullptr;
	fopen_s(&file, fileName.c_str(), "rb");
	if (file == nullptr)
	{
		return nullptr;
	}
	Chunk* chunk = new Chunk();
	chunk->setWorldLighting(worldLighting);
	chunk->m_blocks.resize(BLOCKS_PER_CHUNK);
	chunk->m_startPosition = Vector3((float)(chunkCell.x * BLOCKS_WIDE_X), (float)(chunkCell.y * BLOCKS_WIDE_Y), 0.f);
	chunk->m_endPosition = Vector3((float)(chunk->m_startPosition.x + BLOCKS_WIDE_X), (float)(chunk->m_startPosition.y + BLOCKS_WIDE_Y), (float)(BLOCKS_WIDE_Z));

	//fseek(fileToRead, SEEK_END, 0);
	//int size = ftell(fileToRead);
	//rewind(fileToRead);
	int curIndex = 0;
	for (int i = 0; i < chunk->BLOCKS_PER_CHUNK; i++)
	{
		unsigned char Type;
		unsigned char numOfType;
		fread(&Type, sizeof(unsigned char), 1, file);
		fread(&numOfType, sizeof(unsigned char), 1, file);
		int num_type = (int)numOfType;
		BLOCK_TYPE type = (BLOCK_TYPE)((int)Type);
		for (size_t j = 0; j < (size_t)num_type && curIndex < chunk->BLOCKS_PER_CHUNK; j++)
		{
			chunk->m_blocks[curIndex].setBlockType(type);
			curIndex++;
		}
	}

	fclose(file);

	chunk->reloadAllLightingBits();
	chunk->m_chunkDirty = true;
	chunk->m_numOfVertices = 0;
	return chunk;
}

const std::string Chunk::GetFileName(const IntVector2& chunkCell)
{
	std::string fileName = Stringf("chunk(%i,%i).bin", chunkCell.x, chunkCell.y);
	fileName = s_saveFolderLocation + fileName;
	return fileName;
}

Block* Chunk::getBlockByIndex(int blockIndex)
{
	return &(m_blocks[blockIndex]);
}


BlockInfo::BlockInfo(const Chunk* chunk, const int& blockIndex)
: m_chunk((Chunk*)chunk), m_blockIndex(blockIndex)
{
}

Block* BlockInfo::GetBlock()
{
	if (m_chunk == nullptr)
	{
		return nullptr;
	}
	return m_chunk->getBlockByIndex(m_blockIndex);
}

BlockInfo BlockInfo::GetNeighborAbove() const
{
	if (m_chunk == nullptr)
	{
		return BlockInfo(nullptr, 0);
	}
	if (m_blockIndex >= (Chunk::BLOCKS_PER_CHUNK - Chunk::BLOCKS_PER_LAYER))
	{
		return BlockInfo(nullptr, 0);
	}
	return BlockInfo(m_chunk, m_blockIndex + Chunk::BLOCKS_PER_LAYER);
}
BlockInfo BlockInfo::GetNeighborBelow() const
{
	if (m_chunk == nullptr)
	{
		return BlockInfo(nullptr, 0);
	}
	if (m_blockIndex < (Chunk::BLOCKS_PER_LAYER))
	{
		return BlockInfo(nullptr, 0);
	}
	return BlockInfo(m_chunk, m_blockIndex - Chunk::BLOCKS_PER_LAYER);
}
BlockInfo BlockInfo::GetNeighborEast() const
{
	if (m_chunk == nullptr)
	{
		return BlockInfo(nullptr, 0);
	}
	IntVector3 localCoords = m_chunk->GetLocalCoordsForIndex(m_blockIndex);
	localCoords.x += 1;
	if ((localCoords.x) >= Chunk::BLOCKS_WIDE_X)
	{
		localCoords.x -= Chunk::BLOCKS_WIDE_X;
		return BlockInfo(m_chunk->EastChunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
	}
	return BlockInfo(m_chunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
}
BlockInfo BlockInfo::GetNeighborWest() const
{
	if (m_chunk == nullptr)
	{
		return BlockInfo(nullptr, 0);
	}
	IntVector3 localCoords = m_chunk->GetLocalCoordsForIndex(m_blockIndex);
	localCoords.x -= 1;
	if ((localCoords.x) < 0)
	{
		localCoords.x += Chunk::BLOCKS_WIDE_X;
		return BlockInfo(m_chunk->WestChunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
	}
	return BlockInfo(m_chunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
}
BlockInfo BlockInfo::GetNeighborNorth() const
{
	if (m_chunk == nullptr)
	{
		return BlockInfo(nullptr, 0);
	}
	IntVector3 localCoords = m_chunk->GetLocalCoordsForIndex(m_blockIndex);
	localCoords.y += 1;
	if ((localCoords.y) >= Chunk::BLOCKS_WIDE_Y)
	{
		localCoords.y -= Chunk::BLOCKS_WIDE_Y;
		return BlockInfo(m_chunk->NorthChunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
	}
	return BlockInfo(m_chunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
}
BlockInfo BlockInfo::GetNeighborSouth() const
{
	if (m_chunk == nullptr)
	{
		return BlockInfo(nullptr, 0);
	}
	IntVector3 localCoords = m_chunk->GetLocalCoordsForIndex(m_blockIndex);
	localCoords.y -= 1;
	if ((localCoords.y) < 0)
	{
		localCoords.y += Chunk::BLOCKS_WIDE_Y;
		return BlockInfo(m_chunk->SouthChunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
	}
	return BlockInfo(m_chunk, m_chunk->GetBlockIndexForLocalCoords(localCoords));
}
//Block_Hit
/*Block_Hit::Block_Hit(Chunk* chunk, const int& blockHit, const int& airHit, Chunk* airChunk)
: m_HitBlockInfo(chunk, blockHit),
m_AirBlockInfo(airChunk, airHit)
{
}
*/

Block_Hit::Block_Hit(Vector3 worldPosHit, Vector3 worldPosBeforeHit, float fractionCompleted)
: m_fractionCompleted(fractionCompleted),
m_worldPosBeforeHit(worldPosBeforeHit),
m_worldPosHit(worldPosHit)
{

}

Block_Hit::Block_Hit(const Block_Hit& other)
: m_fractionCompleted(other.m_fractionCompleted),
m_worldPosBeforeHit(other.m_worldPosBeforeHit),
m_worldPosHit(other.m_worldPosHit)
{
}
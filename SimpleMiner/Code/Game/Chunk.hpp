#pragma once
#include "Game/Block.hpp"
#include "Game/BlockTextureContainer.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "Game/Cave.hpp"

#ifndef CHUNK_HPP
#define CHUNK_HPP

//IntVector3 = WorldCoords
//Vector3 = WorldPos
//IntVector3 = LocalCoords
//IntVector2 = ChunkCoords
//int = LocalIndex
struct Block_Hit;
struct BlockInfo;

class Chunk
{
public:
	//constructors
	Chunk();
	Chunk(const Vector3& startPosition, const IntVector3& chunkSize, int worldLighting = 15);
	~Chunk();

	//chunk generation
private:
	void GenerateChunkFlat();
	void GenerateChunkPerlinNoise();
	void FindAndDefineNearbyCaves(std::vector<Cave*> & nearbyCaves);
	void BuildCave(Cave& cave);
	void RegenBlocksBasedOnCavePoints(const CavePoint& point1, const CavePoint& point2);
	void ChangeBlocksAtPositionOutByLengthInSpheres(const Vector3& position, const float& lengthOut);

public:
	//block type conversion related
	void convertBlockToAir(const IntVector3& localCoords);
	void convertBlockToBlockType(const IntVector3& localCoords, const BLOCK_TYPE& type);
private:
	void dirtyBlocksBelow(const IntVector3& localCoords, const bool& isSky);
	const bool verifyCanBeSky(const IntVector3& localCoords) const;
public:
	std::vector<BlockInfo> getAllNonOpaqueEdgeBlocks() const;
	std::vector<BlockInfo> getAllSkyBlockInfos() const;
	void reloadAllLightingBits();
	inline const unsigned char getLightingAtWorldCoords(const Vector3& worldCoords) const;
	void setWorldLighting(int lighting) { m_worldLighting = lighting; };
	void setLightingAtWorldCoords(const Vector3& worldCoords, const unsigned char& lighting);
	void setSkyAtWorldCoords(const Vector3& worldCoords, const bool& isSky);
	const bool getIsSkyAtWorldCoords(const Vector3& worldCoords) const;
	const int getWorldLighting() const { return m_worldLighting; };
	inline const BLOCK_TYPE getBlockTypeAtLocalCoords(const IntVector3& localCoords) const {return m_blocks[GetBlockIndexForLocalCoords(localCoords)].getBlockType();};
	const BLOCK_TYPE getBlockTypeAtWorldCoords(const Vector3& worldCoords) const { return m_blocks[GetBlockIndexForWorldCoords(worldCoords)].getBlockType(); };
	Block* getBlockByIndex(int blockIndex);

	//Render
	void Render(const SpriteSheet* spriteSheet) const;
private:
	void GetQuadEWDir(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos, 
		const int& index, const Vector3& worldvsStartDiff);
	void GetQuadNSDir(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos,
		const int& index, const Vector3& worldvsStartDiff);
	void GetQuadTBDir(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos,
		const int& index, const Vector3& worldvsStartDiff);
	void GetQuadVertsAtBlock(std::vector<Vertex_PCT>& quadVerticesToRender, const SpriteSheet* spriteSheet, const BLOCK_TYPE& type, const Vector3& worldPos, const int& index);
public:
	void UpdateQuadVertices(const SpriteSheet* spriteSheet);
private:
	inline void getBottomVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords = AABB2(), const Rgba& lightValOfNeighborBelow = g_Renderer->s_White) const;
	inline void getTopVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords = AABB2(), const Rgba& lightValOfNeighborAbove = g_Renderer->s_White) const;
	inline void getTopUpsideDownVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords = AABB2(), const Rgba& lightValOfNeighborAbove = g_Renderer->s_White) const;
	inline void getNorthVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords = AABB2(), const Rgba& lightValOfNeighborNorth = g_Renderer->s_White) const;
	inline void getSouthVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords = AABB2(), const Rgba& lightValOfNeighborSouth = g_Renderer->s_White) const;
	inline void getWestVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords = AABB2(), const Rgba& lightValOfNeighborWest = g_Renderer->s_White) const;
	inline void getEastVertices(std::vector<Vertex_PCT>& quadVerticesToRender, const Vector3& position, const AABB2& texCoords = AABB2(), const Rgba& lightValOfNeighborEast = g_Renderer->s_White) const;

public:
	//Fast Line Of Sight testing
	const Vector3 getStartPosition() const;
	const Vector3 getEndPosition() const;

	//getters
	inline const int GetBlockIndexForWorldCoords(const Vector3& worldCoords) const;
	const int GetBlockIndexForLocalCoords(const IntVector3& localCoords) const;
	inline const IntVector3 GetLocalCoordsForIndex(const int& index) const;
	const IntVector3 GetLocalCoordsForWorldCoords(const Vector3& worldCoords) const;
	inline const Vector3 GetWorldCoordsForIndex(const int& index) const;
	const Vector3 GetWorldCoordsForLocalCoords(const IntVector3& localCoords) const;
	const Vector3 GetChunkCenter() const;
	const Vector3 GetChunkCenterGroundLevel() const;
	//testers
	const bool isLocalCoordsInChunk(const IntVector3& localCoords) const;
	//const bool testDirectoryExistsOrCreate() const;
	const bool testDirectoryExists() const;
	const Vector3 GetBlockSizes() const { return m_blockSize;};

	//Save and Load related
	const bool saveChunk() const;
	static Chunk* loadChunk(const IntVector2& chunkCell, int worldLighting = 15);
private:
	static const std::string GetFileName(const IntVector2& chunkCell);
public:
	//other chunks
	Chunk* EastChunk;
	Chunk* WestChunk;
	Chunk* NorthChunk;
	Chunk* SouthChunk;
	bool m_chunkDirty;

private:
	//variables
	//std::vector<Vertex_PCT> m_quadVerticesToRender;
	Vector3 m_startPosition;
	Vector3 m_endPosition;
	std::vector<Block> m_blocks;
	int m_worldLighting;
	//std::map<IntVector3, Block> m_blocks;
public:
	static const int CHUNK_BITS_X;
	static const int BLOCKS_WIDE_X;
	static const int LOCAL_X_MASK;

	static const int CHUNK_BITS_Y;
	static const int BITS_XY;
	static const int BLOCKS_WIDE_Y;
	static const int LOCAL_Y_MASK;
	static const int BLOCKS_PER_LAYER;

	static const int CHUNK_BITS_Z;
	static const int BLOCKS_WIDE_Z;
	static const int LOCAL_Z_MASK;
	static const int BLOCKS_PER_CHUNK;


private:
	static const Vector3 m_blockSize;
	static const std::string s_saveFolderLocation;
	static const int s_SandLevel;
	static const int s_DirtLevelAboveSand;
	static const int s_GrassLevelBelowTop;

	static const int s_NumOfAdditionBlocksBeforeGenerateSand;
	static const int s_NumOfAdditionalBlocksBeforeGenerateDirt;
	static const int s_NumOfAdditionalBlocksBeforeGenerateGrass;
	static const int s_NumOfAdditionalBlocksBeforeGenerateAir;
	static const int s_RandomAdditionalBlocksBeforeDirt;
	static const int s_RandomAdditionalBlocksBeforeGrass;
	static const int s_seaLevel;

	unsigned int m_vboID;
	int m_numOfVertices;
};


struct BlockInfo
{
	BlockInfo(const Chunk* chunk = nullptr, const int& blockIndex = -1);
	Block* GetBlock();
	BlockInfo GetNeighborAbove() const;
	BlockInfo GetNeighborBelow() const;
	BlockInfo GetNeighborEast() const;
	BlockInfo GetNeighborWest() const;
	BlockInfo GetNeighborNorth() const;
	BlockInfo GetNeighborSouth() const;
	Chunk* m_chunk;
	int m_blockIndex;
};

//Block_Hit
struct Block_Hit
{
	//Block_Hit(Chunk* chunk = nullptr, const int& blockHitIndex = -1, const int& airHit = -1, Chunk* airChunk = nullptr);
	Block_Hit(Vector3 worldPosHit = Vector3(), Vector3 worldPosBeforeHit = Vector3(), float fractionCompleted = 0.f);
	Block_Hit(const Block_Hit& other);


	//BlockInfo m_AirBlockInfo;
	//BlockInfo m_HitBlockInfo;
	Vector3 m_worldPosHit;
	Vector3 m_worldPosBeforeHit;
	float m_fractionCompleted;
	//Chunk* m_whereHit;
	//Chunk* m_airBlockChunk;
	//IntVector3 m_localCoordsOfBlockHit;
	//IntVector3 m_lastAirBlock;
};

#endif // !CHUNK_HPP


#pragma once
#include "Game/BlockTextureContainer.hpp"

#ifndef BLOCK_HPP
#define BLOCK_HPP

class Block
{
public:
	//constructors
	Block(const BLOCK_TYPE& type = AIR_BLOCK);

	//setters
	void setBlockType(const BLOCK_TYPE& type);
//	void setBit(unsigned char& bitFlags, unsigned char bitMask) { bitFlags |= bitMask; };
	inline void setLightValue(const int& lightValue0To15);
	inline void setOpaqueValue(const bool& isOpaque);
	void setSkyValue(const bool& isSky);
	void setDirtyBit(const bool& isDirty);
	void setBit(unsigned char& bitFlags, const unsigned char& bitMask) { bitFlags |= bitMask; };
	void clearBit(unsigned char& bitFlags, const unsigned char& bitMask) { bitFlags &= ~bitMask; };

	//getters
	const BLOCK_TYPE getBlockType() const {return (BLOCK_TYPE)m_type;};
	const unsigned char getBlockTypeAsChar() const { return m_type; };
	const unsigned char getLightValue() const { return m_lightingAndFlags & LIGHTING_MASK; };
	const bool isBitSet(unsigned char bitFlags, unsigned char bitMask) const { return (bitFlags& bitMask) != 0; };
	const bool isOpaqueSet() const { return (m_lightingAndFlags & OPAQUE_MASK) != 0; };
	const bool isSkySet() const { return (m_lightingAndFlags & SKY_MASK) != 0; };
	const bool isDirtySet() const { return (m_lightingAndFlags & DIRTY_MASK) != 0; };


private:
	void verifyOpacity();
	void verifyLightSource();

	//variables
	unsigned char m_type;
	unsigned char m_lightingAndFlags; //lighting vals 2 = just barely visible.

	static const int OPAQUE_BITS;
	static const int OPAQUE_BIT_SHIFTS;
	static const int OPAQUE_MASK;
	static const int SKY_BITS;
	static const int SKY_BIT_SHIFTS;
	static const int SKY_MASK;
	static const int DIRTY_BITS;
	static const int DIRTY_BIT_SHIFTS;
	static const int DIRTY_MASK;
	static const int LIGHTING_BITS;
	static const int MAX_LIGHTING_LEVEL;
	static const unsigned char LIGHTING_MASK;


};
#endif // !BLOCK_HPP

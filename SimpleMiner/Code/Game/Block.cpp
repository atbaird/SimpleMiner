#include "Game/Block.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


const int Block::LIGHTING_BITS = 4;
const int Block::MAX_LIGHTING_LEVEL = (1 << LIGHTING_BITS) - 1;
const unsigned char Block::LIGHTING_MASK = (unsigned char)MAX_LIGHTING_LEVEL;

const int Block::OPAQUE_BITS = 1;
const int Block::OPAQUE_BIT_SHIFTS = 3 + LIGHTING_BITS;
const int Block::OPAQUE_MASK = (OPAQUE_BITS) << OPAQUE_BIT_SHIFTS;
const int Block::SKY_BITS = 1;
const int Block::SKY_BIT_SHIFTS = 2 + LIGHTING_BITS;
const int Block::SKY_MASK = (SKY_BITS) << SKY_BIT_SHIFTS;
const int Block::DIRTY_BITS = 1;
const int Block::DIRTY_BIT_SHIFTS = 1 + LIGHTING_BITS;
const int Block::DIRTY_MASK = (DIRTY_BITS) << DIRTY_BIT_SHIFTS;


Block::Block(const BLOCK_TYPE& type)
: m_type((unsigned char)type), m_lightingAndFlags(0)
{
	if (type != AIR_BLOCK)
		verifyOpacity();
	setSkyValue(false);
	setLightValue(0);
	setDirtyBit(true);
}
void Block::setBlockType(const BLOCK_TYPE& type) 
{ 
	m_type = (unsigned char)type;
	verifyOpacity();
}
void Block::setLightValue(const int& lightValue0To15)
{
	ASSERT_OR_DIE(lightValue0To15 >= 0 && lightValue0To15 <= MAX_LIGHTING_LEVEL, "Do not enter in a lightValue thats greater than 15 or less than 0!");
	m_lightingAndFlags &= ~LIGHTING_MASK; //11110000
	m_lightingAndFlags |= (unsigned char)(lightValue0To15);
}

void Block::setOpaqueValue(const bool& isOpaque)
{
	m_lightingAndFlags &= ~OPAQUE_MASK;
	if (isOpaque == true)
	{
		m_lightingAndFlags |= OPAQUE_MASK;
	}
}
void Block::setSkyValue(const bool& isSky)
{
	m_lightingAndFlags &= ~SKY_MASK;
	if (isSky == true)
	{
		m_lightingAndFlags |= SKY_MASK;
	}
}

void Block::setDirtyBit(const bool& isDirty)
{
	m_lightingAndFlags &= ~DIRTY_MASK;
	if (isDirty == true)
	{
		m_lightingAndFlags |= DIRTY_MASK;
	}
}
void Block::verifyOpacity()
{
	setOpaqueValue(g_blockInfoContainers[m_type].opague);
}
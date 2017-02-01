#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Renderer.hpp"

#ifndef SPRITEANIM_HPP
#define SPRITEANIM_HPP

enum SpriteAnimMode
{
	SPRITE_ANIM_MODE_PLAY_TO_END = 0, //play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,     //Play from time=0 to end then repeat (never finish)
	NUM_SPRITE_ANIM_MODES
};

class SpriteAnim
{
public:
	SpriteAnim();
	SpriteAnim(SpriteSheet* spriteSheet, const float& durationSeconds, const SpriteAnimMode& mode, const int& startSpriteIndex, const int& endSpriteIndex);

	void Update(const float& deltaSeconds);
	const void Render(const AABB2& positionToDraw, const float& m_angularDisplacementDegrees, const Rgba& tint);
	const AABB2 GetCurrentTexCoords() const; //based on the current elapsed time
	const Texture* GetTexture() const;
	void Pause();
	void Resume();
	void Reset();
	void Reverse();
	const bool IsFinished() const { return m_isFinished; };
	const bool IsPlaying() const { return m_isPlaying; };
	const bool IsReversed() const { return m_reverse; };
	const float GetDurationSeconds() const { return m_durationSeconds; };
	const float GetSecondsElapsed() const { return m_elapsedSeconds; };
	const float GetSecondsRemaining() const;
	const float GetFractionElapsed() const;
	const float GetFractionRemaining() const;
	const float SetSecondsElapsed(const float& secondsElapsed); // jump to specific time
	const float SetFractionElapsed(const float& fractionElapsed); //e.g. 0.33f for one-third in
	const void setDurationSeconds(float seconds);

	SpriteSheet* m_spriteSheet;
	SpriteAnimMode m_mode;
	float m_durationSeconds;
	float m_elapsedSeconds;
	float m_elapsedSecondsSinceLastFrameSwitch;
	float m_durationSecondsBetweenFrames;
	int m_startSpriteIndex;
	int m_endSpriteIndex;
	int m_currentSpriteIndex;
	bool m_isFinished;
	bool m_isPlaying;
	bool m_reverse;
};

#endif
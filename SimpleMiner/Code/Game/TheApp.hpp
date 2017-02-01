#pragma once
#include "Game/TheGame.hpp"



#ifndef THEAPP_HPP
#define THEAPP_HPP
//-----------------------------------------------------
class TheApp;

//-----------------------------------------------------
extern TheApp* g_theApp;

//-----------------------------------------------------
class TheApp
{
public:
	TheApp();
	void Update(const float& deltaSeconds);
	void OnGainedFocus() {m_focus = true;};
	void OnLostFocus() {m_focus = false;};
	bool m_focus;
};

#endif // !THEAPP_HPP



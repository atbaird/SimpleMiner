#include "Game/TheApp.hpp"

//--------------------------------------------
TheApp* g_theApp = nullptr;

//--------------------------------------------
TheApp::TheApp() : m_focus(true)
{
}


void TheApp::Update(const float& deltaSeconds)
{
	if (m_focus)
	{
		g_TheGame->Update(deltaSeconds, m_focus);
	}
	else
	{
		g_TheGame->Update(0.f, m_focus);
	}
}
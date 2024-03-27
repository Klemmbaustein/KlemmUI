#include "UIManager.h"
#include <GL/glew.h>
#include <KlemmUI/Window.h>
#include <KlemmUI/UI/UIBox.h>
using namespace KlemmUI;

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
	ClearUI();
	glDeleteFramebuffers(1, &UIBuffer);
	glDeleteTextures(1, &UITexture);
}

void UIManager::ForceUpdateUI()
{
	if (UIBuffer)
	{
		glDeleteFramebuffers(1, &UIBuffer);
		glDeleteTextures(1, &UITexture);
	}
	UIBuffer = 0;
	UITexture = 0;
	InitUI();
	for (UIBox* i : UIElements)
	{
		if (!i->GetParent())
		{
			i->InvalidateLayout();
		}
	}
}

void UIManager::InitUI()
{
	Window* CurrentWindow = Window::GetActiveWindow();
	glGenFramebuffers(1, &CurrentWindow->UI.UIBuffer);
	// create floating point color buffer
	glGenTextures(1, &CurrentWindow->UI.UITexture);
	glBindTexture(GL_TEXTURE_2D, CurrentWindow->UI.UITexture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA16F,
		Window::GetActiveWindow()->GetSize().X,
		Window::GetActiveWindow()->GetSize().Y,
		0,
		GL_RGBA,
		GL_FLOAT,
		NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, CurrentWindow->UI.UIBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CurrentWindow->UI.UITexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int UIManager::GetUIFramebuffer() const
{
	return UITexture;
}

void UIManager::RedrawUI()
{
	RequiresRedraw = true;
}

void UIManager::ClearUI()
{
	ElementsToUpdate.clear();
	for (UIBox* elem : UIElements)
	{
		if (!elem->GetParent())
		{
			delete elem;
		}
	}
	UIElements.clear();
	RedrawUI();
}

bool UIManager::DrawElements()
{
	NewHoveredBox = nullptr;

	for (UIBox* elem : UIElements)
	{
		if (elem->IsVisible != elem->PrevIsVisible)
		{
			RedrawUI();
			elem->PrevIsVisible = elem->IsVisible;
		}
		if (elem->ShouldBeTicked)
		{
			elem->Tick();
		}
		if (!elem->Parent)
		{
			elem->UpdateHoveredState();
		}
	}
	HoveredBox = NewHoveredBox;

	if (RequiresRedraw)
	{
		RequiresRedraw = false;
		for (UIBox* elem : ElementsToUpdate)
		{
			elem->UpdateSelfAndChildren();
		}
		ElementsToUpdate.clear();
		glViewport(0, 0, Window::GetActiveWindow()->GetSize().X, Window::GetActiveWindow()->GetSize().Y);
		glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		for (UIBox* elem : UIElements)
		{
			if (elem->Parent == nullptr)
				elem->DrawThisAndChildren();
		}
		glViewport(0, 0, Window::GetActiveWindow()->GetSize().X, Window::GetActiveWindow()->GetSize().Y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}
	return false;
}

void KlemmUI::UIManager::UpdateEvents()
{
	auto Events = ButtonEvents;
	ButtonEvents.clear();

	for (auto& e : Events)
	{
		if (e.Function)
			e.Function();
		if (e.FunctionIndex)
			e.FunctionIndex(e.Index);
		if (e.Btn)
			e.Btn->OnChildClicked(e.Index);
	}
}

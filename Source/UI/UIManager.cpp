#include <KlemmUI/UI/UIManager.h>
#include <GL/glew.h>
#include <KlemmUI/Window.h>
#include <KlemmUI/UI/UIBox.h>
#include <iostream>
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
	RedrawUI();
}

void UIManager::InitUI()
{
	Window* CurrentWindow = Window::GetActiveWindow();
	glGenFramebuffers(1, &UIBuffer);
	// create floating point color buffer
	glGenTextures(1, &UITexture);
	glBindTexture(GL_TEXTURE_2D, UITexture);
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

	glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, UITexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int UIManager::GetUIFramebuffer() const
{
	return UITexture;
}

void UIManager::RedrawUI()
{
	RedrawArea(RedrawBox{
			.Min = -1,
			.Max = 1,
		});
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

bool UIManager::GetShouldRedrawUI() const
{
	return RequiresRedraw;
}

bool UIManager::DrawElements()
{

	NewHoveredBox = nullptr;

	for (UIBox* elem : UIElements)
	{
		if (elem->IsVisible != elem->PrevIsVisible)
		{
			elem->RedrawElement();
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

	if (!ElementsToUpdate.empty())
	{
		for (UIBox* elem : ElementsToUpdate)
		{
			elem->UpdateSelfAndChildren();
		}
		ElementsToUpdate.clear();
	}

	if (!RedrawBoxes.empty())
	{
		glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
		glClearColor(0, 0, 0, 1);
		glEnable(GL_SCISSOR_TEST);
		glViewport(0, 0, Window::GetActiveWindow()->GetSize().X, Window::GetActiveWindow()->GetSize().Y);
		for (auto& i : RedrawBoxes)
		{
			Vector2ui Pos = Vector2f(i.Min / 2 + 0.5f) * Vector2f(Window::GetActiveWindow()->GetSize());
			Vector2ui Res = Vector2f((i.Max - i.Min) / 2) * Vector2f(Window::GetActiveWindow()->GetSize());

			glScissor(Pos.X, Pos.Y, Res.X + 1, Res.Y + 1);
			glClear(GL_COLOR_BUFFER_BIT);
			for (UIBox* elem : UIElements)
			{
				if (elem->Parent == nullptr)
					elem->DrawThisAndChildren(i);
			}
		}
		glDisable(GL_SCISSOR_TEST);
		glScissor(0, 0, Window::GetActiveWindow()->GetSize().X, Window::GetActiveWindow()->GetSize().Y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		RedrawBoxes.clear();
		return true;
	}
	return false;
}

void UIManager::UpdateEvents()
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

static UIManager::RedrawBox CombineBoxes(const UIManager::RedrawBox& BoxA, const UIManager::RedrawBox& BoxB)
{
	return UIManager::RedrawBox{
		.Min = Vector2f::Min(BoxA.Min, BoxB.Min),
		.Max = Vector2f::Max(BoxA.Max, BoxB.Max),
	};
}

void UIManager::RedrawArea(RedrawBox Box)
{
	// Do nto redraw the element if it's position has not yet been initialized.
	// It will be redrawn once the position has been set anyways.
	if (std::isnan(Box.Min.X) || std::isnan(Box.Min.Y)
		|| std::isnan(Box.Max.X) || std::isnan(Box.Max.Y))
	{
		return;
	}

	RedrawBox* CurrentBox = &Box;
	size_t CurrentBoxIndex = SIZE_MAX;
	for (size_t i = 0; i < RedrawBoxes.size(); i++)
	{
		RedrawBox& IteratedBox = RedrawBoxes[i];
		if (RedrawBox::IsBoxOverlapping(*CurrentBox, IteratedBox))
		{
			// If 2 overlapping redraw areas are given, we combine them to avoid redrawing the same area twice.
			IteratedBox = CombineBoxes(*CurrentBox, IteratedBox);

			if (CurrentBoxIndex != SIZE_MAX)
			{
				RedrawBoxes.erase(RedrawBoxes.begin() + CurrentBoxIndex);
				i--;
			}

			CurrentBox = &IteratedBox;
			CurrentBoxIndex = i;
		}
	}

	if (CurrentBoxIndex == SIZE_MAX)
	{
		RedrawBoxes.push_back(Box);
	}
}

bool UIManager::RedrawBox::IsBoxOverlapping(const UIManager::RedrawBox& BoxA, const UIManager::RedrawBox& BoxB)
{
	return (BoxA.Min.X <= BoxB.Max.X && BoxA.Max.X >= BoxB.Min.X) &&
		(BoxA.Min.Y <= BoxB.Max.Y && BoxA.Max.Y >= BoxB.Min.Y);
}

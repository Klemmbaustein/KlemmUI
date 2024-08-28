#include <KlemmUI/UI/UIManager.h>
#include <GL/glew.h>
#include <KlemmUI/Window.h>
#include <KlemmUI/UI/UIBox.h>
#include <KlemmUI/Image.h>
#include <iostream>
#include <KlemmUI/Resource.h>
#include <algorithm>
using namespace KlemmUI;

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
	ClearUI();
	glDeleteFramebuffers(1, &UIBuffer);
	glDeleteTextures(1, &UITexture);

	for (auto& i : ReferencedTextures)
	{
		Image::UnloadImage(i.first);
	}
	ReferencedTextures.clear();
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
	glGenFramebuffers(1, &UIBuffer);
	// create floating point color buffer
	glGenTextures(1, &UITexture);
	glBindTexture(GL_TEXTURE_2D, UITexture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA16F,
		(GLsizei)Window::GetActiveWindow()->GetSize().X,
		(GLsizei)Window::GetActiveWindow()->GetSize().Y,
		0,
		GL_RGBA,
		GL_FLOAT,
		NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, UITexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RedrawUI();
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
	while (UIElements.size())
	{
		delete UIElements[0];
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
	TickElements();

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

		Vector2ui WindowSize = Window::GetActiveWindow()->GetSize();

		glViewport(0, 0, (GLint)WindowSize.X, (GLint)WindowSize.Y);
		for (auto& i : RedrawBoxes)
		{
			i.Max += Vector2f(3) / Vector2f(WindowSize);
			i.Min = i.Min - Vector2f(3) / Vector2f(WindowSize);

			i.Min = i.Min.Clamp(-1, 1);
			i.Max = i.Max.Clamp(-1, 1);

			Vector2f Pos = (i.Min / 2 + 0.5f) * Vector2f(WindowSize);
			Vector2f Res = (i.Max - i.Min) / 2 * Vector2f(WindowSize);

			glScissor(
				(GLsizei)Pos.X,
				(GLsizei)Pos.Y,
				std::clamp((GLsizei)Res.X, 0, (GLsizei)WindowSize.X),
				std::clamp((GLsizei)Res.Y, 0, (GLsizei)WindowSize.Y)
			);
			glClear(GL_COLOR_BUFFER_BIT);
			for (UIBox* elem : UIElements)
			{
				if (elem->Parent == nullptr)
					elem->DrawThisAndChildren(i);
			}
		}
		glDisable(GL_SCISSOR_TEST);
		glScissor(0, 0, (GLsizei)Window::GetActiveWindow()->GetSize().X, (GLsizei)Window::GetActiveWindow()->GetSize().Y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		RedrawBoxes.clear();
		return true;
	}
	return false;
}

void KlemmUI::UIManager::TickElements()
{
	NewHoveredBox = nullptr;
	for (UIBox* elem : UIElements)
	{
		if (elem->IsVisible != elem->PrevIsVisible)
		{
			elem->RedrawElement(true);
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
}

void UIManager::UpdateEvents()
{
	std::vector<ButtonEvent> Events = ButtonEvents;
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

unsigned int KlemmUI::UIManager::LoadReferenceTexture(std::string FilePath)
{
	for (auto& i : ReferencedTextures)
	{
		if (i.second.Name == FilePath)
		{
			i.second.RefCount++;
			return i.first;
		}
	}

	if (!TexturePath.empty() && !Resource::ResourceExists(FilePath))
	{
		FilePath = TexturePath + "/" + FilePath;
	}

	unsigned int NewTexture = Image::LoadImage(FilePath);
	ReferencedTextures.insert(std::pair(NewTexture, ReferenceTexture{
		.Name = FilePath,
		.RefCount = 1,
		}));

	return NewTexture;
}

void KlemmUI::UIManager::UnloadReferenceTexture(unsigned int TextureID)
{
	auto Texture = ReferencedTextures.find(TextureID);

	if (Texture == ReferencedTextures.end())
	{
		return;
	}

	Texture->second.RefCount--;
	if (Texture->second.RefCount == 0)
	{
		Image::UnloadImage(Texture->first);
		ReferencedTextures.erase(Texture);
	}
}

void KlemmUI::UIManager::SetTexturePath(std::string NewPath)
{
	TexturePath = NewPath;
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

	if (RedrawBoxes.size() > 8)
	{
		RedrawBoxes.clear();
		RedrawUI();
	}

	for (size_t i = 0; i < RedrawBoxes.size(); i++)
	{
		RedrawBox& IteratedBox = RedrawBoxes[i];

		if (IteratedBox.Min == Box.Min && IteratedBox.Max == Box.Max)
		{
			return;
		}

		if (RedrawBox::IsBoxOverlapping(*CurrentBox, IteratedBox))
		{
			// If 2 overlapping redraw areas are given, we combine them to avoid redrawing the same area twice.
			IteratedBox = CombineBoxes(*CurrentBox, IteratedBox);
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

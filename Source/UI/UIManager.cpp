#include <kui/UI/UIManager.h>
#include "../Internal/OpenGL.h"
#include <kui/Window.h>
#include <kui/UI/UIBox.h>
#include <kui/UI/UIBlurBackground.h>
#include <kui/Image.h>
#include <kui/Resource.h>
#include <algorithm>
#include <iostream>
using namespace kui;

thread_local bool UIManager::UseAlphaBuffer = false;

UIManager::UIManager()
{
	UITextures[0] = 0;
	UITextures[1] = 0;
}

UIManager::~UIManager()
{
	UIBackground::FreeVertexBuffer();
	ClearUI();
	GLsizei NumBuffers = UseAlphaBuffer ? 2 : 1;
	glDeleteFramebuffers(1, &UIBuffer);
	glDeleteTextures(NumBuffers, UITextures);

	for (auto& i : ReferencedTextures)
	{
		image::UnloadImage(i.first);
	}
	ReferencedTextures.clear();
}

void UIManager::ForceUpdateUI()
{
	if (UIBuffer)
	{
		glDeleteFramebuffers(1, &UIBuffer);
		GLsizei NumBuffers = UseAlphaBuffer ? 2 : 1;
		glDeleteTextures(NumBuffers, UITextures);
	}
	UIBuffer = 0;
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

	GLsizei NumBuffers = UseAlphaBuffer ? 2 : 1;

	glGenTextures(NumBuffers, UITextures);
	glBindTexture(GL_TEXTURE_2D, UITextures[0]);

	GLsizei x = (GLsizei)Window::GetActiveWindow()->GetSize().X, y = (GLsizei)Window::GetActiveWindow()->GetSize().Y;
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA16F,
		x,
		y,
		0,
		GL_RGBA,
		GL_FLOAT,
		NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, UITextures[0], 0);

	if (UseAlphaBuffer)
	{
		glBindTexture(GL_TEXTURE_2D, UITextures[1]);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			x,
			y,
			0,
			GL_RGBA,
			GL_FLOAT,
			NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, UITextures[1], 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RedrawUI();
}

unsigned int UIManager::GetUIFramebuffer() const
{
	return UITextures[0];
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

static UIManager::RedrawBox CombineBoxes(const UIManager::RedrawBox& BoxA, const UIManager::RedrawBox& BoxB)
{
	return UIManager::RedrawBox{
		.Min = Vec2f::Min(BoxA.Min, BoxB.Min),
		.Max = Vec2f::Max(BoxA.Max, BoxB.Max),
	};
}

bool UIManager::DrawElements()
{
	TickElements();

	if (!ElementsToUpdate.empty())
	{
		for (UIBox* Element : ElementsToUpdate)
		{
			Element->UpdateElement();
		}
		ElementsToUpdate.clear();
	}

	if (!RedrawBoxes.empty())
	{
		glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
		glClearColor(0, 0, 0, 0);
		glEnable(GL_SCISSOR_TEST);
		if (UseAlphaBuffer)
		{
			unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(2, attachments);
		}
		Vec2ui WindowSize = Window::GetActiveWindow()->GetSize();

		glViewport(0, 0, (GLint)WindowSize.X, (GLint)WindowSize.Y);
		for (auto& i : RedrawBoxes)
		{
			for (UIBlurBackground* bg : UIBlurBackground::BlurBackgrounds)
			{
				RedrawBox BackgroundBox = bg->GetRedrawBox();

				if (RedrawBox::IsBoxOverlapping(i, BackgroundBox))
				{
					i = CombineBoxes(i, BackgroundBox);
				}
			}

			i.Max += Vec2f(5) / Vec2f(WindowSize);
			i.Min = i.Min - Vec2f(5) / Vec2f(WindowSize);

			i.Min = i.Min.Clamp(-1, 1);
			i.Max = i.Max.Clamp(-1, 1);

			Vec2f Pos = (i.Min / 2 + 0.5f) * Vec2f(WindowSize);
			Vec2f Res = (i.Max - i.Min) / 2 * Vec2f(WindowSize);

			ScissorXY = Vec2ui(uint64_t(Pos.X), uint64_t(Pos.Y));
			ScissorWH = Vec2ui(
				std::clamp((GLsizei)Res.X + 1, 0, (GLsizei)WindowSize.X),
				std::clamp((GLsizei)Res.Y + 1, 0, (GLsizei)WindowSize.Y)
			);

			glScissor(
				GLint(ScissorXY.X),
				GLint(ScissorXY.Y),
				GLsizei(ScissorWH.X),
				GLsizei(ScissorWH.Y)
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

void kui::UIManager::TickElements()
{
	NewHoveredBox = nullptr;
	for (size_t i = 0; i < UIElements.size(); i++)
	{
		UIBox* elem = UIElements[i];
		if (elem->IsVisible != elem->PrevIsVisible)
		{
			elem->RedrawElement(true);
			if (!elem->IsVisible && Window::GetActiveWindow()->UI.KeyboardFocusBox == elem)
			{
				Window::GetActiveWindow()->UI.KeyboardFocusBox = nullptr;
			}
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
	}
}

unsigned int kui::UIManager::LoadReferenceTexture(std::string FilePath)
{
	for (auto& i : ReferencedTextures)
	{
		if (i.second.Name == FilePath)
		{
			i.second.RefCount++;
			return i.first;
		}
	}

	if (!TexturePath.empty() && !resource::FileExists(FilePath))
	{
		FilePath = TexturePath + "/" + FilePath;
	}

	unsigned int NewTexture = image::LoadImage(FilePath);
	ReferencedTextures.insert(std::pair(NewTexture, ReferenceTexture{
		.Name = FilePath,
		.RefCount = 1,
		}));

	return NewTexture;
}

void kui::UIManager::UnloadReferenceTexture(unsigned int TextureID)
{
	auto Texture = ReferencedTextures.find(TextureID);

	if (Texture == ReferencedTextures.end())
	{
		return;
	}

	Texture->second.RefCount--;
	if (Texture->second.RefCount == 0)
	{
		image::UnloadImage(Texture->first);
		ReferencedTextures.erase(Texture);
	}
}

void kui::UIManager::SetTexturePath(std::string NewPath)
{
	TexturePath = NewPath;
}

UIBox* kui::UIManager::GetNextKeyboardBox(UIBox* From, bool Reverse)
{
	bool Found = false;
	auto Iterate = [&Found, &From, Reverse, this](UIBox* Box) -> UIBox*
		{
			if (Box == From)
			{
				Found = true;
				return nullptr;
			}

			if (Found)
			{
				UIBox* FoundBox = FindKeyboardBox(Box, Reverse);

				if (FoundBox)
					return FoundBox;
			}
			return nullptr;
		};

	bool Direction = From->Parent && From->Parent->VerticalBoxAlign != UIBox::Align::Reverse && !From->Parent->ChildrenHorizontal;

	if (Reverse)
		Direction = !Direction;

	if (Direction && From->Parent)
	{
		for (int64_t i = From->Parent->Children.size() - 1; i >= 0; i--)
		{
			UIBox* FoundBox = Iterate(From->Parent->Children[i]);
			if (FoundBox)
				return FoundBox;
		}
	}
	else if (From->Parent)
	{
		for (UIBox* Box : From->Parent->Children)
		{
			UIBox* FoundBox = Iterate(Box);
			if (FoundBox)
				return FoundBox;
		}
	}

	if (From->Parent)
	{
		return GetNextKeyboardBox(From->Parent, Reverse);
	}
	return nullptr;
}

UIBox* kui::UIManager::FindKeyboardBox(UIBox* From, bool Reverse)
{
	if (!From->IsVisibleInHierarchy())
		return nullptr;
	if (From->KeyboardFocusable && From->IsChildOf(From->ParentWindow->Input.KeyboardFocusTargetBox))
		return From;

	bool Direction = From->VerticalBoxAlign != UIBox::Align::Reverse && !From->ChildrenHorizontal;

	if (Reverse)
		Direction = !Direction;

	if (Direction)
	{
		for (int64_t i = From->Children.size() - 1; i >= 0; i--)
		{
			UIBox* ChildResult = FindKeyboardBox(From->Children[i], Reverse);
			if (ChildResult)
				return ChildResult;
		}
	}
	else
	{
		for (UIBox* Box : From->Children)
		{
			UIBox* ChildResult = FindKeyboardBox(Box, Reverse);
			if (ChildResult)
				return ChildResult;
		}
	}
	return nullptr;
}

UIBox* kui::UIManager::GetNextFocusableBox(UIBox* From, bool Reverse)
{
	if (From)
	{
		UIBox* Next = GetNextKeyboardBox(From, Reverse);
		if (Next)
			return Next;
	}

	for (int WithParent = 0; WithParent < 2; WithParent++)
	{
		bool Found = false;

		auto Iterate = [this, &From, &WithParent, &Found, Reverse](UIBox* Box) -> UIBox*
			{
				if (Box->Parent)
					return nullptr;
				if (!WithParent && From && (From->IsChildOf(Box) || From == Box))
				{
					Found = true;
					return nullptr;
				}

				if (!Found && !WithParent)
					return nullptr;

				UIBox* FoundBox = FindKeyboardBox(Box, Reverse);

				if (FoundBox && From != FoundBox)
					return FoundBox;

				return nullptr;
			};

		if (Reverse)
		{
			for (int64_t i = UIElements.size() - 1; i >= 0; i--)
			{
				UIBox* ElementResult = Iterate(UIElements[i]);
				if (ElementResult)
					return ElementResult;
			}
		}
		else
		{
			for (UIBox* Box : UIElements)
			{
				UIBox* ElementResult = Iterate(Box);
				if (ElementResult)
					return ElementResult;
			}
		}
	}
	return nullptr;
}

void UIManager::RedrawArea(RedrawBox Box)
{
	// Do not redraw the element if it's position has not yet been initialized.
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

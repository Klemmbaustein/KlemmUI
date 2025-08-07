#include <kui/UI/UIManager.h>
#include <kui/Window.h>
#include <kui/UI/UIBox.h>
#include <kui/UI/UIBlurBackground.h>
#include <kui/Image.h>
#include <kui/Resource.h>
#include <algorithm>
#include <iostream>
#include <kui/Rendering/OpenGLBackend.h>
using namespace kui;

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
	UIBackground::FreeVertexBuffer();
	ClearUI();
	delete Render;
}

void UIManager::ForceUpdateUI()
{
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
	if (!Render)
	{
		Render = new render::OpenGLBackend();
	}
	if (!RenderInitialized)
	{
		Render->Initialize(Window::GetActiveWindow());
		RenderInitialized = true;
	}
	Render->CreateBuffer(Window::GetActiveWindow()->GetSize());
	RedrawUI();
}

unsigned int UIManager::GetUIFramebuffer() const
{
	return 0;
}

void UIManager::RedrawUI()
{
	RedrawArea(render::RedrawBox{
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

	for (auto& i : ReferencedTextures)
	{
		image::UnloadImage(i.first);
	}
	ReferencedTextures.clear();

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
		for (UIBox* Element : ElementsToUpdate)
		{
			Element->UpdateElement();
		}
		ElementsToUpdate.clear();
	}

	if (!RedrawBoxes.empty())
	{
		Window* Target = Window::GetActiveWindow();
		Render->BeginFrame(Target);
		for (auto& i : RedrawBoxes)
		{
			for (UIBlurBackground* bg : UIBlurBackground::BlurBackgrounds)
			{
				render::RedrawBox BackgroundBox = bg->GetRedrawBox();

				if (render::RedrawBox::IsBoxOverlapping(i, BackgroundBox))
				{
					i = render::RedrawBox::CombineBoxes(i, BackgroundBox);
				}
			}

			Render->BeginArea(Target, i);

			for (UIBox* elem : UIElements)
			{
				if (elem->Parent == nullptr)
					elem->DrawThisAndChildren(Render, i);
			}
		}
		Render->EndFrame(Target);
		RedrawBoxes.clear();
		return true;
	}
	return false;
}

void kui::UIManager::TickElements()
{
	NewHoveredBox = nullptr;
	for (std::size_t i = 0; i < UIElements.size(); i++)
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

void UIManager::RedrawArea(render::RedrawBox Box)
{
	// Do not redraw the element if it's position has not yet been initialized.
	// It will be redrawn once the position has been set anyways.
	if (std::isnan(Box.Min.X) || std::isnan(Box.Min.Y)
		|| std::isnan(Box.Max.X) || std::isnan(Box.Max.Y))
	{
		return;
	}

	render::RedrawBox* CurrentBox = &Box;
	std::size_t CurrentBoxIndex = SIZE_MAX;

	if (RedrawBoxes.size() > 8)
	{
		RedrawBoxes.clear();
		RedrawUI();
	}

	for (std::size_t i = 0; i < RedrawBoxes.size(); i++)
	{
		render::RedrawBox& IteratedBox = RedrawBoxes[i];

		if (IteratedBox.Min == Box.Min && IteratedBox.Max == Box.Max)
		{
			return;
		}

		if (render::RedrawBox::IsBoxOverlapping(*CurrentBox, IteratedBox))
		{
			// If 2 overlapping redraw areas are given, we combine them to avoid redrawing the same area twice.
			IteratedBox = render::RedrawBox::CombineBoxes(*CurrentBox, IteratedBox);
			CurrentBox = &IteratedBox;
			CurrentBoxIndex = i;
		}
	}

	if (CurrentBoxIndex == SIZE_MAX)
	{
		RedrawBoxes.push_back(Box);
	}
}

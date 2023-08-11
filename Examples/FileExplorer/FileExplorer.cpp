#include <KlemmUI/Application.h>
#include <KlemmUI/UI/UIText.h>
#include <KlemmUI/UI/UITextField.h>
#include <KlemmUI/UI/UIScrollBox.h>
#include <KlemmUI/UI/UIButton.h>
#include <filesystem>

namespace FileExplorer
{	
	UIButtonStyle* BrowserButtonStyle = new UIButtonStyle("BrowserButton");

	UITextField* PathText = nullptr;
	std::filesystem::path CurrentPath;
	UIScrollBox* FilesScrollBox = nullptr;
	std::vector<std::filesystem::path> Paths;
	TextRenderer* Font = nullptr;

	void SetPath(std::filesystem::path NewPath)
	{
		if (!std::filesystem::exists(NewPath))
		{
			std::u8string u8CurrentPath = CurrentPath.u8string();
			PathText->SetText(std::string(u8CurrentPath.begin(), u8CurrentPath.end()));
			return;
		}

		FilesScrollBox->GetScrollObject()->Percentage = 0;
		CurrentPath = NewPath;

		Paths.clear();
		FilesScrollBox->DeleteChildren();

		std::u8string u8CurrentPath = std::filesystem::absolute(NewPath).u8string();
		PathText->SetText(std::string(u8CurrentPath.begin(), u8CurrentPath.end()));

		int CurrentIndex = 0;
		for (const auto& File : std::filesystem::directory_iterator(CurrentPath))
		{
			std::u8string u8CurrentFile = File.path().filename().u8string();
			if (std::filesystem::is_directory(File))
			{
				u8CurrentFile.append(u8" (Directory)");
			}
			FilesScrollBox->AddChild((new UIButton(true, 0, BrowserButtonStyle, [](int Index)
				{
					if (std::filesystem::is_directory(Paths[Index]))
					{
						SetPath(Paths[Index]);
					}
					else
					{
						std::u8string u8CurrentFile = std::filesystem::absolute(Paths[Index]).u8string();
						std::string CurrentFile = std::string(u8CurrentFile.begin(), u8CurrentFile.end());
#if _WIN32
						system(("start \"\" \"" + CurrentFile + "\"").c_str());
#elif __linux
						system(("xdg-open \"" + CurrentFile + "\"").c_str());
#endif
					}
				}, CurrentIndex++))
				->SetMinSize(Vector2f(1.7, 0))
				->AddChild(new UIText(0.4f, 0, std::string(u8CurrentFile.begin(), u8CurrentFile.end()), FileExplorer::Font)));
			Paths.push_back(File);
		}
	}

	void GoBack()
	{
		std::u8string u8CurrentPath = std::filesystem::absolute(CurrentPath).u8string();
		std::string NewPath = std::string(u8CurrentPath.begin(), u8CurrentPath.end());

		if (NewPath.empty())
		{
			return;
		}
#if _WIN32
		std::string DirSeperator = "\\";
#else
		std::string DirSeperator = "/";
#endif

		size_t LastSlash = NewPath.find_last_of(DirSeperator);

		if (LastSlash == std::string::npos)
		{
			return;
		}

		// If the last part of the path is a slash
		if (LastSlash == NewPath.size() - 1)
		{
			NewPath.pop_back();
			LastSlash = NewPath.find_last_of(DirSeperator);
		}

		NewPath = NewPath.substr(0, LastSlash + 1);

		SetPath(NewPath);
	}
}


int main()
{
	Application::SetShaderPath("../../Shaders");

	Application::Initialize("Hello, World", 0);

	FileExplorer::Font = new TextRenderer("../Font.ttf");

	FileExplorer::BrowserButtonStyle->HoveredColor = Vector3f32(1, 1, 0.25);
	FileExplorer::BrowserButtonStyle->PressedColor = Vector3f32(1, 0.5, 0);

	FileExplorer::PathText = new UITextField(true, 0, Vector3f32(0.2f), FileExplorer::Font, []()
		{
			FileExplorer::SetPath(FileExplorer::PathText->GetText());
		});

	(new UIBox(true, Vector2f(-1, 0.9)))
		->AddChild((new UIButton(true, 0, 1, FileExplorer::GoBack))
			->AddChild(new UIText(0.5, 0, "<<", FileExplorer::Font)))
		->AddChild(FileExplorer::PathText);

	FileExplorer::PathText->SetMinSize(Vector2f(1.5, 0.08));

	FileExplorer::FilesScrollBox = new UIScrollBox(false, Vector2f(-1, -1), true);
	FileExplorer::FilesScrollBox
		->SetMinSize(Vector2f(2, 1.85))
		->SetMaxSize(Vector2f(2, 1.85))
		->BoxAlign = UIBox::Align::Reverse;


	FileExplorer::SetPath("./");

	while (!Application::Quit)
	{
		Application::UpdateWindow();
	}
}

int WinMain()
{
	return main();
}
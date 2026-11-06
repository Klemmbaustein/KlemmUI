module;

#include <kui/KlemmUI.h>
#include <kui/Markup/MarkupBox.h>
#include <kui/UI/UITextEditor.h>
#include <kui/UI/TextEditor.h>
#include <kui/UI/TextEditorHighlight.h>
#include <kui/UI/FileEditorProvider.h>

export module kui;

export namespace kui
{
	using kui::AnyContainer;
	export namespace markup
	{
		using kui::markup::MarkupBox;
	}

	export namespace app
	{
		using kui::app::MessageBox;
		using kui::app::MessageType;
		using kui::app::SelectFileDialog;
		using kui::app::YesNoBox;

		export namespace error
		{
			using kui::app::error::Error;
			using kui::app::error::SetErrorCallback;
		}
	}

	export namespace resource
	{
		using kui::resource::BinaryData;
		using kui::resource::UIResourceProvider;
		using kui::resource::FileExists;
		using kui::resource::FreeBinaryFile;
		using kui::resource::GetBinaryFile;
		using kui::resource::GetStringFile;
		using kui::resource::ResourceExists;
		using kui::resource::GetBinaryResource;
		using kui::resource::GetStringResource;
	}

	using kui::Window;

	using kui::UIBox;
	using kui::UIButton;
	using kui::UIBackground;
	using kui::UIBlurBackground;
	using kui::UIDropdown;
	using kui::UIScrollBox;
	using kui::Font;
	using kui::UIText;
	using kui::UITextField;
	using kui::TextSegment;
	using kui::UISpinner;

	using kui::Vec3;
	using kui::Vec2;
	using kui::UISize;
	using kui::SizeVec;

#ifdef KUI_EXPERIMENTAL_FILE_EDIT
	using kui::UITextEditor;
	using kui::ITextEditorProvider;
	using kui::EditorPosition;
	using kui::EditorColorizeSegment;
	using kui::HighlightSegment;
	using kui::HighlightMode;
	using kui::HighlightedArea;
	using kui::FileEditorProvider;
	using kui::EditorKeyword;
#endif
}
#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#pragma once
#include <kui/Font.h>

namespace kui
{
	class UITextEditor;

	class ITextEditorProvider
	{
	public:
		virtual void GetPreLine(size_t LineIndex, std::vector<TextSegment>& To) = 0;
		virtual void GetLine(size_t LineIndex, std::vector<TextSegment>& To) = 0;
		virtual size_t GetLineCount() = 0;
		virtual size_t GetPreLineSize() = 0;
		virtual void RemoveLines(size_t Start, size_t Length) = 0;
		virtual void SetLine(size_t Index, const std::vector<TextSegment>& NewLine) = 0;
		virtual void InsertLine(size_t Index, const std::vector<TextSegment>& Content) = 0;
		virtual void GetHighlightsForRange(size_t Begin, size_t Length) = 0;
		void UpdateLine(size_t Index, const std::vector<TextSegment>& NewContent);

		UITextEditor* ParentEditor = nullptr;
	};

	struct EditorPosition
	{
		size_t Column = 0;
		size_t Line = 0;

		bool operator==(const EditorPosition& other) const
		{
			return Line == other.Line && Column == other.Column;
		}
	};
}
#endif
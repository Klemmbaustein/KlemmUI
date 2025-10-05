#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#pragma once
#include <kui/UI/TextEditor.h>
#include <kui/UI/TextEditorHighlight.h>
#include <kui/UI/UIBackground.h>
#include <kui/UI/UIScrollBox.h>
#include <kui/UI/UIText.h>
#include <kui/Timer.h>
#include <set>
#include <mutex>

namespace kui
{
	class UITextEditor : public kui::UIBackground
	{
	public:
		UITextEditor(ITextEditorProvider* EditorProvider, Font* EditorFont);
		~UITextEditor() override;

		ITextEditorProvider* EditorProvider = nullptr;
		Font* EditorFont = nullptr;

		void UpdateContent();
		UIText* BuildChunk(size_t Position, size_t Length);
		void Update() override;

		void Tick() override;

		Vec2f CharSize = 0;

		EditorPosition ScreenToEditor(Vec2f Position);
		Vec2f EditorToScreen(EditorPosition Position);

		EditorPosition Insert(std::string NewString, EditorPosition At, bool Raw);
		void Erase(EditorPosition Begin, EditorPosition End);
		void Get(EditorPosition Begin, size_t Length, std::vector<TextSegment>& To, bool IncludeUnloaded = false);

		void NewLine();
		void EraseLine();

		void Edit();

		void FullRefresh();
		void DeleteChar();

		void DeleteSelection();
		void ClearSelection();

		void HighlightArea(const HighlightedArea& Area);
		void SnapHighlightToWord();

		void SetLine(size_t Index, const std::vector<TextSegment>& NewContent);

		void Draw(render::RenderBackend* With) override;

		void MoveCursor(int64_t Column, int64_t Line, bool DragSelection, bool SnapToWord);
		void SetCursorPosition(EditorPosition Position);
		EditorPosition GetCursorPosition() const;
		void ScrollTo(EditorPosition Position);
		std::string GetSelectedText();

		void ColorizeLine(size_t Line, const std::vector<EditorColorizeSegment>& Segments);

		EditorPosition SelectionStart;
		EditorPosition SelectionEnd;

		bool UpdateHighlights = false;

		std::mutex LinesMutex;

		friend struct HighlightedArea;

		void InsertNewLine(EditorPosition At);
		UIScrollBox* EditorScrollBox = nullptr;

		void SnapColumn(EditorPosition& Position);

		EditorPosition CharacterPosToGrid(EditorPosition CharacterPos, bool SnapToEnd = true);
		EditorPosition GridToCharacterPos(EditorPosition GridPos, bool SnapToEnd = true);

	private:

		Timer DoubleClickTimer;
		Timer CursorTimer;

		void UpdateSelectionBeam();

		void ClearEmptyLineEntries(size_t Index);
		bool IsLineLoaded(size_t Index);
		void AdjustSelection(EditorPosition& Position, bool DirectionForward);

		enum class SelectMode
		{
			Character,
			Word,
			Line,
		};

		void UpdateSelectionHighlights();

		std::vector<HighlightedArea> Highlighted;

		SelectMode SelectionMode = SelectMode::Character;

		void TickInput();

		bool IsEdited = false;
		bool RefreshText = false;
		bool DraggingSelection = false;

		size_t LinesStart = 0;
		size_t EditorLineSize = 25;
		struct LineEntry
		{
			std::vector<TextSegment> Data;
			size_t Length = 0;
		};
		LineEntry& GetLine(size_t Index);

		std::vector<LineEntry> Lines;
		UIBackground* SelectorBeam = nullptr;
	};
}
#endif
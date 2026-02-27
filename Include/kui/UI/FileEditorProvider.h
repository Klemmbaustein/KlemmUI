#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#pragma once
#include "TextEditor.h"
#include "TextEditorHighlight.h"
#include <utility>
#include <set>
#include <stack>

namespace kui
{
	class FileEditorProvider : public ITextEditorProvider
	{
	public:

		FileEditorProvider(std::string Path);
		FileEditorProvider(std::istream& Stream);
		FileEditorProvider();

		void GetPreLine(size_t LineIndex, std::vector<TextSegment>& To) override;
		void GetLine(size_t LineIndex, std::vector<TextSegment>& To) override;
		size_t GetLineCount() override;
		size_t GetPreLineSize() override;
		void RemoveLines(size_t Start, size_t Length) override;
		void SetLine(size_t Index, const std::vector<TextSegment>& NewLine) override;
		void InsertLine(size_t Index, const std::vector<TextSegment>& Content) override;
		void GetHighlightsForRange(size_t Begin, size_t Length) override;
		void OnLoaded() override;
		void Update() override;

		void UpdateBracketAreas();

		void Undo();
		void Redo();
		void Commit() override;

		void LoadStream(std::istream& Stream);

		void DumpContent();
		std::string GetContent();

		Vec3f KeywordColor = Vec3f(1.0f, 0.2f, 0.5f);
		Vec3f TextColor = 1;
		Vec3f StringColor = Vec3f(0.5f, 1.0f, 0.2f);
		Vec3f NumberColor = Vec3f(0.5f, 0.7f, 1.0f);
		Vec3f LineNumberColor = 0.75f;
		Vec3f BacketAreaColor = 0.5f;
		Vec3f CommentColor = 0.5f;

		std::set<std::string> Keywords;
		bool HighlightComments = false;

		std::vector<std::pair<EditorPosition, EditorPosition>> BracketAreas;

	protected:
		std::vector<std::string> Lines;
	private:

		struct ChangePart
		{
			uint64_t Line;
			std::string Content;
			bool IsRemove = false;
			bool IsAdd = false;
		};

		struct Change
		{
			std::vector<ChangePart> Parts;
		};

		Change ApplyChange(const Change& Target);

		Change NextChange;

		std::stack<Change> Changes;
		std::stack<Change> UnDoneChanges;

	};
}
#endif
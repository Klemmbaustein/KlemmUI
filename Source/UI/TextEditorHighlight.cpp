#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#include <kui/UI/TextEditorHighlight.h>
#include <kui/UI/UITextEditor.h>
#include <kui/Window.h>

void kui::HighlightedArea::GenerateSegments(UITextEditor* Editor)
{
	Segments.clear();
	float LineSize = this->Size.Value == 0 ? Editor->CharSize.Y : this->Size.GetScreen().Y;

	if (Start.Line == End.Line)
	{
		if (Start.Column > End.Column)
		{
			std::swap(Start, End);
		}
		auto Position = Editor->EditorToScreen(Start);
		auto EndPosition = Editor->EditorToScreen(End);

		Segments.push_back(HighlightSegment{
			.Position = Position,
			.Size = EndPosition - Position + Vec2f(0, LineSize),
			});

		Editor->RedrawElement();
		return;
	}

	if (Start.Line > End.Line)
	{
		std::swap(Start, End);
	}

	if (this->Mode == HighlightMode::HighlightLines)
	{
		if (Start.Column != End.Column)
		{
			Start.Column = std::min(Start.Column, End.Column);
			End.Column = Start.Column;
		}

		Vec2f StartPos = Editor->EditorToScreen(Start);
		this->Segments.push_back(HighlightSegment{
			.Position = StartPos + Vec2f(Editor->CharSize.X / 3.0f, 0),
			.Size = Vec2f(
				(Size.Value == 0 ? 1_px : Size).GetScreen().X,
				(Editor->EditorToScreen(End) + Vec2f(0, Editor->CharSize.Y) - StartPos).Y),
			});

		return;
	}

	for (size_t i = Start.Line; i <= End.Line; i++)
	{
		if (!Editor->IsLineLoaded(i))
			continue;

		auto& Line = Editor->GetLine(i);

		if (i == Start.Line)
		{
			auto StartPosition = Editor->EditorToScreen(Start);
			EditorPosition EndEditorPos = Start;
			EndEditorPos.Column = Line.Length;
			auto EndPosition = Editor->EditorToScreen(EndEditorPos);
			EndPosition += Vec2f(Editor->CharSize.X, LineSize);

			Segments.push_back(HighlightSegment{
				.Position = StartPosition,
				.Size = EndPosition - StartPosition,
				});
			continue;
		}
		if (i == End.Line)
		{
			EditorPosition StartEditorPos = End;
			StartEditorPos.Column = 0;
			auto StartPosition = Editor->EditorToScreen(StartEditorPos);
			auto EndPosition = Editor->EditorToScreen(End) + Vec2f(0, LineSize);

			Segments.push_back(HighlightSegment{
				.Position = StartPosition,
				.Size = EndPosition - StartPosition,
				});
			continue;
		}

		EditorPosition Pos = EditorPosition(0, i);
		auto StartPosition = Editor->EditorToScreen(Pos);
		Pos.Column = Line.Length;
		auto EndPosition = Editor->EditorToScreen(Pos);
		EndPosition += Vec2f(Editor->CharSize.X, LineSize);

		Segments.push_back(HighlightSegment{
			.Position = StartPosition,
			.Size = EndPosition - StartPosition,
			});
	}

	Editor->RedrawElement();
}
#endif
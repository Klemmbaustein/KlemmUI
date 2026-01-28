#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#include <kui/UI/UITextEditor.h>
#include <kui/UI/UIBackground.h>
#include <kui/Window.h>
#include <kui/Rendering/OpenGLBackend.h>
#include "../Rendering/VertexBuffer.h"
#include "../Rendering/OpenGL.h"
#include <iostream>
#include <algorithm>

using namespace kui;

static thread_local UITextEditor* CurrentEditor = nullptr;

static void OnTextEditorBackspace(Window* WithWindow)
{
	if (CurrentEditor)
	{
		CurrentEditor->DeleteChar();
	}
}

static void OnTextEditorDelete(Window* WithWindow)
{
	if (CurrentEditor)
	{
		if (CurrentEditor->SelectionStart == CurrentEditor->SelectionEnd)
		{
			CurrentEditor->SelectionStart.Column++;
			CurrentEditor->SelectionEnd.Column++;
		}
		CurrentEditor->DeleteChar();
	}
}

static void OnTextEditorUp(Window* WithWindow)
{
	if (CurrentEditor)
	{
		CurrentEditor->MoveCursor(0, -1,
			WithWindow->Input.IsKeyDown(Key::SHIFT), WithWindow->Input.IsKeyDown(Key::CTRL));
		CurrentEditor->ScrollTo(CurrentEditor->SelectionEnd);
	}
}

static void OnTextEditorDown(Window* WithWindow)
{
	if (CurrentEditor)
	{
		CurrentEditor->MoveCursor(0, 1,
			WithWindow->Input.IsKeyDown(Key::SHIFT), WithWindow->Input.IsKeyDown(Key::CTRL));
		CurrentEditor->ScrollTo(CurrentEditor->SelectionEnd);
	}
}

static void OnTextEditorLeft(Window* WithWindow)
{
	if (CurrentEditor)
	{
		CurrentEditor->MoveCursor(-1, 0,
			WithWindow->Input.IsKeyDown(Key::SHIFT), WithWindow->Input.IsKeyDown(Key::CTRL));
		CurrentEditor->ScrollTo(CurrentEditor->SelectionEnd);
	}
}

static void OnTextEditorRight(Window* WithWindow)
{
	if (CurrentEditor)
	{
		CurrentEditor->MoveCursor(1, 0,
			WithWindow->Input.IsKeyDown(Key::SHIFT), WithWindow->Input.IsKeyDown(Key::CTRL));
		CurrentEditor->ScrollTo(CurrentEditor->SelectionEnd);
	}
}
static void OnTextEditorCut(Window* WithWindow)
{
	if (CurrentEditor && WithWindow->Input.IsKeyDown(Key::CTRL))
	{
		WithWindow->Input.SetClipboard(CurrentEditor->GetSelectedText());
		CurrentEditor->DeleteSelection();
	}
}

static void OnTextEditorCopy(Window* WithWindow)
{
	if (CurrentEditor && WithWindow->Input.IsKeyDown(Key::CTRL))
	{
		WithWindow->Input.SetClipboard(CurrentEditor->GetSelectedText());
	}
}
kui::UITextEditor::UITextEditor(ITextEditorProvider* EditorProvider, Font* EditorFont)
	: UIBackground(false, 0, 0)
{
	EditorScrollBox = new UIScrollBox(false, 0, true);
	EditorScrollBox->SetMinSize(UISize::Parent(1));
	EditorScrollBox->SetMaxSize(UISize::Parent(1));
	EditorScrollBox->OnScroll = [this](UIScrollBox*)
		{
			this->UpdateContent();
			for (auto& i : Highlighted)
			{
				i.GenerateSegments(this);
			}
		};

	SelectionColor = ParentWindow->Colors.TextFieldSelection;
	CursorColor = ParentWindow->Colors.TextFieldTextDefaultColor;

	EditorScrollBox->SetScrollSpeed(24);

	AddChild(EditorScrollBox);

	SelectorBeam = new UIBackground(true, 0, CursorColor);

	this->HasMouseCollision = true;
	this->EditorFont = EditorFont;
	this->EditorProvider = EditorProvider;
	this->EditorProvider->ParentEditor = this;

	ParentWindow->Input.RegisterOnKeyDownCallback(Key::BACKSPACE, &OnTextEditorBackspace);
	ParentWindow->Input.RegisterOnKeyDownCallback(Key::DELETE, &OnTextEditorDelete);
	ParentWindow->Input.RegisterOnKeyDownCallback(Key::UP, &OnTextEditorUp);
	ParentWindow->Input.RegisterOnKeyDownCallback(Key::DOWN, &OnTextEditorDown);
	ParentWindow->Input.RegisterOnKeyDownCallback(Key::LEFT, &OnTextEditorLeft);
	ParentWindow->Input.RegisterOnKeyDownCallback(Key::RIGHT, &OnTextEditorRight);
	ParentWindow->Input.RegisterOnKeyDownCallback(Key::x, &OnTextEditorCut);
	ParentWindow->Input.RegisterOnKeyDownCallback(Key::c, &OnTextEditorCopy);

	auto& SelectionArea = this->Highlighted.emplace_back();
	SelectionArea.Priority = 0;
	SelectionArea.Color = SelectionColor;
	EditorProvider->GetHighlightsForRange(0, EditorProvider->GetLineCount());
	CharSize = UIText::GetTextSizeAtScale(12_px, EditorFont);
	this->EditorProvider->OnLoaded();
}

kui::UITextEditor::~UITextEditor()
{
	delete this->SelectorBeam;
	if (CurrentEditor == this)
	{
		CurrentEditor = nullptr;
	}

	if (this->IsEdited)
	{
		this->IsEdited = false;
		GetParentWindow()->Input.PollForText = false;
	}
}

void kui::UITextEditor::NewLine()
{
	DeleteSelection();
	InsertNewLine(SelectionEnd, true);

	SelectionEnd.Line++;
	SelectionEnd.Column = 0;
	SelectionStart = SelectionEnd;
}

void kui::UITextEditor::RemoveLine(size_t Index)
{
	this->Lines.erase(this->Lines.begin() + Index);
	EditorProvider->RemoveLines(Index, 1);
}

void kui::UITextEditor::AddLine(size_t Index, const std::vector<TextSegment>& NewContent)
{
	this->Lines.insert(this->Lines.begin() + Index, LineEntry{
			.Data = NewContent
		});
	EditorProvider->InsertLine(Index, NewContent);
}

void kui::UITextEditor::EraseLine()
{
	auto& Line = GetLine(SelectionEnd.Line);

	std::vector<TextSegment> EndOfLine;
	Get(SelectionEnd, Line.Length, EndOfLine);

	RemoveLine(this->SelectionEnd.Line);

	this->SelectionEnd.Line--;

	auto& NextLine = Lines[this->SelectionEnd.Line];
	SelectionEnd.Column = NextLine.Length;

	for (auto& i : EndOfLine)
		NextLine.Data.push_back(i);
	EditorProvider->SetLine(this->SelectionEnd.Line, NextLine.Data);
	ClearSelection();
	EditorProvider->Commit();
}

void kui::UITextEditor::MoveCursor(int64_t Column, int64_t Line, bool DragSelection, bool SnapToWord)
{
	if (Column != 0)
	{
		CurrentEditor->SnapColumn(SelectionEnd);
		if (Column > 0 || int64_t(SelectionEnd.Column) >= abs(Column))
		{
			SelectionEnd.Column += Column;
			if (IsLineLoaded(SelectionEnd.Line) && GetLine(SelectionEnd.Line).Length < SelectionEnd.Column)
			{
				SelectionEnd.Line++;
				SelectionEnd.Column = 0;
			}
			if (SnapToWord)
			{
				this->AdjustSelection(SelectionEnd, Column > 0);
			}
		}
		else if (Column < 0 && SelectionEnd.Column == 0 && IsLineLoaded(SelectionEnd.Line) && SelectionEnd.Line > 0)
		{
			SelectionEnd.Line--;
			SelectionEnd.Column = GetLine(SelectionEnd.Line).Length;
		}
	}
	if (Line != 0 && (Line > 0 || int64_t(SelectionEnd.Line) >= abs(Line)))
	{
		SelectionEnd = CharacterPosToGrid(SelectionEnd, false);
		SelectionEnd.Line += Line;
		SelectionEnd = GridToCharacterPos(SelectionEnd, false);
	}


	if (!DragSelection)
	{
		SelectionStart = SelectionEnd;
	}
	else if (SnapToWord)
	{
		if (SelectionStart.Line == SelectionEnd.Line)
		{
			if (SelectionEnd.Column > SelectionStart.Column)
			{
				AdjustSelection(SelectionStart, false);
			}
			else
			{
				AdjustSelection(SelectionStart, true);
			}
		}
		else if (SelectionStart.Line > SelectionStart.Line)
		{
			AdjustSelection(SelectionStart, false);
		}
		else
		{
			AdjustSelection(SelectionStart, true);
		}
	}

	UpdateSelectionHighlights();
	CursorTimer.Reset();
}

void kui::UITextEditor::UpdateSelectionHighlights()
{
	if (this->Highlighted[0].Start == this->Highlighted[0].End
		&& SelectionStart == SelectionEnd)
	{
		return;
	}
	this->Highlighted[0].Start = SelectionStart;
	this->Highlighted[0].End = SelectionEnd;
	this->Highlighted[0].GenerateSegments(this);
}

void kui::UITextEditor::SetCursorPosition(EditorPosition Position)
{
	this->SelectionStart = Position;
	this->SelectionEnd = Position;
	UpdateSelectionHighlights();
	CursorTimer.Reset();
}

void kui::UITextEditor::SetCursorPosition(EditorPosition Start, EditorPosition End)
{
	this->SelectionStart = Start;
	this->SelectionEnd = End;
	UpdateSelectionHighlights();
	CursorTimer.Reset();
}

EditorPosition kui::UITextEditor::GetCursorPosition() const
{
	return this->SelectionEnd;
}

void kui::UITextEditor::ScrollTo(EditorPosition Position)
{
	float CursorPos = Position.Line * this->CharSize.Y;
	float Scrolled = this->EditorScrollBox->GetScrollObject()->Scrolled;
	float Difference = CursorPos - Scrolled;

	if (Difference < CharSize.Y * 2)
	{
		this->EditorScrollBox->GetScrollObject()->Scrolled = std::max(CursorPos - CharSize.Y * 2, 0.0f);
	}
	else if (Difference > EditorScrollBox->GetUsedSize().GetScreen().Y - CharSize.Y * 2)
	{
		this->EditorScrollBox->GetScrollObject()->Scrolled = CursorPos - EditorScrollBox->GetUsedSize().GetScreen().Y + CharSize.Y * 2;
	}
}

std::string kui::UITextEditor::GetSelectedText()
{
	std::vector<TextSegment> Segments;
	std::string Selection;

	auto Start = SelectionStart;
	auto End = SelectionEnd;

	if (Start.Line > End.Line)
	{
		std::swap(Start, End);
	}
	else if (Start.Line == End.Line && Start.Column > End.Column)
	{
		std::swap(Start, End);
	}

	for (size_t i = Start.Line; i <= End.Line; i++)
	{
		EditorPosition GetStart;
		GetStart.Line = i;
		size_t Count = 0;
		if (i == Start.Line)
		{
			GetStart.Column = Start.Column;
			if (Start.Line == End.Line)
			{
				Count = End.Column - Start.Column;
			}
			else
			{
				Count = SIZE_MAX;
			}
		}
		else
		{
			GetStart.Column = 0;
			if (i == End.Line)
			{
				Count = End.Column;
			}
			else
			{
				Count = SIZE_MAX;
			}
		}
		Segments.clear();
		Get(GetStart, Count, Segments, true);
		Selection += TextSegment::CombineToString(Segments);
		if (i != End.Line)
		{
			Selection += "\n";
		}
	}

	return Selection;
}

void kui::UITextEditor::ColorizeLine(size_t Line, const std::vector<EditorColorizeSegment>& Segments)
{
	if (!IsLineLoaded(Line))
		return;

	std::vector<TextSegment> NewLine;

	auto& LineData = GetLine(Line);
	NewLine.reserve(LineData.Data.size() + Segments.size());

	size_t Length = TextSegment::Length(LineData.Data);

	size_t Position = 0;

	for (auto& i : Segments)
	{
		Get(EditorPosition{
			.Column = Position,
			.Line = Line,
			}, i.Offset, NewLine);

		std::vector<TextSegment> ColorizedText;
		Get(EditorPosition{
			.Column = Position + i.Offset,
			.Line = Line,
			}, i.Length, ColorizedText);
		NewLine.push_back(TextSegment(TextSegment::CombineToString(ColorizedText), i.Color));

		Position += i.Offset + i.Length;

		if (Position > Length)
		{
			break;
		}
	}

	Get(EditorPosition{
		.Column = Position,
		.Line = Line,
		}, SIZE_MAX, NewLine);

	LineData.Data = NewLine;
}

void kui::UITextEditor::DeleteSelection()
{
	if (SelectionStart.Column == SelectionEnd.Column && SelectionStart.Line == SelectionEnd.Line)
		return;
	Erase(SelectionStart, SelectionEnd);
}

void kui::UITextEditor::DeleteChar()
{
	CurrentEditor->CursorTimer.Reset();
	if (SelectionStart.Column != SelectionEnd.Column || SelectionStart.Line != SelectionEnd.Line)
	{
		DeleteSelection();
		UpdateContent();
		return;
	}
	else
	{
		SnapColumn(SelectionEnd);
	}
	auto StartPos = SelectionStart;

	if (StartPos.Column == 0)
	{
		if (StartPos.Line == 0)
			return;
		EraseLine();
		UpdateContent();
		return;
	}

	StartPos.Column -= 1;
	Erase(StartPos, SelectionStart);
	SelectionStart = StartPos;
	UpdateContent();
}

void kui::UITextEditor::UpdateContent()
{
	EditorScrollBox->DeleteChildren();

	CharSize = UIText::GetTextSizeAtScale(12_px, EditorFont);
	SelectorBeam->SetMinSize(SizeVec(1_px, CharSize.Y));
	SelectorBeam->SetColor(this->CursorColor);

	size_t LineCount = EditorProvider->GetLineCount();

	size_t CharPosition = size_t(EditorScrollBox->GetScrollObject()->Scrolled / CharSize.Y);

	UIText* NewChunk = BuildChunk(CharPosition, std::min(EditorLineSize + CharPosition, LineCount) - CharPosition);

	size_t RemainingLines = CharPosition + EditorLineSize;

	NewChunk->SetPadding(
		CharPosition * CharSize.Y,
		(LineCount > RemainingLines ? LineCount - RemainingLines : 0) * CharSize.Y,
		0, 0);
	NewChunk->SetTextWidthOverride(GetUsedSize().X);
	NewChunk->Update();
	EditorScrollBox->AddChild(NewChunk);
	RefreshText = false;
}

UITextEditor::LineEntry& kui::UITextEditor::GetLine(size_t Index)
{
	return Lines[Index - LinesStart];
}

void kui::UITextEditor::Draw(render::RenderBackend* With)
{
	auto GLBackend = dynamic_cast<render::OpenGLBackend*>(With);

	if (!GLBackend)
	{
		return;
	}

	if (Highlighted.empty())
	{
		return;
	}

	BackgroundShader = static_cast<render::GLUIBackgroundState*>(State)->UsedShader;

	BackgroundShader->Bind();
	GLBackend->UpdateScroll(this->CurrentScrollObject, BackgroundShader, this->State);

	BackgroundShader->SetFloat("u_opacity", 1);
	BackgroundShader->SetInt("u_drawBorder", 0);
	BackgroundShader->SetInt("u_drawCorner", 0);
	BackgroundShader->SetFloat("u_aspectRatio", ParentWindow->GetAspectRatio());
	BackgroundShader->SetVec2("u_screenRes", Vec2f(
		float(ParentWindow->GetSize().X),
		float(ParentWindow->GetSize().Y)));

	BackgroundShader->SetInt("u_useTexture", 0);
	BackgroundShader->SetVec3("u_offset",
		Vec3f(-EditorScrollBox->GetScrollObject()->GetOffset(),
			EditorScrollBox->GetScrollObject()->GetPosition().Y,
			EditorScrollBox->GetScrollObject()->GetScale().Y));

	if (HighlightsChanged)
	{
		std::sort(this->Highlighted.begin(), this->Highlighted.end(), [](HighlightedArea a, const HighlightedArea& b)
			{
				return a.Priority < b.Priority;
			});
	}
	HighlightsChanged = false;

	for (auto& i : Highlighted)
	{
		BackgroundShader->SetVec3("u_color", i.Color);

		for (auto& Segment : i.Segments)
		{
			glUniform4f(BackgroundShader->GetUniformLocation("u_transform"),
				Segment.Position.X, Segment.Position.Y,
				Segment.Size.X, Segment.Size.Y);
			GLBackend->BoxVertexBuffer->Draw();
		}
	}
}

UIText* kui::UITextEditor::BuildChunk(size_t Position, size_t Length)
{
	Position -= LinesStart;

	if (Position > EditorProvider->GetLineCount())
	{
		return new UIText(12_px, 0, "", nullptr);
	}

	if (Lines.size() < Position + Length)
	{
		if (Lines.empty())
		{
			Lines.resize(Position + Length);
			for (size_t i = 0; i < Position + Length; i++)
			{
				this->EditorProvider->GetLine(i + LinesStart, Lines[i].Data);
			}
		}
		else
		{
			auto OldSize = Lines.size();
			Lines.resize(Position + Length);
			size_t it = OldSize;
			for (auto i = Lines.begin() + OldSize; i < Lines.end(); i++)
			{
				this->EditorProvider->GetLine(it + LinesStart, i->Data);
				it++;
			}
		}
	}
	std::lock_guard g{ LinesMutex };

	size_t ChunkWidth = size_t(UISize::Pixels(this->GetUsedSize().GetPixels().X
		- EditorScrollBox->ScrollBarWidth).GetScreen().X / CharSize.X);

	ChunkWidth -= EditorProvider->GetPreLineSize();

	std::vector<TextSegment> Segments;
	for (size_t i = 0; i < Length; i++)
	{
		this->EditorProvider->GetPreLine(Position + i + LinesStart, Segments);

		LineEntry& LineSegments = Lines[Position + i];

		Segments.reserve(Segments.size() + LineSegments.Data.size());

		LineSegments.Length = 0;
		size_t CharacterLength = 0;
		TextSegment NextSegment = TextSegment("", 1);
		for (auto& i : LineSegments.Data)
		{
			LineSegments.Length += i.Text.size();

			if (i.Color != NextSegment.Color)
			{
				if (NextSegment.Text.size())
				{
					Segments.push_back(NextSegment);
				}
				NextSegment.Text.clear();
				NextSegment.Color = i.Color;
			}

			NextSegment.Text.reserve(i.Text.size());
			for (char c : i.Text)
			{
				CharacterLength += c == '\t' ? 4 : 1;
				if (CharacterLength > ChunkWidth)
					break;
				NextSegment.Text.push_back(c);
			}
			if (CharacterLength > ChunkWidth)
				break;
		}

		if (NextSegment.Text.size())
		{
			Segments.push_back(NextSegment);
		}

		Segments.push_back(TextSegment("\n", 0));
	}

	auto NewText = new UIText(12_px, Segments, this->EditorFont);
	return NewText;
}

void kui::UITextEditor::Update()
{
	CharSize = UIText::GetTextSizeAtScale(12_px, EditorFont);
	EditorLineSize = size_t(this->GetUsedSize().GetScreen().Y / CharSize.Y) * 2;
	UpdateContent();
	EditorScrollBox->Update();
	for (auto& i : Highlighted)
	{
		i.GenerateSegments(this);
	}
}

size_t kui::UITextEditor::GetLoadedLines()
{
	return Lines.size();
}

void kui::UITextEditor::FullRefresh()
{
	this->UpdateHighlights = true;
	this->RefreshText = true;
	this->Lines.clear();
}

void kui::UITextEditor::RefreshHighlights()
{
	this->UpdateHighlights = true;
}

bool kui::UITextEditor::IsLineLoaded(size_t Index)
{
	if (Index < LinesStart)
		return false;

	return Index - LinesStart < Lines.size();
}

void kui::UITextEditor::AdjustSelection(EditorPosition& Position, bool DirectionForward)
{
	if (!IsLineLoaded(Position.Line))
	{
		return;
	}

	auto& Line = GetLine(Position.Line);

	auto String = TextSegment::CombineToString(Line.Data);

	if (Position.Column > String.size())
	{
		return;
	}

	if (DirectionForward)
	{
		if (SelectionMode == SelectMode::Line)
		{
			if (Position.Column != 0)
			{
				if (Position.Line + 1 < Lines.size())
				{
					Position.Line++;
					Position.Column = 0;
				}
				else
				{
					Position.Column = Line.Length;
				}
			}
			return;
		}
		for (int64_t i = Position.Column; i <= int64_t(String.size()); i++)
		{
			char Character = String[Position.Column];

			if (!std::isalpha(Character) && !std::isdigit(Character) && Character != '_')
			{
				break;
			}
			Position.Column = i;
		}
	}
	else if (Position.Column > 0)
	{
		if (SelectionMode == SelectMode::Line)
		{
			Position.Column = 0;
			return;
		}
		for (int64_t i = Position.Column; i >= 0; i--)
		{
			char Character = String[Position.Column - 1];

			if (!std::isalpha(Character) && !std::isdigit(Character) && Character != '_')
			{
				break;
			}
			Position.Column = i;
		}
	}
}
void kui::UITextEditor::Edit()
{
	auto& Input = ParentWindow->Input;
	IsEdited = true;
	Input.Text.clear();
	Input.CanEditText = true;
	Input.PollForText = true;
	Input.TextAllowNewLine = true;
	CurrentEditor = this;
	CursorTimer.Reset();
}

void kui::UITextEditor::Tick()
{
	TickInput();

	if (UpdateHighlights)
	{
		this->Highlighted.clear();

		auto& SelectionArea = this->Highlighted.emplace_back();
		SelectionArea.Priority = 0;
		SelectionArea.Color = SelectionColor;
		UpdateSelectionHighlights();

		EditorProvider->GetHighlightsForRange(0, EditorProvider->GetLineCount());
		UpdateHighlights = false;

		for (auto& i : Highlighted)
		{
			i.GenerateSegments(this);
		}
		if (!Highlighted.empty())
		{
			RefreshText = true;
		}
	}

	if (RefreshText)
	{
		UpdateContent();
	}
	SelectorBeam->IsVisible = this->IsVisible && this->IsEdited && std::fmod(CursorTimer.Get(), 1.0f) < 0.5f
		&& this->ParentWindow->HasFocus();
	auto& Hovered = ParentWindow->UI.HoveredBox;
	auto& Input = ParentWindow->Input;

	if (Hovered && Hovered->IsChildOf(this) && !UIScrollBox::IsDraggingScrollBox)
	{
		ParentWindow->CurrentCursor = Window::Cursor::Text;
	}
	else
	{
		if (Input.IsLMBClicked && this->IsEdited)
		{
			this->IsEdited = false;
			CurrentEditor = nullptr;
			Input.PollForText = false;
		}
		return;
	}

	if (Input.IsLMBDown && !UIScrollBox::IsDraggingScrollBox)
	{
		auto NewPosition = ScreenToEditor(Input.MousePosition);

		if (NewPosition == CursorDragStart && Input.IsLMBClicked
			&& DoubleClickTimer.Get() < 0.5f)
		{
			SelectionMode = SelectMode((int(SelectionMode) + 1) % 4);
		}
		else if (Input.IsLMBClicked)
		{
			SelectionMode = SelectMode::Character;
		}

		DoubleClickTimer.Reset();

		SelectionEnd = NewPosition;
		CursorDragEnd = SelectionEnd;
		if (!DraggingSelection)
		{
			SelectionStart = SelectionEnd;
			CursorDragStart = SelectionStart;
			DraggingSelection = true;
		}
		else
		{
			ScrollTo(SelectionEnd);
			if (SelectionMode != SelectMode::Character)
				SnapHighlightToWord();
			UpdateSelectionHighlights();
		}

		Edit();
	}
	else
	{
		DraggingSelection = false;
	}

	this->EditorProvider->Update();
}

void kui::UITextEditor::TickInput()
{
	if (!IsEdited)
	{
		return;
	}

	auto& Input = ParentWindow->Input;
	if (!Input.PollForText)
	{
		IsEdited = false;
		return;
	}


	UpdateSelectionBeam();

	auto& NewText = Input.Text;
	if (NewText.size())
	{
		std::string Transformed = EditorProvider->ProcessInput(NewText);
		if (Transformed.size())
		{
			DeleteSelection();

			InsertAtCursor(Transformed, Transformed.size() > 4);
			UpdateContent();
		}

		NewText.clear();
	}
}

EditorPosition kui::UITextEditor::InsertAtCursor(std::string NewString, bool Raw)
{
	if (SelectionEnd == SelectionStart)
	{
		SnapColumn(SelectionEnd);
		SelectionStart = SelectionEnd;
	}
	SelectionEnd = Insert(NewString, SelectionStart, Raw);
	SelectionStart = SelectionEnd;
	CurrentEditor->ScrollTo(CurrentEditor->SelectionEnd);
	this->CursorTimer.Reset();
	return SelectionEnd;
}

EditorPosition kui::UITextEditor::Insert(std::string NewString, EditorPosition At, bool Raw, bool Commit)
{
	if (NewString.empty())
	{
		return At;
	}

	auto& LineData = GetLine(At.Line).Data;

	size_t NewLine = NewString.find_first_of('\n');

	if (NewLine != std::string::npos)
	{
		std::string OldIndent;
		if (!Raw)
		{
			OldIndent = TextSegment::CombineToString(LineData);
			OldIndent = OldIndent.substr(0, OldIndent.find_first_not_of("\t "));
		}
		auto NewLinePos = Insert(NewString.substr(0, NewLine), At, Raw, false);

		InsertNewLine(NewLinePos, false);
		NewLinePos.Line++;
		NewLinePos.Column = 0;
		return Insert(OldIndent + NewString.substr(NewLine + 1), NewLinePos, Raw, Commit);
	}

	if (LineData.empty())
	{
		LineData.push_back(TextSegment(NewString, 1));
		GetLine(At.Line).Length = NewString.size();
		EditorProvider->SetLine(At.Line, LineData);
		if (Commit)
		{
			EditorProvider->Commit();
		}
		return EditorPosition(NewString.size(), At.Line);
	}

	size_t ReturnColumn = At.Column + NewString.size();

	for (auto& i : LineData)
	{
		if (At.Column <= i.Text.size())
		{
			i.Text.insert(i.Text.begin() + At.Column, NewString.begin(), NewString.end());
			break;
		}
		else
		{
			At.Column -= i.Text.size();
		}
	}
	EditorProvider->SetLine(At.Line, LineData);
	GetLine(At.Line).Length += NewString.size();
	if (Commit)
	{
		EditorProvider->Commit();
	}
	return EditorPosition(ReturnColumn, At.Line);
}

void kui::UITextEditor::Erase(EditorPosition Begin, EditorPosition End, bool DoCommit)
{
	if (Begin.Line == End.Line)
	{
		if (Begin.Column == End.Column)
		{
			return;
		}

		if (Begin.Column > End.Column)
		{
			std::swap(Begin.Column, End.Column);
		}

		size_t OldStart = Begin.Column;

		size_t Length = End.Column - Begin.Column;
		auto& Line = GetLine(Begin.Line).Data;
		for (auto i = Line.begin(); i < Line.end(); i++)
		{
			if (Begin.Column <= i->Text.size())
			{
				size_t Erased = std::min(i->Text.size() - Begin.Column, Length);

				auto At = i->Text.begin() + Begin.Column;

				i->Text.erase(At, At + Erased);
				Length -= Erased;

				if (SelectionStart.Line == Begin.Line
					&& SelectionStart.Column > OldStart)
				{
					SelectionStart.Column -= Erased;
				}

				if (SelectionEnd.Line == Begin.Line
					&& SelectionEnd.Column > OldStart)
				{
					SelectionEnd.Column -= Erased;
				}

				if (Length == 0)
				{
					break;
				}

				Begin.Column = 0;
			}
			else
			{
				Begin.Column -= i->Text.size();
			}
		}
		EditorProvider->SetLine(Begin.Line, Line);
		if (DoCommit)
		{
			EditorProvider->Commit();
		}
		return;
	}

	if (Begin.Line > End.Line)
	{
		std::swap(Begin, End);
	}

	for (size_t it = Begin.Line; it <= End.Line; it++)
	{
		if (it >= Lines.size())
		{
			continue;
		}

		auto& Line = this->GetLine(it);

		if (it == Begin.Line)
		{
			auto EndPos = Begin;
			EndPos.Column = Line.Length;
			Erase(Begin, EndPos, false);
			EditorProvider->SetLine(it, Line.Data);
		}
		else if (it == End.Line)
		{
			std::vector<TextSegment> Segments;
			Get(End, Line.Length, Segments);

			auto& LastLine = GetLine(it - 1).Data;

			for (auto& i : Segments)
			{
				LastLine.push_back(i);
			}
			EditorProvider->SetLine(it - 1, LastLine);
			EditorProvider->RemoveLines(it, 1);
			Lines.erase(Lines.begin() + it);
			if (SelectionStart.Line >= End.Line)
			{
				SelectionStart.Line--;
			}
			if (SelectionEnd.Line >= End.Line)
			{
				SelectionEnd.Line--;
			}
		}
		else
		{
			Lines.erase(Lines.begin() + it);
			if (SelectionStart.Line >= End.Line)
			{
				SelectionStart.Line--;
			}
			if (SelectionEnd.Line >= End.Line)
			{
				SelectionEnd.Line--;
			}
			it--;
			End.Line--;
			EditorProvider->RemoveLines(it, 1);
		}
	}
	if (DoCommit)
	{
		EditorProvider->Commit();
	}
	this->ClearSelection();
}

void kui::UITextEditor::ClearSelection()
{
	SelectionStart = SelectionEnd;
	UpdateSelectionHighlights();
}

void kui::UITextEditor::HighlightArea(const HighlightedArea& Area)
{
	this->Highlighted.push_back(Area);
	HighlightsChanged = true;
}

void kui::UITextEditor::SnapHighlightToWord()
{
	SelectionStart = CursorDragStart;
	SelectionEnd = CursorDragEnd;

	if (SelectionStart.Line == SelectionEnd.Line)
	{
		if (SelectionStart.Column > SelectionEnd.Column)
		{
			AdjustSelection(SelectionStart, true);
			AdjustSelection(SelectionEnd, false);
		}
		else
		{
			AdjustSelection(SelectionStart, false);
			AdjustSelection(SelectionEnd, true);
		}
	}
	else if (SelectionStart.Line > SelectionEnd.Line)
	{
		AdjustSelection(SelectionStart, true);
		AdjustSelection(SelectionEnd, false);
	}
	else
	{
		AdjustSelection(SelectionStart, false);
		AdjustSelection(SelectionEnd, true);
	}
}

void kui::UITextEditor::SetLine(size_t Index, const std::vector<TextSegment>& NewContent)
{
	if (!IsLineLoaded(Index))
	{
		return;
	}

	GetLine(Index).Data = NewContent;
	this->RefreshText = true;
}

void kui::UITextEditor::InsertNewLine(EditorPosition At, bool Commit)
{
	auto& Line = GetLine(At.Line);

	auto EraseEnd = At;
	EraseEnd.Column = Line.Length;

	std::vector<TextSegment> EndOfLine;
	size_t EndOfLineCount = Line.Length - At.Column;
	Get(At, EndOfLineCount, EndOfLine);

	Erase(At, EraseEnd, false);
	EditorProvider->SetLine(At.Line, Line.Data);

	this->Lines.insert(this->Lines.begin() + At.Line + 1, LineEntry{
		EndOfLine,
		})->Length = EndOfLineCount;
	EditorProvider->InsertLine(At.Line + 1, EndOfLine);

	if (Commit)
	{
		EditorProvider->Commit();
	}
}

void kui::UITextEditor::SnapColumn(EditorPosition& Position)
{
	if (IsLineLoaded(Position.Line))
	{
		Position.Column = std::min(GetLine(Position.Line).Length, Position.Column);
	}
}
void kui::UITextEditor::UpdateSelectionBeam()
{
	SelectorBeam->SetCurrentScrollObject(this->EditorScrollBox);
	Vec2f Pos = EditorToScreen(SelectionEnd);
	SelectorBeam->SetPosition(Pos);
}

void kui::UITextEditor::ClearEmptyLineEntries(size_t Index)
{
	auto& Line = GetLine(Index).Data;

	for (size_t i = 0; i < Line.size(); i++)
	{
		if (Line[i].Text.empty())
		{
			Line.erase(Line.begin() + i);
			i--;
		}
	}
}

void kui::UITextEditor::Get(EditorPosition Begin, size_t Length, std::vector<TextSegment>& To,
	bool IncludeUnloaded, bool Locking)
{
	std::vector<TextSegment>* Line = nullptr;

	std::vector<TextSegment> LineData;
	if (!IsLineLoaded(Begin.Line))
	{
		if (IncludeUnloaded && Begin.Line < EditorProvider->GetLineCount())
		{
			EditorProvider->GetLine(Begin.Line, LineData);
		}
		Line = &LineData;
	}
	else
	{
		Line = &GetLine(Begin.Line).Data;
	}

	if (Locking)
	{
		LinesMutex.lock();
	}

	for (size_t it = 0; it < Line->size(); it++)
	{
		auto* i = &Line->at(it);
		if (Begin.Column <= i->Text.size())
		{
			size_t Copied = std::min(i->Text.size() - Begin.Column, Length);

			if (Copied)
			{
				To.push_back(TextSegment(i->Text.substr(Begin.Column, Copied), i->Color));

				Length -= Copied;

				if (Length == 0)
				{
					break;
				}
			}

			Begin.Column = 0;
		}
		else
		{
			Begin.Column -= i->Text.size();
		}
	}
	if (Locking)
	{
		LinesMutex.unlock();
	}
}

EditorPosition kui::UITextEditor::ScreenToEditor(Vec2f Position, bool SnapToEnd)
{
	Position.X += CharSize.X / 2;

	Vec2f Pos = Position - GetPosition() - Vec2f(0, EditorScrollBox->GetScrollObject()->GetOffset());

	Pos.Y = this->Size.Y - Pos.Y;
	Pos = Pos / CharSize;

	float LineSize = float(EditorProvider->GetPreLineSize());
	Pos.X -= LineSize;

	if (Pos.X < 0)
	{
		Pos.X = 0;
	}

	size_t PosX = size_t(Pos.X);
	size_t PosY = size_t(Pos.Y);

	return GridToCharacterPos(EditorPosition(PosX, PosY), SnapToEnd);
}

Vec2f kui::UITextEditor::EditorToScreen(EditorPosition Position)
{
	Position = CharacterPosToGrid(Position);
	Vec2f Pos = Vec2f(
		float(Position.Column + EditorProvider->GetPreLineSize()),
		-float(Position.Line)) * CharSize;

	return GetPosition() + Pos + Vec2f(0, Size.Y - CharSize.Y);
}

EditorPosition kui::UITextEditor::CharacterPosToGrid(EditorPosition CharacterPos, bool SnapToEnd)
{
	if (IsLineLoaded(CharacterPos.Line))
	{
		auto& Line = GetLine(CharacterPos.Line);

		size_t PosX = CharacterPos.Column;
		size_t Count = 0;
		for (auto& Segment : Line.Data)
		{
			if (PosX == 0)
			{
				break;
			}

			for (auto it = Segment.Text.begin(); it < Segment.Text.end(); it++)
			{
				char c = *it;
				if (PosX == 0)
				{
					break;
				}

				if (uint8_t(c) > 0x7f)
				{
					Count++;
					PosX--;
					if (PosX == 0)
					{
						break;
					}
					if (it < Segment.Text.end() - 1)
					{
						it++;
					}
				}
				else if (c == '\t')
				{
					Count += 4 - Count % 4;
				}
				else
				{
					Count++;
				}
				PosX--;
			}
		}
		PosX = SnapToEnd ? Count : PosX + Count;
		CharacterPos.Column = PosX;
	}
	return CharacterPos;
}

EditorPosition kui::UITextEditor::GridToCharacterPos(EditorPosition GridPos, bool SnapToEnd)
{
	size_t PosX = size_t(GridPos.Column);
	size_t PosY = size_t(GridPos.Line);

	if (PosY >= Lines.size() && Lines.size() != 0)
	{
		PosY = Lines.size() - 1;
	}

	if (IsLineLoaded(PosY))
	{
		auto& Line = GetLine(PosY);
		size_t Count = 0;
		size_t CharCount = 0;
		for (auto& i : Line.Data)
		{
			if (PosX == 0)
			{
				break;
			}

			for (char c : i.Text)
			{
				if (PosX == 0)
				{
					break;
				}

				if (c == '\t')
				{
					size_t TabSize = 4 - CharCount % 4;
					if (PosX < TabSize / 2)
					{
						PosX = 0;
						break;
					}
					else if (PosX < TabSize)
					{
						PosX = 0;
						Count++;
						break;
					}
					CharCount += TabSize - 1;
					PosX -= TabSize - 1;
				}
				if (uint8_t(c) > 127)
				{
					Count += 1;
				}
				Count++;
				CharCount++;
				PosX--;
			}
		}
		PosX = SnapToEnd ? Count : PosX + Count;
	}

	return EditorPosition(PosX, PosY);
}
#endif
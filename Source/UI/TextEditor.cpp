#ifdef KUI_EXPERIMENTAL_FILE_EDIT
#include <kui/UI/UITextEditor.h>

void kui::ITextEditorProvider::UpdateLine(size_t Index, const std::vector<TextSegment>& NewContent)
{
	this->ParentEditor->SetLine(Index, NewContent);
}
#endif
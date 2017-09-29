#include "zoomsrc.h"

/*
SmallView::SmallView() : wxTextFile(_T("")) {
	Clear();
	AddLine(_T(""));

}
*/
int SmallView::GetZoomTop(){
	return fontHeight * rangeStart + titleOffset + top ;
}
int SmallView::GetZoomHeight(){
	return fontHeight * rangeLength;
}

int SmallView::Left(){
	return left;
}

SmallView::SmallView(wxPanel *e, const wxString& fn) 
	: wxTextFile(fn) {

	filename = fn;
	ep = e;
	EditPanel *edp = (EditPanel *) ep;

	titleOffset = 10;
	curCol = 0; curLine = 0;
	rangeStart = 0;
	lastRangeStart = rangeStart;
	rangeLength = 43;
	rangeEnd = rangeLength + rangeStart;
	tabstop = 4;
	left = 0; top = 0;

	Resize(3);

	Open();
	Alloc();
	
	CalcNumLines();
	CalcCurLineLen();
	
	DrawSmallText();
}

void SmallView::Alloc(){
	if( IsOpened() ){
		cout("Loaded %d lines", GetLineCount() );

		width = fontWidth * 80;
		height = fontHeight * GetLineCount() + titleOffset;
		cout2("Allocating %dx%d small raster", width, height );	

		smallBmp.Create(80 * fontWidth, 
				GetLineCount() * fontHeight + titleOffset, 32);

	} else {
		width = fontWidth * 80;
		height = fontHeight + titleOffset;
		smallBmp.Create(width, height, 32);
		Create();
		AddLine(_T(""));
	}
}

void SmallView::Resize(int fs){
	EditPanel *edp = (EditPanel *) ep;
	fontSize = constrain(fs, 1, 14);
	font = wxFont( fontSize, wxFONTFAMILY_TELETYPE,
			wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,
			false, _T("Lucida Console"));
	edp->GetCharSize(font, fontWidth, fontHeight);
}

void SmallView::DrawSmallText( ){
	
	//wxMemoryDC smallDC;
	//smallDC.SelectObject(smallBmp);
	wxBufferedDC smallDC( NULL, smallBmp );

	smallDC.SetFont( font );
	fontHeight = smallDC.GetCharHeight();
	fontWidth = smallDC.GetCharWidth();

	smallDC.SetBrush( wxColor( 244,244,244 ) );
	smallDC.SetTextForeground( wxColor( 244,244,244 ) );

	EditPanel *e = (EditPanel *)ep;

	unsigned int i=0; 
	while( i < GetLineCount() ){
		e->DrawLine( smallDC, GetLine(i) , 0, titleOffset + i * fontHeight );
		i++;
	}

	wxFont fnFont = wxFont( 8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
			wxFONTWEIGHT_BOLD, false, _T("Lucida Console") );
	smallDC.SetFont(fnFont);
	smallDC.SetBrush( wxColour( 0x00, 0x00, 0x5f ) );
	smallDC.SetPen(wxNullPen);
	smallDC.DrawRectangle( 0, 0, smallBmp.GetWidth(), titleOffset );
	smallDC.SetTextForeground( wxColour( 0xff, 0xff, 0x77 ) );
	smallDC.DrawText(GetName(), 1, 1 );
}

void SmallView::Indent(){
	wxString& line = GetLine(curLine);
	line.Prepend(_T("\t"));
	curCol++;
	UpdateChar();
}
void SmallView::Undent(){
	wxString& line = GetLine(curLine);
	int i = 0;
	if( line.GetChar(0) == '\t' ) i = 1;
	else while( i < tabstop && line.GetChar(i) == ' ' ) i++;
	line.Remove(0, i);
	curCol--;
	UpdateChar();
}

void SmallView::UpdateChar(){
	curChar = GetLine(curLine).GetChar(curCol);
}

void SmallView::CursorLeft(int n){
	curCol = constrain( curCol - n, 0, curLineLen);
	UpdateChar();
}
void SmallView::CursorRight(int n){
	curCol = constrain( curCol + n, 0, curLineLen);
	UpdateChar();
}
void SmallView::CursorDown(int n){
	curLine = constrain(curLine + n, 0, numLines - n);
	CalcCurLineLen();
	curCol = constrain( curCol, 0, curLineLen );
	UpdateChar();
}
void SmallView::CursorUp(int n){
	curLine = constrain(curLine - n, 0, numLines - n);
	CalcCurLineLen();
	curCol = constrain( curCol, 0, curLineLen );
	UpdateChar();
}

void SmallView::CalcCurLineLen(){
	curLineLen = GetLine(curLine).Len();
}
void SmallView::CalcNumLines(){
	numLines = GetLineCount();
}

void SmallView::InsertChar(wxChar c){
	if(c >= ' ') {
		wxString& line = GetLine(curLine);
		wxString l = line(0, curCol);
		wxString r = line(curCol, line.Len() - curCol);
		line = l + c + r;
		curCol++;
		CalcCurLineLen();
		UpdateChar();
	}
}
void SmallView::RemoveChar(){
	wxString& line = GetLine(curLine);
	if(curCol > 0){
		line.Remove(curCol - 1, 1);
		curCol--;
	}
	UpdateChar();
	CalcCurLineLen();
}
void SmallView::DeleteChar(){
	wxString& line = GetLine(curLine);
	line.Remove(curCol, 1);
	GetLine(curLine) = line;
	UpdateChar();
	CalcCurLineLen();
}

void SmallView::ScrollDown( int d ){
	lastRangeStart = rangeStart;
	rangeStart = constrain(rangeStart + d, 0, numLines - rangeLength);
}

void SmallView::ScrollUp( int d ){
	lastRangeStart = rangeStart;
	rangeStart = constrain(rangeStart - d, 0, numLines - rangeLength);
}

bool SmallView::Scroll(){
	int scrollMargin = 2;
	if(curLine > rangeStart + rangeLength - scrollMargin) {
		ScrollDown(1);
		return true;
	} else
	if(curLine < scrollMargin + rangeStart){
		ScrollUp(1);
		return true;
	}
	return false;
}
int SmallView::GetLeft(int col){
	int x = 0;
	for(int i= 0; i < col; i++){
		int cw = GetCharWidth( GetLine(curLine).GetChar(i) );
		x += cw;
	}
	return x;
}
int SmallView::GetCharWidth(wxChar ch){
	if(ch == '\t') return tabstop;
	else return 1;
}


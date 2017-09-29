#include "zoomsrc.h"
#include <wx/listimpl.cpp>
#include <wx/datetime.h>
WX_DEFINE_LIST( SmallViewList );

//WX_DEFINE_LIST(SmallViewList);

BEGIN_EVENT_TABLE(EditPanel, wxPanel)
	EVT_MOUSEWHEEL(EditPanel::OnMouseWheel)
	EVT_KEY_DOWN(EditPanel::OnKeyDown)
	EVT_KEY_UP(EditPanel::OnKeyUp)
	EVT_CHAR(EditPanel::OnChar)
	EVT_PAINT(EditPanel::OnPaint)
	EVT_SIZE(EditPanel::OnSize)
	EVT_ERASE_BACKGROUND(EditPanel::OnEraseBackground)
	EVT_MOTION(EditPanel::OnMouseMotion)
	EVT_LEFT_DOWN(EditPanel::OnMouseMotion)
	EVT_MENU(ZoomSrc_Indent, EditPanel::OnIndent)
	EVT_MENU(ZoomSrc_Undent, EditPanel::OnUndent)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(EditPanel, wxPanel)

#define SKIPMOD if(downNum <= skipNum || downNum % skipNum == 0)

void EditPanel::OnUndent(wxCommandEvent& event){
	src->Undent();
	RedrawLine();
	Refresh();
}
void EditPanel::OnIndent(wxCommandEvent& event){
	src->Indent();
	RedrawLine();
	Refresh();
}

void EditPanel::GetCharSize(wxFont &font, int &w, int &h){
	wxBitmap fakeBmp(32, 32, 32);
	wxBufferedDC dc(NULL, fakeBmp);
	dc.SetFont(font);
	w = dc.GetCharWidth();
	h = dc.GetCharHeight();
}

void EditPanel::Load(const wxString& fn){
	MainFrame *frm = (MainFrame *)m_parent;
//	int i = frm->menuGo->GetMenuItemCount() - 1;
//	for(; i >= 1; i--){
//		frm->menuGo->Remove(i);
//	}

	SmallView *sv1 = new SmallView( this, fn );

	if(views.GetCount() > 0) newRight -= sv1->width ;

	sv1->left = newRight; sv1->top = 0;

	views.Append(sv1);

	if(views.GetCount() == 1) {
		frm->menuGo->AppendSeparator();
		newRight = width;
	}

//	for(i=0; i < views.GetCount(); i++){
	frm->menuGo->Append( wxID_ANY, sv1->filename );
//	}
}

//typedef int (*wxSortCompareFunction)(const void *elem1, const void *elem2);

int sortlefttoright( const void *elem1, const void *elem2 ){
	//cout2("%d %d", (**(SmallView**)elem1).Left(), (**(SmallView**)elem2).Left() );
	return (
		(**(SmallView**)elem1).Left()  < 
		(**(SmallView**)elem2).Left()  )
		? -1
		: 1;
}
void EditPanel::SortViews(){
	MainFrame *frm = (MainFrame *)m_parent;
	views.Sort(sortlefttoright);
	SmallViewList::iterator view;


	int cnt = frm->menuGo->GetMenuItemCount() -1;
	cout("%d menus", cnt);
	for(int i = cnt; i > frm->menuGoOffset; i--){
		frm->menuGo->Delete( frm->menuGo->FindItemByPosition( i ) );
	}
	//frm->RebuildGo();
	int i = 1;
	for(view = views.begin(); view != views.end(); ++view){
		SmallView *sv1 = *view;
		cout2("%d: %d", i, sv1->left );
		wxString titlestr;
		titlestr.Printf(_T("%s\tCtrl-%d"), sv1->filename.c_str(), i );
		frm->menuGo->AppendRadioItem( ZoomSrc_GotoView, titlestr );
		//frm->menuGo->FindItemByPosition(frm->menuGoOffset + i)->SetItemLabel( titlestr );
		i++;
	}
	//frm->menuGo->UpdateUI();
}

EditPanel::EditPanel(wxWindow *parent, const wxString& filename) 
	: wxPanel(parent) {

	m_parent = parent;

	bfpShowAll = new wxBrush( wxColour( 0x22, 0x22, 0x22, 0x80 ) );
	frontPanelBrush = new wxBrush( wxColour( 0x00, 0x11, 0x33, 0x90 ) );
	cursorBrush = new wxBrush( wxColour( 0x00, 0xff, 0x00, 0x9f ) );
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	
	showModDown = false;
	newRight = 0;
	showall = false;
	midSize = 14;
	midFont = wxFont( midSize, wxFONTFAMILY_TELETYPE,
			wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,
			false, _T("Lucida Console"));

	
	GetCharSize(midFont, midWidth, midHeight);
	parent->GetClientSize(&width, &height);
	cout2("Client size is %d,%d", width, height);

	wTypes.Add(_T("void"));
	wTypes.Add(_T("int"));
	wTypes.Add(_T("boolean"));
	wTypes.Add(_T("float"));
	wTypes.Add(_T("double"));
	wTypes.Add(_T("String"));
	wTypes.Add(_T("long"));
	wTypes.Add(_T("class"));
	wKeyword.Add(_T("public"));
	wKeyword.Add(_T("private"));
	wKeyword.Add(_T("protected"));
	wKeyword.Add(_T("static"));
	cursormode = epcChar;
	//SmallView *sv2 = new SmallView(this, _T("mainframe.cpp") );
	//sv2->left = width - sv2->smallBmp.GetWidth(); sv2->top = 0;

	//views.Append(sv1);
	//views.Append(sv2);
	comment = false;
	tabstop = 4;

	midUpdate = true;
	downNum = 0;
	skipNum = 2;

	Load(_T("zoomsrc.h"));
	Load(_T("mainframe.cpp"));
	Load(_T("editpanel.cpp"));
	Load(_T("smallview.cpp"));
	Load(_T("zoomsrc.cpp"));
	SortViews();

	curView = views.begin();
	src = *curView;


	//m_bmp = wxBitmap( width, height, 32 );
}

void EditPanel::GotoView(int i){
	cout("GotoView(%d)", i);
	curView = views.begin();
	int k = constrain(i, 0, (signed int) (views.GetCount() - 1 ) );
	for(int j=0; j < k && curView != views.end() ; j++){
		++curView;
	}
	//curView += i; 

	//if( curView == views.end() ) curView = views.begin();

	src = *curView;
	midUpdate = true;
	src->lastRangeStart = src->rangeStart;
	Refresh();
}

void EditPanel::Swap(int direction){
	if(direction > 0) {
		++curView;
		if( curView == views.end() ) curView = views.begin();
	} else if(direction < 0){
		if( curView == views.begin() ) curView = views.end();
		--curView;
	}

	src = *curView;
	midUpdate = true;
	src->lastRangeStart = src->rangeStart;
	Refresh();
}

inline void EditPanel::DrawTri(wxDC &dc,
		int x1 , int y1  ,  int x2 , int y2 ,
		int x3 , int y3   ){
	
	wxPoint *p;
	p = new wxPoint[3];
	p[0] = wxPoint(x1, y1);
	p[1] = wxPoint(x2, y2);
	p[2] = wxPoint(x3, y3);

	dc.DrawPolygon(3, p);
	delete p;
}
inline void EditPanel::DrawQuad(wxDC &dc,
		int x1 , int y1  ,  int x2 , int y2 ,
		int x3 , int y3  ,  int x4 , int y4 ){
	
	wxPoint *p;
	p = new wxPoint[4];
	p[0] = wxPoint(x1, y1);
	p[1] = wxPoint(x2, y2);
	p[2] = wxPoint(x3, y3);
	p[3] = wxPoint(x4, y4);

	dc.DrawPolygon(4, p);
	delete p;
}

inline void EditPanel::DrawOct(wxDC &dc,
		int x1 , int y1  ,  int x2 , int y2 ,
		int x3 , int y3  ,  int x4 , int y4 ,
	    int x5 , int y5  ,  int x6 , int y6 ,
	 	int x7 , int y7  ,  int x8 , int y8 ){
	
	wxPoint *p;
	p = new wxPoint[8];
	p[0] = wxPoint(x1, y1);
	p[1] = wxPoint(x2, y2);
	p[2] = wxPoint(x3, y3);
	p[3] = wxPoint(x4, y4);
	p[4] = wxPoint(x5, y5);
	p[5] = wxPoint(x6, y6);
	p[6] = wxPoint(x7, y7);
	p[7] = wxPoint(x8, y8);

	dc.DrawPolygon(8, p);
	delete p;
}
inline void EditPanel::DrawHex(wxDC &dc,
		int x1 , int y1  ,  int x2 , int y2 ,
		int x3 , int y3  ,  int x4 , int y4 ,
	    int x5 , int y5  ,  int x6 , int y6 ){
	
	wxPoint *p;
	p = new wxPoint[6];
	p[0] = wxPoint(x1, y1);
	p[1] = wxPoint(x2, y2);
	p[2] = wxPoint(x3, y3);
	p[3] = wxPoint(x4, y4);
	p[4] = wxPoint(x5, y5);
	p[5] = wxPoint(x6, y6);

	dc.DrawPolygon(6, p);
	delete p;
}

void EditPanel::OnEraseBackground(wxEraseEvent& event) {}


int EditPanel::DrawChar( wxDC& dc, wxChar k, int x, int y ){
	int x2 = x;


	if(k=='/'){
		slashCount++;
		if(slashCount == 2){
			comment = true;
			wordColor = wxColour( 0x00, 0xff, 0x00 );
		}
	} else {
		slashCount = 0;
	}
	if(!comment) {
		switch (k){
			case '{': case '}': case ';': case ':':
				dc.SetTextForeground( wxColour( 0xff, 0x44, 0x33 ) );
				break;
			case '(': case ')': case ',':
				dc.SetTextForeground( wxColour( 0xff, 0x55, 0x88 ) );
				break;
			case '"': case '\'':
				dc.SetTextForeground( wxColour( 0xff, 0x00, 0xff ) );
				break;
			case '+': case '-': case '*': case '/': case '%':
			case '!': case '^': case '|': case '&': case '=':
				dc.SetTextForeground( wxColour( 0x33, 0xff, 0x33 ) );
				break;
			default:
				dc.SetTextForeground( wordColor );
		}
	} else {
		dc.SetTextForeground( wxColour( 0xcc, 0xcc, 0xcc ) );
	}
	if(k == '\t') x2 += tabstop * fontWidth;
	else          x2 += fontWidth;

	//cout("%d", x1);
	dc.DrawText( k, x, y );
	return x2;
}

int EditPanel::DrawWord( wxDC& dc, wxString& str, int x, int y ){
	return x;
}

int EditPanel::DrawLine( wxDC& dc, wxString& str, int x, int y ){
	wordColor = wxColour( 0xcc, 0xcc, 0xcc );
	fontWidth = dc.GetCharWidth(); fontHeight = dc.GetCharHeight();
	int x1 = x;
	slashCount = 0;
	//cout2("font,mid Width = %d,%d", fontWidth, midWidth);
	for(unsigned int i=0; i < str.Len(); i++){
		x1 = DrawChar(dc, str[i], x1, y);
		//cout("%d", x1);
	}
	comment = false;
	return x1;
	//dc.DrawText( str , x, y );
}

inline void EditPanel::CalcViewArea(wxDC& dc){

	//if( !src->IsOpened() ){
	//	src->curLine = 0;
	//	src->curCol  = 0;
	//	src->curChar = 255;
	//}

	//cout2("%d,%d", curLine, curCol);

	src->rangeEnd = src->rangeStart + src->rangeLength;

	//chOffset = 0;
	my  = src->GetZoomTop(); //+ chOffset;
	ml  = src->GetZoomHeight(); //+ chOffset;

	sw  = 100;

	sx1 = src->left;
	ex1 = width/2 - 80 * midWidth/2 ;//sw - overlap;

	sy1 = my;
	sx2 = sx1 + src->width;
	sy2 = sy1 + ml;

	ey1 = height / 2 - src->rangeLength * midHeight / 2; //src->titleOffset; //20 - midHeight;
	ey2 = ey1 + src->rangeLength * midHeight;
	ex2 = ex1 + 80 * midWidth ;//width - sw + overlap;

	/*
	dc.SetFont(src->font);
	src->fontHeight = dc.GetCharHeight();
	src->fontWidth = dc.GetCharWidth();
	*/
	
	//wow look at me, I'm performing all these operations each time anything
	//ever happens. I sure wish I could save some computational cycles by
	//caching this and updating only when needed.
	showall = false;
	switch(cursormode){
		case epcChar:
			scx1 = sx1 + src->fontWidth  * src->GetLeft(src->curCol);
			scx2 = scx1 + src->fontWidth * src->GetCharWidth(src->curChar);
			scy1 = sy1 + src->fontHeight * (src->curLine - src->rangeStart);
			scy2 = scy1 + src->fontHeight;

			mcx1 = ex1 + midWidth * src->GetLeft(src->curCol);
			mcx2 = mcx1 + midWidth * src->GetCharWidth(src->curChar);// - midWidth / 2,
			mcy1 = ey1 + midHeight * (src->curLine - src->rangeStart);
			mcy2 = mcy1 + midHeight;
			break;

		case epcFile:
			scx1 = src->left;
			scx2 = scx1 + src->width;
			scy1 = src->top;
			scy2 = scy1 + src->height;

			mcx1 = ex1;
			mcx2 = ex2;
			mcy1 = ey1;
			mcy2 = ey2;

			showall = true;
			break;
		case epcLine:
			scx1 = sx1;
			scx2 = scx1 + src->fontWidth * src->GetLeft( src->GetLine( src->curLine ).Len() );
			scy1 = sy1  + src->fontHeight * (src->curLine - src->rangeStart);
			scy2 = scy1 + src->fontHeight;

			mcx1 = ex1;
			mcx2 = mcx1 + midWidth * src->GetLeft( src->GetLine( src->curLine ).Len() ) + midWidth / 2;
			mcy1 = ey1 + midHeight * ( src->curLine - src->rangeStart );
			mcy2 = mcy1 + midHeight;
			break;
		case epcBlock: /*{
			blkStartRow = 0, blkStartCol = 0, blkEndRow = 0, blkEndCol = 0;
			for(int i = curLine; i >= 0; i--){
				for(int j = (*src)[curLine].Len() - 1; j >= 0; j++){
					if((*src)[i].GetChar(j) == '{'){
						blkStartRow = i;
						blkStartCol = j;
						break;
					}
				}
				if(blkStartRow != 0) {
					break;
				}
			}
			int braces = 1;
			unsigned int j = blkStartCol;
			for(unsigned int i = blkStartRow; i < (*src).GetLineCount(); i++){
				for(; j < (*src)[i].Len(); j++){
					wxChar chb = (*src)[i].GetChar(j);
					if(chb == '{') braces++;
					else if(chb == '}') braces--;
					if(braces == 0) break;
				}
				if(braces == 0) {
					blkEndRow = i;
					blkEndCol = j;
					break;
				}
				j = 0;
			}

			curCol = blkStartCol;
			curLine = blkStartRow;
		*/
			scx1 = sx1;
			scx2 = scx1 + src->fontWidth * src->GetLeft(src->GetLine(src->curLine).Len() );
			scy1 = sy1 + src->fontHeight * (src->curLine - src->rangeStart);
			scy2 = scy1 + src->fontHeight;

			mcx1 = ex1;
			mcx2 = mcx1 + midWidth * src->GetLeft(src->GetLine(src->curLine).Len() ) + midWidth/2,
			mcy1 = ey1 + midHeight * (src->curLine - src->rangeStart);
			mcy2 = mcy1 + midHeight;
			break;
		default: // character mode
			scx1 = sx1 + src->fontWidth  * src->GetLeft(src->curCol);
			scx2 = scx1 + src->fontWidth * src->GetCharWidth(src->curChar);
			scy1 = sy1 + src->fontHeight * (src->curLine - src->rangeStart);
			scy2 = scy1 + src->fontHeight;

			mcx1 = ex1 + midWidth * src->GetLeft(src->curCol);
			mcx2 = mcx1 + midWidth * src->GetCharWidth(src->curChar);// - midWidth / 2,
			mcy1 = ey1 + midHeight * (src->curLine - src->rangeStart);
			mcy2 = mcy1 + midHeight;
			break;
	}
	//cout2("CVA: %d %d", mcx1, mcx2);

}

void EditPanel::DrawSmallView(wxDC& dc, SmallView *cv, bool background){
	if( background ){
		//dc.SetBrush( wxColour( 0,0,0, 0xC0 ) );
		dc.DrawRectangle( cv->left, cv->top, cv->width, cv->height );
	}

	dc.DrawBitmap( cv->smallBmp, cv->left, cv->top );

	dc.SetBrush( wxColour( 0,0,0, 0x80 ) );
	dc.DrawRectangle(
			cv->left, 
			cv->top + cv->titleOffset, 
			cv->width,
			cv->GetZoomTop() - cv->top - cv->titleOffset
			);
	dc.DrawRectangle( 
			cv->left,
			cv->GetZoomHeight() + cv->GetZoomTop(), 
			cv->width,
			cv->height - cv->GetZoomTop() - cv->GetZoomHeight()
			);
}
void EditPanel::DrawSmallViews(wxDC& dc, bool background){
	SmallViewList::iterator cview = views.begin();
	while( cview != views.end() ){
		SmallView *cv = *cview;
		DrawSmallView(dc, cv, background);
		++cview;
	}
}

inline void EditPanel::DrawOutlines(wxDC& dc){
	int outlineAlpha = 0x50;
	// small outline
	/*
	if(showall){
		dc.SetBrush( wxColour( 0x66, 0x00, 0x00, 0x66 ) );
		dc.SetPen(   wxColour( 0x00, 0x33, 0x77 ) );
	} else {
	*/
	dc.SetBrush( wxColour( 0x22, 0x33, 0x44, outlineAlpha ) );
	dc.SetPen(   wxColour( 0x00, 0x33, 0x77 ) );
	//}
	dc.DrawRectangle( sx1, sy1, sx2 - sx1, sy2 - sy1 );

	// small midline
	dc.SetBrush( wxColour( 0x00, 0x88, 0x44, outlineAlpha ) );
	dc.SetPen(   wxColour( 0x00, 0x88, 0x44, outlineAlpha ) );
	dc.DrawRectangle( sx1, scy1, sx2 - sx1, src->fontHeight );


}

inline void EditPanel::DrawZoomVectors(wxDC& dc){
	int vectorAlpha = 0x50;

	dc.SetPen( wxNullPen );
	dc.SetBrush( wxColour( 0x22, 0x33, 0x55, vectorAlpha ) );

	// top / bottom zoom outlines
	DrawQuad( dc, sx1, sy1,   ex1, ey1,   ex2, ey1,   sx2,sy1   );
	DrawQuad( dc, sx1, sy2,   ex1, ey2,   ex2, ey2,   sx2,sy2   );

}

inline void EditPanel::DrawVectors(wxDC& dc){
	int vectorAlpha = 0x90;

	dc.SetPen( wxNullPen );
	dc.SetBrush( wxColour( 0x22, 0x33, 0x55, vectorAlpha ) );

	// line vector left / right
	dc.SetBrush( wxColour( 0x00, 0x33, 0x11, vectorAlpha ) );
	DrawQuad(dc, ex1 , mcy1 ,
				 sx1 , scy1 ,
				 sx1 , scy2 ,
				 ex1 , mcy2 );

	DrawQuad(dc, sx2 , scy2,
				 sx2 , scy1 ,             
				 ex2 , mcy1 ,   
				 ex2 , mcy2 );

	// top / bottom line surface vectors
	
	dc.SetBrush( wxColour( 0x11, 0x33, 0x22, vectorAlpha ) );
	DrawQuad(dc, ex1 , mcy1 ,
				 ex2 , mcy1 ,
				 sx2 , scy1 ,
				 sx1 , scy1 );
	DrawQuad(dc, ex1 , mcy2 ,
				 ex2 , mcy2 ,
				 sx2 , scy2 ,
				 sx1 , scy2 );

	//middle line
	if(!showall) {
		wxBrush curVecBrush( wxColour( 0x00, 0x55, 0x00, vectorAlpha ) );
		dc.SetBrush( curVecBrush );
		dc.DrawRectangle( ex1, mcy1, ex2 - ex1, midHeight );
	}
}

void EditPanel::DrawCursor(wxDC& dc){
	// cursor vector
	/*
	DrawQuad(dc,scx1, scy1 ,
				mcx1, mcy1 ,
				mcx2, mcy1 ,
				scx2, scy1 );
	DrawQuad(dc,scx1 , scy2 ,
				mcx1 , mcy2 ,
				mcx2 , mcy2 ,
				scx2 , scy2 );
	DrawQuad(dc,scx1 , scy1 ,
				mcx1 , mcy1 ,
				mcx1 , mcy2 ,
				scx1 , scy2 );
	DrawQuad(dc,scx2 ,  scy1 ,
				mcx2 ,  mcy1 ,
				mcx2 ,  mcy2 ,
				scx2 ,  scy2 );
	*/
	//DrawTri(dc, mcx1, mcy1, mcx1, mcy2, mcx2, mcy2 );
	DrawHex(dc, 
			scx1,scy1,
			scx2,scy1,
			mcx2,mcy1,
			mcx2,mcy2,
			mcx1,mcy2,
			scx1,scy2  );

	// front panel
	if(showall) {
		dc.SetBrush( *bfpShowAll );
		cursorBrush->SetColour( wxColour( 0x00, 0x00, 0x22, 0x61 ) );
	} else {
		dc.SetBrush( *frontPanelBrush );
		//frontPanelBrush->SetColour( wxColour( 0x00, 0x11, 0x33, 0x90 ) );
		cursorBrush->SetColour( wxColour( 0x00, 0xff, 0x00, 0x9f ) );
	}

	//dc.SetBrush( *frontPanelBrush ); 
	int border = 5;
	dc.DrawRectangle(ex1 - border, ey1 - border, ex2 - ex1 + border, ey2 - ey1 + border);

	dc.SetBrush( *cursorBrush );
	dc.DrawRectangle( mcx1 , mcy1 , mcx2 - mcx1 , mcy2 - mcy1 );
	dc.DrawRectangle( scx1 , scy1 , scx2 - scx1 , scy2 - scy1 );
}

void EditPanel::OnPaint(wxPaintEvent& event){
	wxDateTime now = wxDateTime::UNow();

	//wxBufferedPaintDC dc( this, m_bmp);
	wxPaintDC dc(this);
	PrepareDC( dc );
	dc.SetBackground( wxColour( 0,0,0 ) );
	dc.Clear();
	dc.SetFont(src->font);

	CalcViewArea(dc);

	//wxMemoryDC memDC;
	//memDC.SelectObjectAsSource(src->smallBmp);
	//dc.DrawBitmap( src2->smallBmp, src2->left, src2->top );
	//dc.Blit( 0, 0, src->smallBmp.GetWidth(), src->smallBmp.GetHeight(), &memDC, 0, 0 );
	
	//wxMemoryDC midDC;
	//midDC.SelectObjectAsSource( midBmp );
	//dc.Blit(ex1, ey1, midBmp.GetWidth(), midBmp.GetHeight(), &midDC, 0, 0);


	if(!showall) {
		if(midUpdate) DrawMidText();
	   	DrawSmallViews(dc);
		DrawOutlines(dc);
		DrawZoomVectors(dc);
		DrawVectors(dc);
		DrawCursor(dc);
		dc.DrawBitmap(midBmp, ex1, ey1);
	} else {
/*		if(midUpdate) DrawMidText();
		DrawOutlines(dc);
		dc.DrawBitmap(midBmp, ex1, ey1);
		DrawZoomVectors(dc);
		DrawVectors(dc);
		DrawCursor(dc);
		DrawSmallViews(dc);
		dc.SetBrush(wxColour(0x00,0xa0,0x00,0x50 ));
		DrawSmallView(dc, src, true);
*/
		if(midUpdate) DrawMidText();
		DrawVectors(dc);
		DrawCursor(dc);
		dc.DrawBitmap(midBmp, ex1, ey1);
		dc.SetBrush(wxColour(0x00,0x00,0x00,0x50 ));
	   	DrawSmallViews(dc, true);
		DrawZoomVectors(dc);
		DrawOutlines(dc);
		dc.SetBrush(wxColour(0x00,0xa0,0x00,0x50 ));
		DrawSmallView(dc, src, true);
	}

	dc.SetFont( midFont );
	dc.DrawText( src->curChar, mcx1, mcy1 );

	dc.SetTextForeground( wxColour( 0xff, 0xff, 0xff ) );
	wxString statusLine = wxString::Format(_T("%d:%d=%2x"), src->curLine,  src->curCol, src->curChar);
	dc.DrawText(statusLine, width - midWidth * statusLine.length() - 5, height - midHeight * 5);


	wxDateTime now2 = wxDateTime::UNow();
	wxTimeSpan span = now2.Subtract(now);
	cout("Drawn in %d ms", span.GetMilliseconds().ToLong() );

}

void EditPanel::MatchBracket(){
	char chOpen [4] = { '{', '(', '[', '"' };
	char chClose[4] = { '}', ')', ']', '"' };
	char chBracket1 = 0, chBracket2 = 0;
	int direction = 0;

	for(int k = 0; k < 4; k++){
		if(src->curChar == chOpen[k]) {
			chBracket1 = chClose[k];
			chBracket2 = chOpen[k];
			direction = 1;
			break;
		} else if(src->curChar == chClose[k]){
			chBracket1 = chOpen[k];
			chBracket2 = chClose[k];
			direction = -1;
			break;
		}
	}

	int j = src->curCol + direction;
	if(direction != 0) {
		int brackets = 1;
		for(int i = src->curLine; i >= 0 && i < (signed int)(*src).GetLineCount(); i += direction){


			for(; j >= 0 && j < (signed int) (*src)[i].Len(); j += direction){

				cout2("   %d:%d", i, j);

				if((*src)[i].GetChar(j) == chBracket1){
					brackets--;
				} else if( src->GetLine(i).GetChar(j) == chBracket2 ) {
					brackets++;
				}
				if(brackets == 0){
					src->curCol = j;
					src->curLine = i;
					Refresh();
					return;
				}
			}

			if(direction < 0) {
				if( i - 1 > 0 ){
				   	j = (*src)[i-1].Len() - 1;
				}
			}
			else j = 0;
		}
	}
}

void EditPanel::OnMouseWheel( wxMouseEvent& event ){
	int dl = event.GetLinesPerAction();
	int dw = event.GetWheelDelta();
	int dr = event.GetWheelRotation();
	int r = dl * dr / dw;
	if( r < 0 ) r = -r;
	if( dr > 0 ){ 
		src->ScrollUp(r);
		DrawMidText();
		Refresh();
	} else if( dw > 0 ){
		src->ScrollDown(r);
		DrawMidText();
		Refresh();
	}

}

void EditPanel::OnMouseMotion( wxMouseEvent& event ){
	if(event.LeftIsDown()){
		int x = event.GetX(), y = event.GetY();
		if( x < 80 ){ 
			int ly = y / src->fontHeight;
			int lyc = ly - src->rangeLength / 2;
			if(lyc < 0) lyc = 0;
			src->lastRangeStart = src->rangeStart;
			src->rangeStart = lyc;
			src->rangeEnd = lyc + src->rangeLength;
			midUpdate = true;
			Refresh();
		}
	}
}

void EditPanel::Smaller(){
	src->Resize(src->fontSize - 1);
	src->Alloc();
	src->DrawSmallText();
	Refresh();
	
}
void EditPanel::Larger(){
	src->Resize(src->fontSize + 1);
	src->Alloc();
	src->DrawSmallText();
	Refresh();
}
void EditPanel::ClearBmp(wxBitmap& bmp){
	int w = bmp.GetWidth(), h = bmp.GetHeight();
	wxAlphaPixelData data(bmp, wxPoint(0,0), wxSize(w,h) );
	wxAlphaPixelData::Iterator p(data);
	for(int y=0; y < h; y++){
		wxAlphaPixelData::Iterator rowStart = p;
		for(int x=0; x < w; x++){
			p.Alpha() = 0;
			p.Red() = 0;
			p.Green() = 0;
			p.Blue() = 0;
			p++;
		}
		p = rowStart;
		p.OffsetY(data,1);
	}
}

void EditPanel::RedrawLine(){
	if( midBmp.IsOk() ){
		wxBufferedDC dc(NULL, midBmp, wxBUFFER_VIRTUAL_AREA);
		// Clear bitmap line
		
		int firstY = midHeight * (src->curLine - src->rangeStart);
		if(firstY > 0) {
	
			int w = midBmp.GetWidth();//, h = firstY + midHeight;
			int h = midBmp.GetHeight();
			wxAlphaPixelData data(midBmp, wxPoint(0,0), wxSize(w, h) );
			wxAlphaPixelData::Iterator p(data);
			p.MoveTo(data, 0, firstY);
			for(int y=0; y < midHeight; y++){
				wxAlphaPixelData::Iterator rowStart = p;
				for(int x=0; x < w; x++){
					p.Alpha() = 0;
					p.Red() = 0;
					p.Green() = 0;
					p.Blue() = 0;
					p++;
				}
				p = rowStart;
				p.OffsetY(data,1);
			}
			// now draw the new line
			dc.SetFont(midFont);
			DrawLine( dc, (*src).GetLine(src->curLine) , 0, firstY  );
	
		}
	}
}

void EditPanel::DrawMidText(){
	if( ! midBmp.IsOk() ){
		int w = midWidth * 80, h = midHeight * 43;
		cout2("Allocating %dx%d mid raster", w, h);
		midBmp.Create(w, h, 32);
	}

	wxBitmap bm(midBmp);
	//midBmp.Create(width, height, 32);

	//wxBitmap bm = midBmp.GetSubBitmap( wxRect( 0, 0, midClearBmp.GetWidth(), midClearBmp.GetHeight() ) );

	wxMemoryDC dc;
	dc.SelectObject(midBmp);
	//wxBufferedDC dc(NULL, midBmp, wxBUFFER_VIRTUAL_AREA);

	ClearBmp(midBmp); // wait, the whole thing?

	//dc.SetBrush(wxBrush(wxColour(0,0,0)));
	//dc.SetBackground(wxBrush(wxColour(0,0,0)));
	//dc.DrawRectangle(0, 0, midBmp.GetWidth(), midBmp.GetHeight());

	dc.SetTextForeground( wxColor( 244,244,244 ) );
	dc.SetFont(midFont);

	int i = 0;
	//fontWidth = midWidth;
	//fontHeight = midHeight;

	int dr = src->lastRangeStart - src->rangeStart; 
	if(dr < 0) { // scroll down
		//dr = -dr;
		//midBmp = midClearBmp.GetSubBitmap( wxRect( 0, 0, midClearBmp.GetWidth(), midClearBmp.GetHeight() ) );
		//dc.DrawBitmap( bm, 0,0 );
		int drawLength = (src->rangeLength + dr);
		dc.DrawBitmap( bm, 0, dr * midHeight);

		i += drawLength;

		while( i < src->rangeLength && i + src->rangeStart < (signed int)( src->GetLineCount() ) ){
			DrawLine( dc, src->GetLine(src->rangeStart + i) , 0, i * midHeight  );
			i++;
		}
		//cout("Redrew %d", dr);

	} else if(dr > 0){ // scroll up
		//midBmp = midClearBmp.GetSubBitmap( wxRect( 0, 0, midClearBmp.GetWidth(), midClearBmp.GetHeight() ) );
		//dc.DrawBitmap( bm, 0,0);
		int drawLength = dr;
		dc.DrawBitmap(bm, 0, dr * midHeight);
		i = 0;

		while( i < drawLength && i + src->rangeStart < (signed int) (src->GetLineCount()) ){
			DrawLine( dc, src->GetLine(src->rangeStart + i) , 0, i * midHeight  );
			i++;
		}
		//cout("Redrew %d", dr);
	} else { 
		while( i < src->rangeLength && i + src->rangeStart < (signed int) src->GetLineCount() ){
			DrawLine( dc, src->GetLine(src->rangeStart + i) , 0, i * midHeight  );
			i++;
		}
		//cout("Redrew all %d", i);
	}
	midUpdate = false;
}

void EditPanel::OnSize(wxSizeEvent& event){
}

void EditPanel::Away(){
	cursormode++;
	cout("Cursor mode %d",cursormode);
	Refresh();
}
void EditPanel::Into(){
	if(cursormode > 0) cursormode--;
	cout("Cursor mode %d",cursormode);
	Refresh();
}
void EditPanel::ShowAll(){
	cursormode = epcFile;
	Refresh();
}

void EditPanel::OnChar(wxKeyEvent& event) {
	cout("%c", event.GetUnicodeKey() );
	src->InsertChar( event.GetUnicodeKey() );
	RedrawLine();
	Refresh();

}
void EditPanel::OnKeyDown(wxKeyEvent& event){
	int keyCode = event.GetKeyCode();
	int shiftmode = event.GetModifiers();
	if(shiftmode == 0){
		switch(keyCode) {
			case WXK_DOWN:
				if(cursormode == epcFile){
					src->CursorDown(3);
					src->ScrollDown(3);
					DrawMidText();
				} else {
					src->CursorDown();
					if(src->Scroll()) DrawMidText();
				}
				SKIPMOD Refresh();
				break;
			case WXK_UP:
				if(cursormode == epcFile){
					src->CursorUp(3);
					src->ScrollUp(3);
					DrawMidText();
					Refresh();
				} else {
					src->CursorUp();
					if(src->Scroll()) DrawMidText();
					SKIPMOD Refresh();
				}
				break;
			case WXK_LEFT:
				if(cursormode == epcFile){
					Swap(-1);
				} else {
					src->CursorLeft();
				}
				SKIPMOD Refresh();
				break;
			case WXK_RIGHT:
				if(cursormode == epcFile){
					Swap();
				} else {
					src->CursorRight();
				}
				SKIPMOD Refresh();
				break;
			case WXK_BACK:
				src->RemoveChar();
				RedrawLine();
				Refresh();
				break;
			case WXK_DELETE:
				src->DeleteChar();
				RedrawLine();
				Refresh();
				break;
			case WXK_PRIOR: // pgup
				src->CursorUp(20);
				src->ScrollUp(20);
				DrawMidText();
				Refresh();
				break;
			case WXK_NEXT: // pgdn
				src->CursorDown(20);
				src->ScrollDown(20);
				DrawMidText();
				Refresh();
				break;
			case WXK_HOME: // home
				src->curCol = 0;
				Refresh();
				break;
			case WXK_END: // end
				src->curCol = (*src)[src->curLine].Len();
				Refresh();
				break;
			case WXK_ESCAPE:
				Away();
				break;
			case WXK_RETURN:
				if(cursormode == epcLine){
					Into();
				} else if(cursormode == epcChar){
					src->lastRangeStart = src->rangeStart;
					src->InsertLine(_T(""), src->curLine);
					src->curLine++;
					midUpdate = true;
					Refresh();
				} else {
					cursormode = epcChar;
					showModDown = false;
					Refresh();
				}
				break;
			default:
				event.Skip();
		}
	} else if(shiftmode == ZoomSrc_SHIFTMOD) {
		event.Skip();
	} else if(shiftmode == ZoomSrc_WORDMOD) { 
		switch(keyCode){
			case WXK_LEFT: // word left
				if(src->curCol > 0) src->curCol--;
				while( src->curCol > 0 && (*src)[src->curLine].GetChar(src->curCol) == ' '){
					src->CursorLeft();
				}
				while( src->curCol > 0 && (*src)[src->curLine].GetChar(src->curCol) != ' '){
					src->CursorLeft();;
				}
				src->CursorRight();
				Refresh();
				break;
			case WXK_RIGHT: // word right
				if(src->curCol < (*src)[src->curLine].length()) src->curCol++;
				while( src->curCol < src->GetLine(src->curLine).length() && src->GetLine(src->curLine).GetChar(src->curCol) != ' '){
					src->CursorRight();
				}
				while( src->curCol < src->GetLine(src->curLine).length() && src->GetLine(src->curLine).GetChar(src->curCol) == ' '){
					src->CursorRight();
				}
				Refresh();
				break;
			case WXK_UP:
				if(downNum <= skipNum) src->ScrollUp(1);
				else src->ScrollUp(skipNum);
				DrawMidText();
				Refresh();
				break;
			case WXK_DOWN:
				if(downNum <= skipNum) src->ScrollDown(1);
				else src->ScrollDown(skipNum);
				DrawMidText();
				Refresh();
				break;
			default:
				event.Skip();
		}
	} else if( shiftmode == ZoomSrc_APPLEMOD ){ // for CMD-LEFT
		switch(keyCode){
			case WXK_RIGHT:
				src->curCol = (*src)[src->curLine].Len();
				Refresh();
				break;
			case WXK_LEFT:
				src->curCol = 0;
				Refresh();
				break;
		}
	} else if( shiftmode == ZoomSrc_SHOWMOD ){
		showModDown = true;
		switch(keyCode){
			case WXK_DOWN:
				src->CursorDown(3);
				src->ScrollDown(3);
				DrawMidText();
				SKIPMOD Refresh();
				break;
			case WXK_UP:
				src->CursorUp(3);
				src->ScrollUp(3);
				DrawMidText();
				SKIPMOD Refresh();
				break;
			case WXK_LEFT:
				Swap(-1);
				break;
			case WXK_RIGHT:
				Swap();
				break;
		}
		cursormode = epcFile;
		Refresh();
	} else {
		cout("%x", shiftmode );
	}
	downNum++;
}
void EditPanel::OnKeyUp(wxKeyEvent& event){
	int d = downNum;
	downNum = 0;
	if(showModDown && event.GetModifiers() != ZoomSrc_SHOWMOD ){
		cursormode = epcChar;
		showModDown = false;
	}
	//if(d % skipNum == 0) 
	Refresh();
}
EditPanel::~EditPanel(){
}
int EditPanel::GetRangeLength() { return src->rangeLength; }

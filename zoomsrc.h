// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all <standard< wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#include <wx/textfile.h>
#include <wx/dcbuffer.h>
#include <wx/rawbmp.h>
#include <wx/filedlg.h>
#include <wx/ffile.h>

//#include <wx/bookctrl.h>
//#include <wx/propdlg.h> 
//#include <wx/generic/propdlg.h>
#include <wx/dialog.h>


#define wx_MAC_USE_CORE_GRAPHICS 1
#define __WXDEBUG__ 1

#define cout(x,y) { wxString str; str.Printf(_T(x), y); wxLogMessage(str); }
#define cout2(x,y,z) { wxString str; str.Printf(_T(x), y, z); wxLogMessage(str); }
#define cout3(x,y,z,w) { wxString str; str.Printf(_T(x), y, z, w); wxLogMessage(str); }
#define MENUCMD(x) void x(wxCommandEvent& event)
#define constrain( x, y, z ) ( (x) < (y) ) ? (y) : (  ( (x) > (z) ) ? (z) : (x) );

// Set up the keyboard modifier keys for different platforms
#ifdef __WXMAC__
	#define ZoomSrc_WORDMOD wxMOD_ALT
	#define ZoomSrc_APPLEMOD wxMOD_CMD
	#define ZoomSrc_SHOWMOD ( wxMOD_CMD + wxMOD_SHIFT )
#else
	#define ZoomSrc_WORDMOD wxMOD_CONTROL
	#define ZoomSrc_APPLEMOD wxMOD_ALL
	#define ZoomSrc_SHOWMOD ( wxMOD_ALT + wxMOD_SHIFT )
#endif

#define ZoomSrc_SHIFTMOD wxMOD_SHIFT


class SmallView : public wxTextFile {
public:
	SmallView(wxPanel *, const wxString&);
	void DrawSmallText();
	void CalcCurLineLen();
	void CalcNumLines();
	void CursorLeft (int n=1), CursorRight (int n=1), 
		 CursorUp   (int n=1), CursorDown  (int n=1);
	void Indent(), Undent();

	int GetLeft(int);
	bool Scroll();
	void ScrollUp(int);
	void ScrollDown(int);
	void RemoveChar();
	void DeleteChar();
	void Resize(int);
	void Alloc();
	int GetCharWidth(wxChar ch);
	int GetZoomTop();
	int GetZoomHeight();
	void InsertChar(wxChar c);
	void UpdateChar();
	int Left();

	wxBitmap smallBmp;
	wxFont font;
	int fontSize;
	int curCol, curLine;
	int curLineLen, numLines;
	int rangeStart, rangeEnd, lastRangeStart, rangeLength;
	int tabstop;
	int fontWidth, fontHeight;
	int titleOffset;
	int top, left, width, height;
	wxPanel *ep;
	wxString filename;
	wxChar curChar;
	//wxString curString;

	//~SmallView();
};

WX_DECLARE_LIST( SmallView, SmallViewList );

// EDIT PANEL
class EditPanel : public wxPanel {
public:
	EditPanel(){}
	EditPanel(wxWindow* parent, const wxString&);
    ~EditPanel();

	//bool LoadFile(const wxString&);

	int newRight;
	void GotoView(int);
	void Load(const wxString&);
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void SortViews();
	//int sortlefttoright(const void*,const void*);

	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent&);
	void MatchBracket();
	void Into(), Away(), Swap(int direction=1), ShowAll();
	void Larger(), Smaller();
	int GetRangeLength();
	bool showModDown;

	int DrawLine(wxDC&, wxString&, int,int);
	int DrawWord(wxDC&, wxString&, int,int);
	int DrawChar(wxDC&, wxChar, int, int);
	void GetCharSize(wxFont&, int&, int&);

	wxBrush *frontPanelBrush, *cursorBrush, *bfpShowAll;
	void SetFrontPanelColour(wxColour& col){ frontPanelBrush->SetColour(col); }
	wxColour& GetFrontPanelColour(){ return frontPanelBrush->GetColour(); }
private:

	MENUCMD( OnIndent );
	MENUCMD( OnUndent );

	void DrawOutlines(wxDC&);
	void DrawVectors(wxDC&);
	void DrawZoomVectors(wxDC&);
	void DrawCursor(wxDC&);
	//window
	wxWindow *m_parent;
	wxBitmap midBmp, m_bmp;
	wxFont smallFont, midFont;
	int midSize, smallSize, midHeight, midWidth, smallHeight, smallWidth;


	//int fontHeight, fontWidth;
	int cursormode;
	int overlap;
	int displaymode, width, height;
	bool midUpdate;
	bool showall;
	int downNum, skipNum;

	//temporal 
	int sx1, sx2, sy1, sy2, ex1, ex2, ey1, ey2, sw,
		scx1, scy1, scx2, scy2, mcx1, mcy1, mcx2, mcy2;
	int chOffset, my, ml;
	int blkStartRow, blkStartCol, blkEndRow, blkEndCol;
	wxChar curChar;

	bool comment, slashCount;
	wxColour wordColor;
	int tabstop, fontWidth, fontHeight;

	//file instance	
	SmallView *src;
	SmallViewList views;
	SmallViewList::iterator curView;

	//instance functions
	
	//syntax dependent
	wxArrayString wTypes, wKeyword;

	//drawing methods
	static inline void DrawTri(wxDC&, int,int,int,int, int,int );
	static inline void DrawQuad(wxDC&, int,int,int,int, int,int,int,int);
	static inline void DrawHex(wxDC&, int,int,int,int, int,int,int,int, int,int,int,int);
	static inline void DrawOct(wxDC&, int,int,int,int, int,int,int,int,
						int,int,int,int, int,int,int,int );
	void ClearBmp(wxBitmap&);

	//front panel
	void DrawMidText();
	void DrawSmallViews(wxDC&, bool background = false);
	void DrawSmallView(wxDC& dc, SmallView *cv, bool background = false);
	void RedrawLine();
	void CalcViewArea(wxDC& dc);

	DECLARE_DYNAMIC_CLASS(EditPanel);
	DECLARE_EVENT_TABLE();
};

enum {
	epcChar = 0,
	epcLine = 1,
	epcBlock = 2,
	epcFile = 3
};

#include <wx/notebook.h>
#include <wx/colordlg.h>
// A custom modal dialog
class PrefsDlg : public wxDialog {
public:
    PrefsDlg(wxWindow *parent);
	
	void SetButtonColour(wxBitmapButton* b, wxColour& c);

    //void OnButton(wxCommandEvent& event);
	MENUCMD( OnColour );

private:
	wxNotebook book;
	wxSizer *m_sizer;
	wxBookCtrlBase *m_book;
    //wxButton *m_btnModal,
    //         *m_btnModeless,
    //         *m_btnDelete;
	EditPanel *m_ep;

    DECLARE_EVENT_TABLE()
};
enum {
	Prefs_FrontPanel
};

// MAIN FRAME
class MainFrame : public wxFrame {
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	void Load(wxString&);
	MENUCMD( OnClose );
	MENUCMD( OnQuit );
	MENUCMD( OnAbout );
	MENUCMD( OnOpen );
	MENUCMD( OnMatchBracket );
	MENUCMD( OnAway );
	MENUCMD( OnInto );
	MENUCMD( OnLarger );
	MENUCMD( OnSmaller );
	MENUCMD( OnSwap );
	MENUCMD( OnShowAll );
	MENUCMD( OnGotoView );
	MENUCMD( OnPrefs );
    //void OnSetBlinkTime(wxCommandEvent& event);
    //void OnSetFontSize(wxCommandEvent& event);
    //void OnCaretMove(wxCommandEvent& event);
	wxMenu *menuFile, *menuEdit, *menuView, *menuGo;// = new wxMenu;
	void RebuildGo();
	int menuGoOffset;
private:
	EditPanel *m_editpanel;

	DECLARE_EVENT_TABLE()
};

enum {
	ZoomSrc_Prefs = wxID_PREFERENCES,
	ZoomSrc_New = wxID_NEW,
	ZoomSrc_Open = wxID_OPEN,
	ZoomSrc_Close = wxID_CLOSE,
	ZoomSrc_Save = wxID_SAVE,
	ZoomSrc_SaveAs = wxID_SAVEAS,
	ZoomSrc_Quit = wxID_EXIT,
	ZoomSrc_About = wxID_ABOUT,
	ZoomSrc_Cut = wxID_CUT,
	ZoomSrc_Copy = wxID_COPY,
	ZoomSrc_Paste = wxID_PASTE,
	ZoomSrc_Undo = wxID_UNDO,
	ZoomSrc_Redo = wxID_REDO,
	ZoomSrc_Delete = wxID_DELETE,
	ZoomSrc_SelectAll = wxID_SELECTALL,
	ZoomSrc_MatchBracket,
	ZoomSrc_ShowAll,
	ZoomSrc_GotoView,
	ZoomSrc_Away,
	ZoomSrc_Into,
	ZoomSrc_Swap,
	ZoomSrc_Larger,
	ZoomSrc_Smaller,
	ZoomSrc_Indent, ZoomSrc_Undent
};

// APPLICATION
class MainApp : public wxApp {
public:
	virtual bool OnInit();
	void OnOpen(wxCommandEvent&);
private:
	wxFFile *logtxt;
	wxLogStderr *err;

	DECLARE_EVENT_TABLE();
};

DECLARE_APP(MainApp)

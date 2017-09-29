#include "zoomsrc.h"
#include "ZoomSrc.xpm"
	
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(ZoomSrc_Close,  MainFrame::OnClose)
	EVT_MENU(ZoomSrc_About, MainFrame::OnAbout)
	EVT_MENU(ZoomSrc_Open,  MainFrame::OnOpen)
	EVT_MENU(ZoomSrc_MatchBracket, MainFrame::OnMatchBracket)
	EVT_MENU(ZoomSrc_Away, MainFrame::OnAway)
	EVT_MENU(ZoomSrc_Into, MainFrame::OnInto)
	EVT_MENU(ZoomSrc_Larger, MainFrame::OnLarger)
	EVT_MENU(ZoomSrc_Smaller, MainFrame::OnSmaller)
	EVT_MENU(ZoomSrc_Swap, MainFrame::OnSwap)
	EVT_MENU(ZoomSrc_ShowAll, MainFrame::OnShowAll)
	EVT_MENU(ZoomSrc_GotoView, MainFrame::OnGotoView)
	EVT_MENU(ZoomSrc_Prefs, MainFrame::OnPrefs)
//	EVT_MENU(ZoomSrc_Indent, MainFrame::OnIndent)
//	EVT_MENU(ZoomSrc_Undent, MainFrame::OnUndent)


	//EVT_MENU(ZoomSrc_Move, MyFrame::OnCaretMove)
END_EVENT_TABLE()
/*
void MainFrame::OnClose( wxCommandEvent& WXUNUSED(event) ){	
	Close(true); // true is to force the frame to close
}
*/

void MainFrame::OnPrefs(wxCommandEvent& event){
	PrefsDlg p(m_editpanel);
	p.ShowModal();
}

void MainFrame::OnGotoView(wxCommandEvent& event){
	//wxMenuItem *mi = (wxMenuItem *) event.GetEventObject();
	//wxString str;
	wxMenuItemList& mil = menuGo->GetMenuItems();
	for(int i = menuGoOffset; i < mil.GetCount(); i++){
		if(mil[i]->IsChecked()) {
			//str = mil[i]->GetItemLabel();
			m_editpanel->GotoView(i - menuGoOffset -1);
			break;
		}
	}
	//str.Printf( _T("Menu %d"), event.GetInt());
	//wxMessageBox(str);
}

void MainFrame::OnClose(wxCommandEvent& event){
	Close(true);
}
void MainFrame::OnShowAll(wxCommandEvent& event){
	m_editpanel->ShowAll();
}
void MainFrame::OnSwap(wxCommandEvent& event){
	m_editpanel->Swap();
}

void MainFrame::OnOpen(wxCommandEvent& event ){
	wxString filename = wxFileSelector(_T("Choose a file to open"));
	if( ! filename.empty() ) {
		//delete m_editpanel;
		//m_editpanel->LoadFile(filename);
		//m_editpanel->SetSize(300,300);
		Refresh();
	}
}

void MainFrame::Load( wxString& filename ){
//	m_editpanel->LoadFile(filename);
//	Refresh();
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size) {
	// set the frame icon
	SetIcon(wxICON(ZoomSrc));
	SetTransparent( 0xF0 );

	// create a menu bar
	menuFile = new wxMenu;

	/*
	menuFile->Append(Caret_SetBlinkTime, _T("&Blink time...\tCtrl-B"));
	menuFile->Append(Caret_SetFontSize, _T("&Font size...\tCtrl-S"));
	menuFile->Append(Caret_Move, _T("&Move caret\tCtrl-C"));
	menuFile->AppendSeparator();
	*/
	
	menuFile->Append(ZoomSrc_New,    _T("&New\tCtrl-N"), _T("New file") );
	menuFile->Append(ZoomSrc_Open,   _T("&Open...\tCtrl-O"), _T("Open a file from disk") );
	menuFile->AppendSeparator();
	menuFile->Append(ZoomSrc_Close,  _T("&Close\tCtrl-W"), _T("Close the current file") );
	menuFile->Append(ZoomSrc_Save,   _T("&Save\tCtrl-S"),        _T("Save changes to the current file") );
	menuFile->Append(ZoomSrc_SaveAs, _T("Save &As...\tCtrl-Shift-S"),  _T("Save to another file") );
	menuFile->AppendSeparator();
	menuFile->Append(ZoomSrc_About, _T("&About..."), _T("Show about dialog"));
	menuFile->AppendSeparator();
	menuFile->Append(ZoomSrc_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));
	
	menuEdit = new wxMenu;
	menuEdit->Append(ZoomSrc_Undo,  _T("&Undo\tCtrl-Z"),  _T("Undo the last command"));
	menuEdit->Append(ZoomSrc_Redo,  _T("&Redo\tCtrl-Y"),  _T("Redo the recently undone command"));
	menuEdit->AppendSeparator();
	menuEdit->Append(ZoomSrc_Cut,   _T("Cu&t\tCtrl-X"),   _T("Cut the selected text"));
	menuEdit->Append(ZoomSrc_Copy,  _T("&Copy\tCtrl-C"),  _T("Copy the selected text"));
	menuEdit->Append(ZoomSrc_Paste, _T("&Paste\tCtrl-V"), _T("Paste at the insertion point"));
	menuEdit->AppendSeparator();
	menuEdit->Append(ZoomSrc_Delete,_T("&Delete\tCtrl-D"),_T("Delete current line") );
	//menuEdit->Append(ZoomSrc_SelectAll,_T("Select &All\tCtrl-A"),_T("Extend selection") );
	menuEdit->Append(ZoomSrc_MatchBracket,_T("Match &Bracket\tCtrl-B"),_T("Go to the matching bracket") );
	menuEdit->Append(ZoomSrc_Indent, _T("Indent\tTab"), _T("") );
	menuEdit->Append(ZoomSrc_Undent, _T("Undent\tShift+Tab"), _T("") );
	menuEdit->AppendSeparator();
	menuEdit->Append(ZoomSrc_Prefs,  _T("Preferences...\tCtrl-,"), _T("Bring up the options panel") );

	menuView = new wxMenu;
	menuView->Append(ZoomSrc_Smaller, _T("Smaller\tCtrl--"), _T("Reduce the small view text size") );
	menuView->Append(ZoomSrc_Larger, _T("Larger\tCtrl-="), _T("Increase the small view text size") );

	menuGo = NULL;
	RebuildGo();

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, _T("&File"));
	menuBar->Append(menuEdit, _T("&Edit"));
	menuBar->Append(menuView, _T("&View"));
	menuBar->Append(menuGo,   _T("&Go") );
	SetMenuBar(menuBar);

	m_editpanel = new EditPanel(this, _T("zoomsrc.h"));
/*
#if wxUSE_STATUSBAR
	// create a status bar just for fun (by default with 1 pane only)
	CreateStatusBar(2);
	SetStatusText(_T("ZoomSrc ready."));
#endif // wxUSE_STATUSBAR
*/
}

void MainFrame::RebuildGo(){
	if(menuGo != NULL) delete menuGo;
	menuGo = new wxMenu;
	menuGo->Append(ZoomSrc_Away, _T("Away\tCtrl-Up"), _T("Move away from the current position") );
	menuGo->Append(ZoomSrc_Into, _T("Into\tCtrl-Down"), _T("Move away from the current position") );
	menuGo->AppendSeparator();
	menuGo->Append(ZoomSrc_Swap, _T("Swap\tCtrl-U"), _T("Switch to the other open file") );
	menuGo->Append(ZoomSrc_ShowAll, _T("Show all\tCtrl-A"), _T("Show all source views") );
	menuGoOffset = menuGo->GetMenuItemCount();
}

void MainFrame::OnAway(wxCommandEvent& event){
	m_editpanel->Away();
}
void MainFrame::OnInto(wxCommandEvent& event){
	m_editpanel->Into();
}

void MainFrame::OnLarger(wxCommandEvent& event){
	m_editpanel->Larger();
}
void MainFrame::OnSmaller(wxCommandEvent& event){
	m_editpanel->Smaller();	
}

void MainFrame::OnMatchBracket(wxCommandEvent& event){
	m_editpanel->MatchBracket();
}
void MainFrame::OnAbout(wxCommandEvent& event){
	wxMessageBox( _T("ZoomSrc Copyright © D. Casey Tucker 2008") );
}

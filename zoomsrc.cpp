#include "zoomsrc.h"

BEGIN_EVENT_TABLE(MainApp, wxApp)
	EVT_MENU(ZoomSrc_Open,  MainApp::OnOpen)
END_EVENT_TABLE()

IMPLEMENT_APP(MainApp)

void MainApp::OnOpen(wxCommandEvent& event){
	wxString filename = wxFileSelector(_T("Choose a file to open"));
	if( ! filename.empty() ) {
		MainFrame *frame = new MainFrame(filename,
				wxPoint(50,50), wxSize(900,700) );
		frame->Load(filename);
		frame->Show(true);
	}
}

bool MainApp::OnInit(){
	//setup logging
	logtxt = new wxFFile( _T("log.txt"), _T("w") );
	err = new wxLogStderr(logtxt->fp());
	wxLog::SetActiveTarget(err);

//	wxApp::SetExitOnFrameDelete(false);
	wxMenuBar *menubar = new wxMenuBar;
	wxMenu *menuFile = new wxMenu;
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
	menubar->Append(menuFile, _T("&File"));
	wxMenuBar::MacSetCommonMenuBar(menubar);

	MainFrame *frame = new MainFrame(_T("ZoomSrc"),
		wxPoint(50, 50), wxSize(900, 700));
	frame->Show(true);

	/*
	MainFrame *frame2 = new MainFrame(_T("ZoomSrc2"),
		wxPoint(50, 50), wxSize(900, 700));
	frame2->Show(true);
	*/
	return true;
}


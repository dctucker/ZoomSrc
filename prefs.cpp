#include "zoomsrc.h"

//IMPLEMENT_CLASS(PrefsDlg, wxDialog)

BEGIN_EVENT_TABLE(PrefsDlg, wxDialog)
	EVT_BUTTON(Prefs_FrontPanel, PrefsDlg::OnColour)
END_EVENT_TABLE()

void PrefsDlg::OnColour(wxCommandEvent &event){
	wxColourData cdata;
	wxBitmapButton *button = (wxBitmapButton *)( event.GetEventObject() );

	wxColour color( m_ep->GetFrontPanelColour() );
	unsigned char red, green, blue, alpha;
	alpha = color.Alpha();
	
	cdata.SetColour( color );
	wxColourDialog cdlg(this, &cdata);
	if ( cdlg.ShowModal() == wxID_OK ){
		wxColourData retData = cdlg.GetColourData();
		color = retData.GetColour();
		SetButtonColour( button, color );

		red = color.Red();
		green = color.Green();
		blue = color.Blue();
		color = wxColour(red,green,blue,alpha);

		m_ep->SetFrontPanelColour(color);
		m_ep->Refresh();
	}
}

void PrefsDlg::SetButtonColour(wxBitmapButton* b, wxColour& c){
	wxBitmap bitmap( 30, 20 );
	wxColour color( c.Red(), c.Green(), c.Blue(), 0xff );
	wxMemoryDC dc;
	dc.SelectObject( bitmap );
	dc.SetBackground( color );
	dc.Clear();
	dc.SelectObject( wxNullBitmap );
	b->SetBitmapLabel( bitmap );
}

PrefsDlg::PrefsDlg(wxWindow *parent) 
             : wxDialog(parent, wxID_ANY, wxString(_T("Preferences"))) {

	m_ep = (EditPanel*) parent;
	m_sizer = new wxBoxSizer(wxVERTICAL);
	//m_panel->SetSizer(m_sizer);
	m_book = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxNB_TOP);

	wxPanel *panel = new wxPanel(m_book);

	(void)new wxStaticText(panel, wxID_ANY, _T("Front panel"), wxPoint( 20, 20 ) );
	wxBitmapButton *b1 = new wxBitmapButton(panel, Prefs_FrontPanel, wxNullBitmap, wxPoint(120, 20), wxSize(40,30) );
	SetButtonColour(b1, m_ep->GetFrontPanelColour() );

	//(void)new wxBitmapButton( panel, Prefs_FrontPanel, bitmap, wxPoint(10,10), wxSize(120,30) );
	m_book->AddPage(panel, _T("Colors"), true );
}


#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/string.h>
#endif
#include <wx/settings.h>
#include <wx/statline.h>
#include "byogameselect.h"
#include "byogame.h"

//(*InternalHeaders(byoGameSelect)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(byoGameSelect)
const wxWindowID byoGameSelect::ID_STATICTEXT1 = wxNewId();
const wxWindowID byoGameSelect::ID_PANEL1 = wxNewId();
const wxWindowID byoGameSelect::ID_LISTBOX1 = wxNewId();
const wxWindowID byoGameSelect::ID_STATICLINE1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(byoGameSelect,wxScrollingDialog)
	//(*EventTable(byoGameSelect)
	//*)
END_EVENT_TABLE()

byoGameSelect::byoGameSelect(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(byoGameSelect)
	Create(parent, id, _("Select game to play"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Panel1->SetBackgroundColour(wxColour(0,0,128));
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("BYO Games collection"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
	StaticText1->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	StaticText1->SetBackgroundColour(wxColour(0,0,128));
	wxFont StaticText1Font(16,wxFONTFAMILY_SWISS,wxFONTSTYLE_ITALIC,wxFONTWEIGHT_NORMAL,true,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText1->SetFont(StaticText1Font);
	BoxSizer3->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 4);
	Panel1->SetSizer(BoxSizer3);
	BoxSizer1->Add(Panel1, 0, wxEXPAND, 4);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Select game"));
	m_GamesList = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(m_GamesList, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 4);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	Button1 = new wxButton(this, wxID_OK, _("Play"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
	BoxSizer2->Add(Button1, 0, wxBOTTOM|wxEXPAND, 4);
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	BoxSizer2->Add(StaticLine1, 0, wxTOP|wxBOTTOM|wxEXPAND, 4);
	Button2 = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	BoxSizer2->Add(Button2, 0, wxTOP|wxBOTTOM|wxEXPAND, 4);
	BoxSizer2->Add(-1,-1,0, wxEXPAND, 4);
	StaticBoxSizer1->Add(BoxSizer2, 0, wxLEFT|wxRIGHT|wxEXPAND, 4);
	BoxSizer4->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 4);
	BoxSizer1->Add(BoxSizer4, 1, wxBOTTOM|wxEXPAND, 4);
	SetSizer(BoxSizer1);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(byoGameSelect::OnPlay));
	Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(byoGameSelect::OnCancel));
	//*)

	for ( int i=0; i<byoGameLauncher::GetGamesCount(); ++i )
	{
	    m_GamesList->Append(byoGameLauncher::GetGameName(i));
	}

	m_GamesList->SetSelection(0);
}

byoGameSelect::~byoGameSelect()
{
}


void byoGameSelect::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(-1);
}

void byoGameSelect::OnPlay(wxCommandEvent& /*event*/)
{
    if ( m_GamesList->GetSelection() == wxNOT_FOUND ) return;
    EndModal(m_GamesList->GetSelection());
}

/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef FILEPATHPANEL_H
#define FILEPATHPANEL_H


//(*HeadersPCH(FilePathPanel)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/string.h>

class FilePathPanel: public wxPanel
{
	public:

		FilePathPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~FilePathPanel();

        wxString GetFilename() const { return txtFilename->GetValue(); }
        wxString GetHeaderGuard() const { return txtGuard->IsShown() ? txtGuard->GetValue() : _T(""); }
        bool GetAddToProject() const { return chkAddToProject->GetValue(); }
        void SetAddToProject(bool add);
        int GetTargetIndex();
        void SetFilePathSelectionFilter(const wxString& filter){ m_ExtFilter = filter; }
        void ShowHeaderGuard(bool show)
        {
            lblGuard->Show(show);
            txtGuard->Show(show);
        }

		//(*Identifiers(FilePathPanel)
		static const wxWindowID ID_STATICTEXT1;
		static const wxWindowID ID_STATICTEXT2;
		static const wxWindowID ID_TEXTCTRL1;
		static const wxWindowID ID_BUTTON1;
		static const wxWindowID ID_STATICTEXT3;
		static const wxWindowID ID_TEXTCTRL2;
		static const wxWindowID ID_CHECKBOX1;
		static const wxWindowID ID_STATICTEXT4;
		static const wxWindowID ID_CHECKLISTBOX2;
		static const wxWindowID ID_BUTTON2;
		static const wxWindowID ID_BUTTON3;
		//*)

	private:

		//(*Handlers(FilePathPanel)
		void OntxtFilenameText(wxCommandEvent& event);
		void OnbtnBrowseClick(wxCommandEvent& event);
		void OnchkAddToProjectChange(wxCommandEvent& event);
		void OnbtnAllClick(wxCommandEvent& event);
		void OnbtnNoneClick(wxCommandEvent& event);
		//*)

		//(*Declarations(FilePathPanel)
		wxBoxSizer* BoxSizer2;
		wxBoxSizer* BoxSizer3;
		wxBoxSizer* BoxSizer6;
		wxButton* btnAll;
		wxButton* btnBrowse;
		wxButton* btnNone;
		wxCheckBox* chkAddToProject;
		wxCheckListBox* clbTargets;
		wxFlexGridSizer* FlexGridSizer1;
		wxStaticText* lblGuard;
		wxTextCtrl* txtFilename;
		wxTextCtrl* txtGuard;
		//*)

		wxString m_ExtFilter;
		int m_Selection;

		void ToggleVisibility(bool on);

		DECLARE_EVENT_TABLE()
};

#endif

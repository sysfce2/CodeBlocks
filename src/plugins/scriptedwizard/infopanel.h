/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef INFOPANEL_H
#define INFOPANEL_H


//(*HeadersPCH(InfoPanel)
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class wxString;

class InfoPanel: public wxPanel
{
	public:

		InfoPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~InfoPanel();

		//(*Identifiers(InfoPanel)
		static const wxWindowID ID_STATICTEXT1;
		static const wxWindowID ID_CHECKBOX1;
		//*)

        void SetIntroText(const wxString& intro_msg)
        {
            lblIntro->SetLabel(intro_msg);

            GetSizer()->Fit(this);
            GetSizer()->SetSizeHints(this);
        }
//	protected:

		//(*Handlers(InfoPanel)
		//*)

		//(*Declarations(InfoPanel)
		wxBoxSizer* BoxSizer1;
		wxCheckBox* chkSkip;
		wxStaticText* lblIntro;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // INFOPANEL_H

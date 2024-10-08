/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <wx/wxprec.h>

//(*Headers(Configuration)
#include <wx/panel.h>
class wxBoxSizer;
class wxButton;
class wxListBox;
class wxStaticBoxSizer;
class wxStaticText;
class wxTextCtrl;
//*)

#include <wx/event.h>
#include <wx/string.h>

#include <configurationpanel.h>
#include "bindings.h"

class wxWindow;

class Configuration: public cbConfigurationPanel
{
public:

  Configuration(wxWindow* parent);
  virtual ~Configuration();

  //(*Identifiers(Configuration)
  static const wxWindowID ID_LST_GROUPS;
  static const wxWindowID ID_BTN_ADD_GROUP;
  static const wxWindowID ID_BTN_DELETE_GROUP;
  static const wxWindowID ID_BTN_RENAME_GROUP;
  static const wxWindowID ID_BTN_DEFAULTS;
  static const wxWindowID ID_LST_IDENTIFIERS;
  static const wxWindowID ID_BTN_ADD_IDENTIFIER;
  static const wxWindowID ID_BTN_DELETE_IDENTIFIERS;
  static const wxWindowID ID_BTN_CHANGE_IDENTIFIER;
  static const wxWindowID ID_TXT_HEADERS;
  //*)

protected:

  //(*Handlers(Configuration)
  void OnBtnAddGroupClick(wxCommandEvent& event);
  void OnBtnDeleteGroupClick(wxCommandEvent& event);
  void OnRenameGroup(wxCommandEvent& event);
  void OnGroupsSelect(wxCommandEvent& event);
  void OnBtnAddIdentifierClick(wxCommandEvent& event);
  void OnBtnDeleteIdentifierClick(wxCommandEvent& event);
  void OnChangeIdentifier(wxCommandEvent& event);
  void OnIdentifiersSelect(wxCommandEvent& event);
  void OnHeadersText(wxCommandEvent& event);
  void OnBtnDefaultsClick(wxCommandEvent& event);
  //*)

  //(*Declarations(Configuration)
  wxButton* m_AddGroup;
  wxButton* m_AddIdentifier;
  wxButton* m_ChangeIdentifier;
  wxButton* m_Defaults;
  wxButton* m_DeleteGroup;
  wxButton* m_DeleteIdentifier;
  wxButton* m_RenameGroup;
  wxListBox* m_Groups;
  wxListBox* m_Identifiers;
  wxTextCtrl* m_Headers;
  //*)

private:

  bool IdentifierOK(const wxString& Identifier);
  void ShowGroups();
  void SelectGroup(int Number);
  void SelectIdentifier(int Number);
  virtual wxString GetTitle() const
  { return _("HeaderFixup configuration"); }
  virtual wxString GetBitmapBaseName() const
  { return _T("generic-plugin"); }
  virtual void OnApply();
  virtual void OnCancel()
  { ; }

  Bindings m_Bindings;
  bool     m_BlockHeadersText;
  bool     m_Dirty;

  DECLARE_EVENT_TABLE()
};

#endif

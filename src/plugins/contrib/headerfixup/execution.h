/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EXECUTION_H
#define EXECUTION_H

#include <wx/wxprec.h>

//(*Headers(Execution)
#include "scrollingdialog.h"
class wxBoxSizer;
class wxButton;
class wxCheckBox;
class wxCheckListBox;
class wxGauge;
class wxRadioBox;
class wxStaticBoxSizer;
//*)

#include <wx/arrstr.h>
#include <wx/event.h>

#include <cbproject.h>

#include "bindings.h"
#include "fileanalysis.h"
#include "helper.h"

class wxWindow;
class wxString;

class Execution: public wxScrollingDialog
{
public:

  Execution(wxWindow* parent,wxWindowID id = -1);
  virtual ~Execution();

  //(*Identifiers(Execution)
  static const wxWindowID ID_RB_SCOPE;
  static const wxWindowID ID_RB_OPTIONS;
  static const wxWindowID ID_CHK_IGNORE;
  static const wxWindowID ID_CHK_FWD_DECL;
  static const wxWindowID ID_CHK_OBSOLETE_LOG;
  static const wxWindowID ID_RDO_FILE_TYPE;
  static const wxWindowID ID_CHK_DEBUG_LOG;
  static const wxWindowID ID_CHK_SIMULATION;
  static const wxWindowID ID_LST_SETS;
  static const wxWindowID ID_BTN_SELECT_ALL;
  static const wxWindowID ID_BTN_SELECT_NONE;
  static const wxWindowID ID_BTN_INVERT;
  static const wxWindowID ID_GAU_PROGRESS;
  static const wxWindowID ID_BTN_RUN;
  static const wxWindowID ID_BTN_EXIT;
  //*)

protected:

  //(*Handlers(Execution)
  void OnBtnExitClick(wxCommandEvent& event);
  void OnBtnSelectAllClick(wxCommandEvent& event);
  void OnBtnSelectNoneClick(wxCommandEvent& event);
  void OnBtnInvertClick(wxCommandEvent& event);
  void OnBtnRunClick(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnChkSimulationClick(wxCommandEvent& event);
  //*)

  //(*Declarations(Execution)
  wxBoxSizer* sizRunExit;
  wxButton* m_Exit;
  wxButton* m_Invert;
  wxButton* m_Run;
  wxButton* m_SelectAll;
  wxButton* m_SelectNone;
  wxCheckBox* m_FwdDecl;
  wxCheckBox* m_Ignore;
  wxCheckBox* m_ObsoleteLog;
  wxCheckBox* m_Protocol;
  wxCheckBox* m_Simulation;
  wxCheckListBox* m_Sets;
  wxGauge* m_Progress;
  wxRadioBox* m_FileType;
  wxRadioBox* m_Options;
  wxRadioBox* m_Scope;
  wxStaticBoxSizer* sizExecute;
  //*)

private:

  void LoadSettings();
  void SaveSettings();

  void ToggleControls(bool DoEnable);
  int RunScan(const wxArrayString& FilesToProcess, const wxArrayString& Groups);

  void AddFilesFromProject(wxArrayString& Files, cbProject* Project);
  int ProcessFile(const wxString& GlobalFileName,const wxArrayString& Groups);
  void OperateToken(const wxString&      Token,
                    const wxArrayString& Groups,
                    const wxArrayString& IncludedHeaders,
                    const wxArrayString& ExistingFwdDecls,
                    const wxChar&        Ch,
                    const wxString&      Line,
                    wxArrayString&       RequiredHeaders,
                    wxArrayString&       RequiredFwdDecls);

  DECLARE_EVENT_TABLE()

  enum EProcessor // NOTE (Morten#5#): Ensure this matches wxRadioBox* m_FileType;
  {
    ProcessHeaderFiles,
    ProcessSourceFiles
  };

  wxArrayString m_Log;
  wxArrayString m_TokensProcessed;
  Bindings      m_Bindings;
  FileAnalysis  m_FileAnalysis;
  bool          m_Execute;
  EProcessor    m_Processor;
};

#endif

// ----------------------------------------------------------------------------
//  JumpTracker.cpp
// ----------------------------------------------------------------------------
#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include <cbstyledtextctrl.h>
#include <logmanager.h> // (ph 25/06/24)
#include <projectmanager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include <wx/xrc/xmlres.h>

#include "JumpTrackerView.h"
//#include "Version.h"
#include "JumpTracker.h"
#include "JumpData.h"

// now that we have ArrayOfJumpData declaration in scope we may finish the
// definition -- note that this expands into some C++
// code and so should only be compiled once (i.e., don't put this in the
// header, but into a source file or you will get linking errors)
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ArrayOfJumpData);

// ----------------------------------------------------------------------------
//  namespace
// ----------------------------------------------------------------------------
// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    //-PluginRegistrant<JumpTracker> reg(_T("JumpTracker"));
    int idMenuJump     = wxNewId();
    int idMenuJumpBack = wxNewId();
    int idMenuJumpNext = wxNewId();
    int idMenuJumpClear = wxNewId();
    int idMenuJumpDump = wxNewId();
    int idMenuJumpView = wxNewId();

    int idToolJumpPrev = XRCID("idJumpPrev");
    int idToolJumpNext = XRCID("idJumpNext");
}

// ----------------------------------------------------------------------------
//  Events Table
// ----------------------------------------------------------------------------
// events handling
BEGIN_EVENT_TABLE(JumpTracker, cbPlugin)
    // add any events you want to handle here
//    Now doing Connect() in BuildMenu()
//    EVT_MENU( idMenuJumpBack,  JumpTracker::OnMenuJumpBack)
//    EVT_MENU( idMenuJumpNext,  JumpTracker::OnMenuJumpNext)
//    EVT_MENU( idMenuJumpClear, JumpTracker::OnMenuJumpClear)
//    EVT_MENU( idMenuJumpDump,  JumpTracker::OnMenuJumpDump)

END_EVENT_TABLE()

// constructor
// ----------------------------------------------------------------------------
JumpTracker::JumpTracker()
// ----------------------------------------------------------------------------
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
//    if(!Manager::LoadResource(_T("JumpTracker.zip")))
//    {
//        NotifyMissingFile(_T("JumpTracker.zip"));
//    }

    m_bShuttingDown = false;
    m_FilenameLast = wxEmptyString;
    m_PosnLast = 0;
    m_ArrayCursor = 0;
    m_ArrayOfJumpData.Clear();
    m_bProjectClosing = false;
    m_IsAttached = false;
    m_bJumpInProgress = false;
    m_bWrapJumpEntries = false;
    m_pToolBar = nullptr;
}
// ----------------------------------------------------------------------------
JumpTracker::~JumpTracker()
// ----------------------------------------------------------------------------
{
    // destructor
}
// ----------------------------------------------------------------------------
void JumpTracker::OnAttach()
// ----------------------------------------------------------------------------
{
    // do whatever initialization you need for your plugin
    // :NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

    // --------------------------------------------------
    // Initialize a debugging log/version control **Debugging**
    // --------------------------------------------------
    //    PlgnVersion plgnVersion;
    //    #if LOGGING
    //     wxLog::EnableLogging(true);
    //     m_pPlgnLog = new wxLogWindow( Manager::Get()->GetAppWindow(), _T(" JumpTracker"),true,false);
    //     wxLog::SetActiveTarget( m_pPlgnLog);
    //     m_pPlgnLog->GetFrame()->SetSize(20,30,600,300);
    //     LOGIT( _T("JT JumpTracker Logging Started[%s]"),plgnVersion.GetVersion().c_str());
    //     m_pPlgnLog->Flush();
    //    #endif

    //    // Set current plugin version
    //    PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
    //    pInfo->version = plgnVersion.GetVersion();

    m_bJumpInProgress = false;
    ConfigManager* pCfgMgr = Manager::Get()->GetConfigManager("BrowseTracker");
	m_BrowseMarksEnabled =  pCfgMgr->ReadBool( "BrowseMarksEnabled", false ) ;

    CreateJumpTrackerView();

    wxWindow* appWin = Manager::Get()->GetAppWindow();
    appWin->PushEventHandler(this);
    appWin->Connect(idMenuJumpBack, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Connect(idMenuJumpNext, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Connect(idMenuJumpClear, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpClear), 0, this);
    appWin->Connect(idMenuJumpDump, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpDump), 0, this);

    appWin->Connect(idToolJumpPrev, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Connect(idToolJumpNext, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Connect(idToolJumpPrev, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);
    appWin->Connect(idToolJumpNext, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);


    // JumpTracker Codeblocks Events registration
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_UPDATE_UI   , new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorUpdateUIEvent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorActivated));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_DEACTIVATED, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorDeactivated));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnEditorClosed));

    Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnStartShutdown));
    // -- Project events
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnProjectActivatedEvent));
    //-Manager::Get()->RegisterEventSink(cbEVT_PROJECT_OPEN, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnProjectOpened));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnProjectClosing));
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<JumpTracker, CodeBlocksEvent>(this, &JumpTracker::OnAppStartupDone));

    // --------------------------------------------------------
	// register event syncs for JumpTrackerView window docking
    // --------------------------------------------------------
	// request app to switch view layout - BUG: this is not being issued on View/Layout change
    Manager::Get()->RegisterEventSink(cbEVT_SWITCH_VIEW_LAYOUT, new cbEventFunctor<JumpTracker, CodeBlocksLayoutEvent>(this, &JumpTracker::OnSwitchViewLayout));
    // app notifies that a new layout has been applied
    Manager::Get()->RegisterEventSink(cbEVT_SWITCHED_VIEW_LAYOUT, new cbEventFunctor<JumpTracker, CodeBlocksLayoutEvent>(this, &JumpTracker::OnSwitchedViewLayout));
    // app notifies that a docked window has been hidden/shown
    Manager::Get()->RegisterEventSink(cbEVT_DOCK_WINDOW_VISIBILITY, new cbEventFunctor<JumpTracker, CodeBlocksDockEvent>(this, &JumpTracker::OnDockWindowVisability));
    // Connect to MainMenu/View/JumpTracker checkbox item
    appWin->Connect(idMenuJumpView, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnViewJumpTrackerWindow), 0, this);
}
// ----------------------------------------------------------------------------
void JumpTracker::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...

    wxWindow* appWin = Manager::Get()->GetAppWindow();

    //If appShutdown leave the event handler, else wxWidgets asserts on linux
    if (not appShutDown)
        appWin->RemoveEventHandler(this); //2017/11/23 stop uninstall crash 2017/12/6 crashes linux

    // Free JumpData memory
    wxCommandEvent evt;
    OnMenuJumpClear(evt);

    appWin->Disconnect(idMenuJumpBack, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Disconnect(idMenuJumpNext, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Disconnect(idMenuJumpClear, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpClear), 0, this);
    appWin->Disconnect(idMenuJumpDump, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnMenuJumpDump), 0, this);

    appWin->Disconnect(idToolJumpPrev, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpBack), 0, this);
    appWin->Disconnect(idToolJumpNext, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(JumpTracker::OnMenuJumpNext), 0, this);
    appWin->Disconnect(idToolJumpPrev, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);
    appWin->Disconnect(idToolJumpNext, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(JumpTracker::OnUpdateUI), 0, this);
    // Disconnect from MainMenu/View/JumpTracker checkbox item
    appWin->Disconnect(idMenuJumpView, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(JumpTracker::OnViewJumpTrackerWindow), 0, this);

    Manager::Get()->    Manager::Get()->RemoveAllEventSinksFor(this); //2017/11/23
}
// ----------------------------------------------------------------------------
void JumpTracker::BuildMenu(wxMenuBar* menuBar)
// ----------------------------------------------------------------------------
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    //-NotImplemented(_T("JumpTracker::BuildMenu()"));

     // insert menu items
    wxMenu* jump_submenu = new wxMenu;
    jump_submenu->Append(idMenuJumpBack,  _("Jump Back"),   _("Jump back to previous ed position"));
    jump_submenu->Append(idMenuJumpNext,  _("Jump Frwd"),   _("Jump to next ed position"));
    jump_submenu->Append(idMenuJumpClear, _("Jump Clear"),  _("Jump Clear History"));
    #if defined(LOGGING)
    jump_submenu->Append(idMenuJumpDump,  _("Jump Dump"),   _("Jump Dump Array"));
    #endif

    int viewPos = menuBar->FindMenu(_("&View"));
    if ( viewPos == wxNOT_FOUND )
        return;
    // Place the menu inside the View Menu
    wxMenu* pViewMenu = menuBar->GetMenu(viewPos);
    pViewMenu->Append(idMenuJump, _("Jump"), jump_submenu, _("Jump"));

    // ----------------------------------------------------------------------------
    // Add JumpTrackerView to the View menu
    // ----------------------------------------------------------------------------
    bool m_appIsShutdown = Manager::Get()->IsAppShuttingDown();
    if (m_appIsShutdown) return;

    bool isSet = false;

	int idx = menuBar->FindMenu(_("&View"));
	if (idx != wxNOT_FOUND)
	{
		wxMenu* viewMenu = menuBar->GetMenu(idx);
		wxMenuItemList& items = viewMenu->GetMenuItems();
		// Find the first separator and insert before it
		for (size_t i = 0; i < items.GetCount(); ++i)
		{
			if (items[i]->IsSeparator())
			{
				viewMenu->InsertCheckItem(i, idMenuJumpView, _("JumpTracker"), _("Toggle displaying the JumpTracker list."));
				isSet = true;
				break;
			}
		}//for
		// Not found, just append
		if (not isSet)
            viewMenu->AppendCheckItem(idMenuJumpView, _("JumpTracker"), _("Toggle displaying the JumpTracker view."));
	}

}
// ----------------------------------------------------------------------------
void JumpTracker::BuildModuleMenu(const ModuleType /*type*/, wxMenu* /*menu*/, const FileTreeData* /*data*/)
// ----------------------------------------------------------------------------
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    //-NotImplemented(_T("JumpTracker::BuildModuleMenu()"));
}
// ----------------------------------------------------------------------------
bool JumpTracker::BuildToolBar(wxToolBar* toolBar)
// ----------------------------------------------------------------------------
{
    m_pToolBar = toolBar;

    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    //-NotImplemented(_T("JumpTracker::BuildToolBar()"));

    // return true if you add toolbar items
    return false;
}
// ----------------------------------------------------------------------------
void JumpTracker::CreateJumpTrackerView()
// ----------------------------------------------------------------------------
{
    // ---------------------------------------
    // setup JumpTracker list docking window
    // ---------------------------------------
    wxArrayInt widths;
    wxArrayString titles;
    titles.Add(_("File"));
    titles.Add(_("Line"));
    titles.Add(_("Text"));
    widths.Add(128);
    widths.Add(48);
    widths.Add(640);

    m_pJumpTrackerView.reset( new JumpTrackerView(titles, widths));

    // Ask DragScroll plugin to apply its support for this log
    //wxWindow* pWindow = GetJumpTrackerView()->m_pControl;
    //cbPlugin* pPlgn = Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll"));
    //if (pWindow && pPlgn)
    //{
    //    /// *bug* Dragscroll plugin may not be initialized yet; code moved to OnStartupDone()
    //    wxCommandEvent dsEvt(wxEVT_COMMAND_MENU_SELECTED, XRCID("idDragScrollAddWindow"));
    //    dsEvt.SetEventObject(pWindow);
    //    pPlgn->ProcessEvent(dsEvt);
    //}

    // Floating windows must be set by their parent
	CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
	evt.name = _T("JumpTrackerPane");
	evt.title = _("JumpTracker View");
	evt.pWindow = GetJumpTrackerView()->m_pListCtrl;
    evt.desiredSize.Set(300, 300);
    evt.floatingSize.Set(300, 400);
    evt.minimumSize.Set(30, 40);
	// assume floating window
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.dockSide = CodeBlocksDockEvent::dsLeft;
    evt.stretch = true;

	Manager::Get()->ProcessEvent(evt);

}//CreateJumpTrackerView
// ----------------------------------------------------------------------------
void JumpTracker::OnViewJumpTrackerWindow(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    wxUnusedVar(event);

    // This routine invoked from MainMenu/View/JumpTracker checkbox
    // For some unknow reason, event.IsCheck() gives an incorrect state
    // So we'll find the menu check item ourselves
    wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    wxMenu* pViewMenu = 0;
    wxMenuItem* pViewItem = pbar->FindItem(idMenuJumpView, &pViewMenu);
    #if defined(LOGGING)
    LOGIT( _T("OnViewJumpTracker [%s] Checked[%d] IsShown[%d]"),
            GetConfig()->IsFloatingWindow()?_T("float"):_T("dock"),
            pViewMenu->IsChecked(idViewJumpTracker),
            IsWindowReallyShown(JumpTrackerWindow())
            );
    #endif

        // ---------------------------------------
        // setup JumpTracker view docking window
        // ---------------------------------------
     if (not GetJumpTrackerViewControl())
    {
        // JumpTracker view Window is closed, initialize and open it.
        CreateJumpTrackerView();
    }


    #if defined(LOGGING)
    LOGIT( _T("OnView [%s] Checked[%d] IsShown[%d]"),
            GetConfig()->IsFloatingWindow()?_T("float"):_T("dock"),
            pViewItem->IsChecked(),
            IsWindowReallyShown(GetJumpTrackerWindow())
            );
    #endif

    // hiding window. remember last window position
    if ( IsWindowReallyShown(GetJumpTrackerViewControl()) )
    {   if (not pViewItem->IsChecked()) //hiding window
        {   //wxAUI is so annoying, we have to check that it's *not* giving us
            // the position and size of CodeBlocks.
            if ( GetConfigBool("IsFloatingWindow") )
                SettingsSaveWinPosition();
            #if defined(LOGGING)
            LOGIT( _T("OnViewJumpTracker saving settings on HideWindow"));
            #endif
        }
    }

	CodeBlocksDockEvent evt(pViewItem->IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
	evt.pWindow = GetJumpTrackerViewControl();
	Manager::Get()->ProcessEvent(evt);

}//OnViewJumpTracker
// ----------------------------------------------------------------------------
void JumpTracker::OnSwitchViewLayout(CodeBlocksLayoutEvent& event)
// ----------------------------------------------------------------------------
{
    //event.layout
    #if defined(LOGGING)
    LOGIT( _T("cbEVT_SWITCH_LAYOUT[%s]"),event.layout.c_str() );
    #endif
    event.Skip();
}
// ----------------------------------------------------------------------------
void JumpTracker::OnSwitchedViewLayout(CodeBlocksLayoutEvent& event)
// ----------------------------------------------------------------------------
{
    //event.layout
    #if defined(LOGGING)
    LOGIT( _T("cbEVT_SWITCHED_LAYOUT[%s]"),event.layout.c_str() );
    #endif
    event.Skip();
}
// ----------------------------------------------------------------------------
void JumpTracker::OnDockWindowVisability(CodeBlocksDockEvent& event)
// ----------------------------------------------------------------------------
{
    // Called when user issues the cbEVT_SHOW_DOCK_WINDOW/cbEVT_HIDE_DOCK_WINDOW)
    // But not called when user uses system [x] close on docked/float window

    //event.layout
    //BUG: the event.GetId() is always null. It should be the window pointer.
    #if defined(LOGGING)
    LOGIT( _T("cbEVT_DOCK_WINDOW_VISIBILITY[%p]"),event.GetId() );
    #endif
    wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    if (not IsWindowReallyShown(GetJumpTrackerViewControl()))
        pbar->Check(idMenuJumpView, false);

    event.Skip();
}
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorActivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    cbEditor* pEd =  Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(not pEd)
        return;
    cbStyledTextCtrl* pControl = pEd->GetControl();
    if(pControl->GetCurrentLine() == wxSCI_INVALID_POSITION)
        return;

    // Add any new editor to vector of editors bound to mouse events
    bool edFound = std::find(m_MouseBoundEditors.begin(),
                        m_MouseBoundEditors.end(),
                            pEd) != m_MouseBoundEditors.end();
    if (not edFound) {
        m_MouseBoundEditors.push_back(pEd);
        pControl->Bind(wxEVT_LEFT_DOWN, &JumpTracker::OnLeftDown, this);
        pControl->Bind(wxEVT_MOTION, &JumpTracker::OnMouseMove, this);
        pControl->Bind(wxEVT_LEFT_UP, &JumpTracker::OnLeftUp, this);
    }
}
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorUpdateUIEvent(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // Called when user clicks on an editor line or moves to another line
    // Also called when only editor is activated

    if ( m_bShuttingDown )
        return;
    if (GetJumpInProgress())
        return;

    if ( (not m_leftDown) or m_isDragging)
        return;

    cbEditor* pEd =  Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(not pEd)
        return;

    wxString edFilename = pEd->GetFilename();
    cbStyledTextCtrl* pControl = pEd->GetControl();
    if(pControl->GetCurrentLine() == wxSCI_INVALID_POSITION)
        return;

    long edLine = pControl->GetCurrentLine();
    long edPosn = pControl->GetCurrentPos();

    #if defined(LOGGING)
    //LOGIT( _T("JT OnEditorUpdateEvent Filename[%s] line[%ld] pos[%ld] "), edFilename.c_str(), edLine, edPosn);
    //LOGIT( _T("JT \ttopLine[%ld] botLine[%ld] OnScrn[%ld] "), topLine, botLine, edstc->LinesOnScreen());
    #endif

    // Newly activated editor ?
    if (m_FilenameLast != edFilename)
    {
        m_PosnLast = edPosn;
        m_FilenameLast = edFilename;
        JumpDataAdd(edFilename, edPosn, edLine);
            return;
    }

    // Not a new editor activation
    // if user has not moved line cursor, ignore this update (eg. click on part of same line)
    if (pControl->LineFromPosition(m_PosnLast) == pControl->LineFromPosition(edPosn))
        return;

    // record new posn
    m_PosnLast = edPosn;
    m_FilenameLast = edFilename;
    if (m_leftDown and (not m_isDragging)) // (ph 25/04/25)
        JumpDataAdd(edFilename, edPosn, edLine);

    return;
}//OnEditorUpdateEvent
// ----------------------------------------------------------------------------
void JumpTracker::OnLeftDown(wxMouseEvent& event) {
// ----------------------------------------------------------------------------
    m_leftDown = true;
    event.Skip();  // Allow default processing (e.g., focus changes)
}

// ----------------------------------------------------------------------------
void JumpTracker::OnMouseMove(wxMouseEvent& event) {
    // ----------------------------------------------------------------------------
    if (m_leftDown && event.Dragging()) {
        m_isDragging = true;
    }
    event.Skip();
}
// ----------------------------------------------------------------------------
void JumpTracker::OnLeftUp(wxMouseEvent& event) {
    // ----------------------------------------------------------------------------
    if (m_leftDown && (not m_isDragging)) {
        // Pure click (no drag) occurred, nothing to do
        // clear left-Down and m_isDragging anyway. // (blauzahn 25/06/22)
        ;
    }
    m_leftDown = m_isDragging = false;
    event.Skip();
}
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorDeactivated(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();
    //Add the deactivated editor location
    // add the deactivated plugin position so we can jump back

    if (m_bShuttingDown) return;
    if (not IsAttached()) return;

    // Don't record closing editor activations
    if (m_bProjectClosing)
        return;

    EditorBase* pEdBase = event.GetEditor();
    wxString edFilename = pEdBase->GetFilename();
    cbEditor* pcbEd = Manager::Get()->GetEditorManager()->GetBuiltinEditor(pEdBase);
    if (not pcbEd) return;

    cbStyledTextCtrl* pControl = pcbEd->GetControl();
    if (not pControl) return;
    if(pControl->GetCurrentLine() == wxSCI_INVALID_POSITION)
        return;

    long edPosn = pControl->GetCurrentPos();
    JumpDataAdd(edFilename, edPosn, pControl->GetCurrentLine());

    return;

}//OnEditorDeactivated
// ----------------------------------------------------------------------------
void JumpTracker::OnEditorClosed(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    // clear this editor out of our arrays and pointers

    //_NOTE: using Manager::Get->GetEditorManager()->GetEditor... etc will
    //      fail in this codeblocks event.
    //      The cbEditors are nolonger available

    event.Skip();

    if (not IsAttached())
        return;

    wxString filePath = event.GetString();

    EditorBase* pEdBase = event.GetEditor();
    #if defined(LOGGING)
        LOGIT( _T("JT OnEditorClosed Eb[%p][%s]"), eb, eb->GetShortName().c_str() );
    #endif


    if (m_ArrayOfJumpData.GetCount())
        for (int ii=m_ArrayOfJumpData.GetCount()-1; ii > -1; --ii)
        {
            // remove the entry
            if (m_ArrayOfJumpData[ii].GetFilename() == filePath)
                m_ArrayOfJumpData.RemoveAt(ii);
            // reset m_ArrayCursor to previous entry
            if (((int)m_ArrayOfJumpData.GetCount()-1) < m_ArrayCursor)
                m_ArrayCursor = GetPreviousIndex(m_ArrayCursor);
        }

    SetJumpTrackerViewIndex(m_ArrayCursor < 0 ? 0 : m_ArrayCursor);

    // Remove editor in vector of editors bound to mouse events
    cbEditor* pCBEd = Manager::Get()->GetEditorManager()->GetBuiltinEditor(pEdBase);
    if (pCBEd)
    {
        // Find the iterator position of pEd
        //std::vector<cbEditor*>::iterator
        auto it = std::find(m_MouseBoundEditors.begin(),
                            m_MouseBoundEditors.end(),
                            pCBEd);  // Ensure type safety
        // The cbStyledTextCtrl always is nullptr
        cbStyledTextCtrl* pControl = pCBEd->GetControl();
        if ( it != m_MouseBoundEditors.end())
        {
            // Erase using the iterator
            m_MouseBoundEditors.erase(it);
            if (pControl)   // always nullptr
            {
                // Clean up event bindings
                pControl->Unbind(wxEVT_LEFT_DOWN, &JumpTracker::OnLeftDown, this);
                pControl->Unbind(wxEVT_MOTION,    &JumpTracker::OnMouseMove, this);
                pControl->Unbind(wxEVT_LEFT_UP,   &JumpTracker::OnLeftUp, this);
            }
        }
    }

    #if defined(LOGGING)
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif

    UpdateJumpTrackerViewWindow();

}//OnEditorClosed

// ----------------------------------------------------------------------------
void JumpTracker::OnStartShutdown(CodeBlocksEvent& /*event*/)
// ----------------------------------------------------------------------------
{
    m_bShuttingDown = true;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnProjectClosing(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();

    // This event occurs before the editors are closed. That allows
    // us to reference CB project and editor related data before CB
    // deletes it all.

    if (m_bShuttingDown) return;
    if (not IsAttached()) return;

    m_bProjectClosing = true;
    SetJumpTrackerViewIndex(0);

    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnProjectActivatedEvent(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip();

    // NB: EVT_PROJECT_ACTIVATE is occuring before EVT_PROJECT_OPEN
    // NB: EVT_EDITOR_ACTIVATE event occurs before EVT_PROJECT_ACTIVATE or EVT_PROJECT_OPEN

    event.Skip();
    if (m_bShuttingDown) return;
    if (not IsAttached()) return;

    // Previous project was closing
    if (m_bProjectClosing)
        m_bProjectClosing = false;

}//OnProjectActivatedEvent
// ----------------------------------------------------------------------------
void JumpTracker::OnProjectOpened(CodeBlocksEvent& event)   // (ph 25/05/07)
// ----------------------------------------------------------------------------
{
    // NB: EVT_PROJECT_ACTIVATE is occuring before EVT_PROJECT_OPEN
    // NB: EVT_EDITOR_ACTIVATE events occur before EVT_PROJECT_ACTIVATE or EVT_PROJECT_OPEN
    // Currently, this event is a hack to us since it occurs AFTER editors are activated
    //  and AFTER the project is activated

    // But since the editors are now already open, we can read the layout file
    // that saved previous BrowseMark and book mark history, and use that data
    // to build/set old saved Browse/Book marks.

    if ( not m_BrowseMarksEnabled)
        return;

    cbProject* pProject = event.GetProject();

    if ( not pProject )
    {   //caused when project is imported
        return;
    }
    ProjectManager* pPrjMgr = Manager::Get()->GetProjectManager();

    #if defined(LOGGING)
     LOGIT( _T("JT -----------------------------------"));
     LOGIT( _T("JT Project OPENED[%s]"), event.GetProject()->GetFilename().c_str() );
    #endif

    // Record an initial jump tracker entry for the active project
    // after all files have been opened when loading a project.
    if (pPrjMgr->IsLoadingProject()  )
    {
        cbEditor* pEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        cbStyledTextCtrl* pControl = pEd ? pEd->GetControl() : nullptr;
        if (pControl)
        {
            //-EditorBase* pEdBase = event.GetEditor();
            wxString edFilename = pEd->GetFilename();
            //-cbEditor* pcbEd = Manager::Get()->GetEditorManager()->GetBuiltinEditor(pEdBase);
            //-cbStyledTextCtrl* pControl = pcbEd->GetControl();
            //-if (not pControl) return;
            if(pControl->GetCurrentLine() == wxSCI_INVALID_POSITION)
                return;
            long edPosn = pControl->GetCurrentPos();
            JumpDataAdd(edFilename, edPosn, pControl->GetCurrentLine());
        }
    }
//    else
//    {
//        // the project is still loading
//        // Record the last CB activated editor as if the user activate it.
//        // else the initial jumpmark won't be recorded.
//        // We have to use a CallAfter() so that IsProjectLoading() will clear.
//
//        cbEditor* pEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
//        if (not pEd) return;
//        auto pEvt = std::make_shared<CodeBlocksEvent>(cbEVT_PROJECT_OPEN);
//        pEvt->SetEditor(pEd);
//        pEvt->SetPlugin(this);
//        pEvt->SetProject(pProject);
//        CallAfter([this, pEvt]() { OnProjectOpened(*pEvt);});
//        #if defined(LOGGING)
//        LOGIT( _T("JT OnProjectOpened [%p][%s]"), pActiveEd, pActiveEd->GetShortName().c_str() );
//        #endif
//    }
//

}//OnProjectOpened

// ----------------------------------------------------------------------------
void JumpTracker::OnUpdateUI(wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    // sync the menu check item with the real world
    wxMenuBar* pbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    bool isShown = IsWindowReallyShown(GetJumpTrackerViewControl());
    pbar->Check(idMenuJumpView, isShown);

    int count = m_ArrayOfJumpData.GetCount();
    //-m_pToolBar->EnableTool(idToolJumpNext, count > 0 /*&& m_ArrayCursor != m_insertNext - 1*/);
    //-m_pToolBar->EnableTool(idToolJumpPrev, count > 0 /*&& m_ArrayCursor != m_insertNext*/);

    // If not  wrapping && we're about to advance pass end of array, diable
    bool enableNext = (count > 0);
    if (not m_bWrapJumpEntries)
        if (m_ArrayCursor+1 >= int(m_ArrayOfJumpData.GetCount()))
            enableNext = false;

    bool enablePrev = count > 0;
    // If not wrapping && we're about to backup into -1 position, disable
    if (not m_bWrapJumpEntries)
        if (m_ArrayCursor-1 < 0) // (ph 25/05/09)
            enablePrev = false;

    m_pToolBar->EnableTool( idToolJumpNext, enableNext);
    m_pToolBar->EnableTool( idToolJumpPrev, enablePrev);

    event.Skip();
}
// ----------------------------------------------------------------------------
void JumpTracker::JumpDataAdd(const wxString& inFilename, const long inPosn, const long inLineNum)
// ----------------------------------------------------------------------------
{
    // note:
    // the 'currentviewindex' has the indes of the JumpTrackerView window.
    // The 'ArrayOfJumpData' index has the next available array entry slot.

    // Do not record old jump locations when a jump is in progress.
    // Caused by activating an editor inside the jump routines.
    if (GetJumpInProgress())
        return;

    // when debugging the mouse status can get stuck, so clear it.
    if (m_leftDown and (not m_isDragging)) // (ph 25/04/26)
        m_leftDown = false;
    // Dont record position if line number is < 1 since a newly loaded
    // file always reports an event for line 0
     if (inLineNum < 1)       // user requested feature 2010/06/1
        return;

    cbEditor* pEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (not pEd) return;
    cbStyledTextCtrl* pControl = pEd->GetControl();
    if (not pControl) return;
    //jdLineNo = pEd->GetControl()->LineFromPosition(jdPosn);

    // if current entry is identical to the currently double-clicked line
    // in the JumTrackerView window, do not enter the line in the array
    if (m_ArrayOfJumpData.GetCount())
    {
        int currentViewIndex = GetJumpTrackerViewIndex();
        JumpData jumpData = m_ArrayOfJumpData[currentViewIndex];
        wxString jdFilename = jumpData.GetFilename();
        int jdPosn = jumpData.GetPosition();
        if (inFilename == jdFilename)
        {
            int jdLineNo = 0;
            cbEditor* pEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if (not pEd) return;
            cbStyledTextCtrl* pControl = pEd->GetControl();
            if (not pControl) return;
            jdLineNo = pEd->GetControl()->LineFromPosition(jdPosn);
            if (jdLineNo == inLineNum)
                return;
        }
    }
    // If the input entry is same as last entry in array, skip it
    if (m_ArrayOfJumpData.GetCount())
    {
        int endIndex = m_ArrayOfJumpData.GetCount()-1;
        JumpData jumpData = m_ArrayOfJumpData[endIndex];
        wxString jdFilename = jumpData.GetFilename();
        if (inFilename == jdFilename)
        {
            if (jumpData.GetLineNo() == inLineNum)
                return;
        }
    }

    // if the new line is half page greater or less than the current viewed line,
    // replace it with the new line. Avoids many lines at the save location.
    int arrayIdx = m_ArrayOfJumpData.GetCount()-1;
    if (arrayIdx >= 0)
    {
        // the current view index has the view the user sees
        // The m_Array of JumpData has the next available entry slot.
        int currentViewIndex = GetJumpTrackerViewIndex();
        JumpData jumpDataOld = m_ArrayOfJumpData[currentViewIndex];
        int jdOldLineNo = jumpDataOld.GetLineNo();
        int halfPage = pControl ? pControl->LinesOnScreen()>>1 : 10;

        if (abs(jdOldLineNo - inLineNum) <= halfPage)
        {
            // set old entry with  new data
            JumpData* pJumpData = &m_ArrayOfJumpData[currentViewIndex];
            //-m_ArrayOfJumpData.RemoveAt(arrayCursor);
            //-m_ArrayOfJumpData.Add(new JumpData(inFilename, inPosn, inLineNum));
            //-m_ArrayCursor = m_ArrayOfJumpData.GetCount()-1;
            pJumpData->SetFilename(inFilename);
            pJumpData->SetLineNo(inLineNum);
            pJumpData->SetPosition(inPosn);
            SetJumpTrackerViewIndex(m_ArrayCursor);
            UpdateJumpTrackerViewWindow();
            return;
        }
    }

    // make room for a new jump
    int kount = m_ArrayOfJumpData.GetCount();
    if (kount  >= maxJumpEntries )
    {
        // Remove the top entry;
        m_ArrayOfJumpData.RemoveAt(0);
        if (m_ArrayCursor) --m_ArrayCursor;
        if (m_ArrayCursor < 0 )
            m_ArrayCursor = m_ArrayOfJumpData.GetCount()-1;
        SetJumpTrackerViewIndex(m_ArrayCursor); // (ph 25/04/26)
    }

    #if defined(LOGGING)
    LOGIT( _T("JT JumpDataAdd[%s][%ld][%d]"), filename.c_str(), posn, m_insertNext);
    #endif

////    if ( kount == maxJumpEntries ) // (ph 25/04/27)
////    {   //remove oldest item in the array
////        m_ArrayOfJumpData.RemoveAt(0);
////        if (m_ArrayCursor) --m_ArrayCursor;
////        if (m_ArrayCursor < 0 )
////            m_ArrayCursor = m_ArrayOfJumpData.GetCount()-1;
////        SetJumpTrackerViewIndex(m_ArrayCursor); // (ph 25/04/26)
////    }

   // initialize new item
    m_ArrayOfJumpData.Add(new JumpData(inFilename, inPosn,inLineNum));
    m_ArrayCursor = m_ArrayOfJumpData.GetCount()-1;

    //#if defined(LOGGING)
    //    // debugging: Dump the array to log
    //    wxCommandEvent evt;
    //    OnMenuJumpDump(evt);
    //#endif

    UpdateJumpTrackerViewWindow();
    SetJumpTrackerViewIndex(m_ArrayCursor);     //(ph 2025-05-11 )

    GetJumpTrackerView()->FocusEntry(m_ArrayCursor);

    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpBack(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("OnMenuJumpBack"));
    #endif

    int knt = m_ArrayOfJumpData.GetCount();
    if (0 == knt)
        return;

    int lastViewedIndex = GetJumpTrackerViewIndex();
    // If not wrapping && we're about to backup into the insert index, return
    if (not m_bWrapJumpEntries)
        //if (not (lastViewedIndex >= 0)) // (ph 25/05/07)
        if (lastViewedIndex <= 0)// (ph 25/05/07)
            return;

    EditorManager* pEdMgr = Manager::Get()->GetEditorManager();
    EditorBase* pEdBase = pEdMgr->GetActiveEditor();
    cbEditor* pcbEd = (pEdBase ? pEdMgr->GetBuiltinEditor(pEdBase) : nullptr);

    if (not pcbEd)
        return;

    m_bJumpInProgress = true;

    wxString activeEdFilename = wxEmptyString;
    if (pcbEd) switch(1)
    {
        default:
        int idx = lastViewedIndex;
        idx = GetPreviousIndex(idx);
        if ( idx == wxNOT_FOUND)
            break;
        JumpData& jumpdata = m_ArrayOfJumpData[idx];
        if (not pEdMgr->IsOpen(jumpdata.GetFilename()))
            break;
        m_ArrayCursor = idx;

        JumpData& jumpData = m_ArrayOfJumpData[m_ArrayCursor];
        wxString edFilename = jumpData.GetFilename();
        long edPosn = jumpData.GetPosition();

        #if defined(LOGGING)
        LOGIT( _T("JT OnMenuJumpBack [%s][%ld]curs[%d]"), edFilename.wx_str(), edPosn, m_ArrayCursor);
        #endif

        // activate editor
        pEdBase = pEdMgr->GetEditor(edFilename);
        if (not pEdBase) break;

        // avoid re-recording this jump location
        SetJumpTrackerViewIndex(m_ArrayCursor);
        // if editors will be switched activate the new editor
        if ( pEdBase != pEdMgr->GetActiveEditor())
            pEdMgr->SetActiveEditor(pEdBase); // cause a cbEVT_EditorActivated event
        // position to editor line
        pcbEd = pEdMgr->GetBuiltinEditor(pEdBase);
        if (not pcbEd) break;

        pcbEd->GotoLine(pcbEd->GetControl()->LineFromPosition(edPosn)); //center on scrn
        pcbEd->GetControl()->GotoPos(edPosn);

        GetJumpTrackerView()->FocusEntry(GetJumpTrackerViewIndex());
    }

    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("END OnMenuJumpBack"));
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif

    m_bJumpInProgress = false;
    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpNext(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("OnMenuJumpNext"));
    #endif

    int knt = 0;
    if ((knt = m_ArrayOfJumpData.GetCount()) == 0)
        return;

    // If not wrapping and we're about to advance into the insert index, return
    if ( (not m_bWrapJumpEntries) and
        (GetJumpTrackerViewIndex() == knt-1) )
            return;

    EditorManager* pEdMgr = Manager::Get()->GetEditorManager();
    EditorBase* pEdBase = pEdMgr->GetActiveEditor();
    cbEditor* pcbEd = (pEdBase ? pEdMgr->GetBuiltinEditor(pEdBase) : nullptr);
    if (not pcbEd)
        return;

    m_bJumpInProgress = true;

    switch(1)
    {
        // find the next appropriate jump position
        default:
        int idx = GetJumpTrackerViewIndex();
        idx = GetNextIndex(idx);
        if ( idx == wxNOT_FOUND)
            break;
        JumpData& jumpdata = m_ArrayOfJumpData[idx];
        if (not pEdMgr->IsOpen(jumpdata.GetFilename()))
            break;
        m_ArrayCursor = idx;
        wxString edFilename = jumpdata.GetFilename();
        long edPosn = jumpdata.GetPosition();
        #if defined(LOGGING)
        LOGIT( _T("JT OnMenuJumpBack [%s][%ld]curs[%d]"), edFilename.wx_str(), edPosn, m_ArrayCursor);
        #endif

        // activate editor
        pEdBase = pEdMgr->GetEditor(edFilename);
        if (not pEdBase) break;

        SetJumpTrackerViewIndex(m_ArrayCursor);
        // if editors needs to be switched activate the new editor
        if ( pEdBase != pEdMgr->GetActiveEditor())
            pEdMgr->SetActiveEditor(pEdBase); // cause a cbEVT_EditorActivated event
        // position to editor line
        pcbEd = pEdMgr->GetBuiltinEditor(pEdBase);
        if (not pcbEd) break;

        pcbEd->GotoLine(pcbEd->GetControl()->LineFromPosition(edPosn)); //center on scrn
        pcbEd->GetControl()->GotoPos(edPosn);

        GetJumpTrackerView()->FocusEntry(GetJumpTrackerViewIndex());

    }

    #if defined(LOGGING)
    LOGIT( _T("JT [%s]"), _T("END OnMenuJumpBack"));
    wxCommandEvent evt;
    OnMenuJumpDump(evt);
    #endif

    m_bJumpInProgress = false;
    return;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpClear(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    m_ArrayOfJumpData.Clear();
    GetJumpTrackerView()->Clear();
    m_ArrayCursor = 0;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnMenuJumpDump(wxCommandEvent &/*event*/)
// ----------------------------------------------------------------------------
{
    #if defined(LOGGING)
    if (not m_ArrayOfJumpData.GetCount())
        LOGIT( _T("JumpDump Empty"));

    for (size_t count = 0; count < m_ArrayOfJumpData.GetCount(); ++count)
    {
        JumpData& jumpData = m_ArrayOfJumpData.Item(count);
        wxString edFilename = jumpData.GetFilename();
        long edLine = wxNOT_FOUND;
        long edPosn = jumpData.GetPosition();
        cbStyledTextCtrl* pstc = 0;
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(edFilename);
        if ( ed )
        {
            pstc = ed->GetControl();
            if (not pstc) pstc = 0;
            if (pstc)
                edLine = pstc->LineFromPosition(jumpData.GetPosition());
            edLine +=1; //editors are 1 origin
        }

        wxString msg = wxString::Format(_T("[%d][%s][%ld][%ld]"), count, edFilename.c_str(), edPosn, edLine);
        if (count == (size_t)m_ArrayCursor)
            msg.Append(_T("<--c"));
        if (count == (size_t)m_insertNext)
            msg.Append(_T("<--i"));
        LOGIT( msg );
    }

    #endif
}
// ----------------------------------------------------------------------------
void JumpTracker::SetWrapJumpEntries(const bool torf)
// ----------------------------------------------------------------------------
{
    m_bWrapJumpEntries = torf;
}
// ----------------------------------------------------------------------------
int JumpTracker::GetPreviousIndex(const int idx)
// ----------------------------------------------------------------------------
{
    int prev = idx;
    int knt = m_ArrayOfJumpData.GetCount();
    prev--;
    if ( prev < 0 )
        prev = knt-1;
    if ( prev < 0 )
        prev = 0;
    return prev;
}
// ----------------------------------------------------------------------------
int JumpTracker::GetNextIndex(const int idx)
// ----------------------------------------------------------------------------
{
    int next = idx;
    int knt = m_ArrayOfJumpData.GetCount();
    next++;
    if ( next > (knt-1) )
        next = 0;
    return next;
}
// ----------------------------------------------------------------------------
void JumpTracker::SettingsSaveWinPosition()
// ----------------------------------------------------------------------------
{
    // This routine was specifically written to solve the placement of floating windows
    // It should only be called on floating windows just before the window is undocked/hidden
    // else the mainframe position will be recorded instead of the floating window.

    // record the current window position and size
    // here use the JumpTracker view window parent, not the main frame
    // Our window was reparented by wxAUI.

    wxWindow* pwin = (wxWindow*)GetJumpTrackerViewControl();
    if (pwin && pwin->GetParent() )
        pwin = pwin->GetParent();
    else return;
    int winXposn, winYposn, winWidth, winHeight;
    pwin->GetPosition( &winXposn, &winYposn );
    pwin->GetSize( &winWidth, &winHeight );

    wxString winPos;
    winPos = wxString::Format(wxT("%d %d %d %d"), winXposn,winYposn,winWidth,winHeight);
    SetConfigString("JTViewWindowPosition",  winPos) ;
    // LOGIT( _T("SavingWindowPosition[%s]"), winPos.c_str() );
}
// ----------------------------------------------------------------------------
void JumpTracker::UpdateJumpTrackerViewWindow()
// ----------------------------------------------------------------------------
{
    //message parts to LSP diagnostics: filename, lineNumber, text
    wxArrayString JumpViewItems;

    //int logFocusLine = GetLSPClient()->LSP_GetLog()->GetItemsCount();
    GetJumpTrackerView()->Clear();

    for (size_t count=0; count < m_ArrayOfJumpData.GetCount(); ++count)
    {
        JumpData& jumpData = m_ArrayOfJumpData.Item(count);
        wxString edFilename = jumpData.GetFilename();
        long edLine = wxNOT_FOUND;
        long edPosn = jumpData.GetPosition();
        cbStyledTextCtrl* pEdControl = 0;
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(edFilename);
        if ( ed )
        {
            pEdControl = ed->GetControl();
            if (not pEdControl) pEdControl = 0;
            if (pEdControl)
            {
                edLine = pEdControl->LineFromPosition(jumpData.GetPosition());
                //edLine +=1; //editors are 0 origin
                wxString edLineStr = wxString::Format("%d",int(edLine+1));

                JumpViewItems.Add(edFilename);
                JumpViewItems.Add(edLineStr);
                JumpViewItems.Add(pEdControl->GetLine(edLine).Trim(false).Trim(true) );
                //write msg to log
                //-m_pJumpTrackerView->Append(JumpViewItems);
                GetJumpTrackerView()->Append(JumpViewItems);
                JumpViewItems.Clear();
            }
        }//end if ed


        wxString msg = wxString::Format("[%d][%s][%d][%d]", int(count), edFilename.c_str(), int(edPosn), int(edLine));
        if (count == (size_t)m_ArrayCursor)
            msg.Append(_T("<--c"));
        //LOGIT( msg );
    }
}//UpdateViewWindow
// ----------------------------------------------------------------------------
int JumpTracker::FindJumpDataContaining(const wxString& filename, const long posn)
// ----------------------------------------------------------------------------
{
    // **Deprecated** //(ph 2023/01/19)

    size_t kount = m_ArrayOfJumpData.GetCount();
    if (not kount) return wxNOT_FOUND;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(filename);
    if (not ed) return wxNOT_FOUND;

    cbStyledTextCtrl* pControl = ed->GetControl();
    if (not pControl) return wxNOT_FOUND;

    // search from array insertion point so we check last entered entry first
    size_t ij = 0;
    kount = m_ArrayOfJumpData.GetCount();
    for (size_t i=0; i<kount; ++i, ++ij)
    {
        ij = GetPreviousIndex(ij);
        JumpData& jumpData = m_ArrayOfJumpData.Item(ij);
        if ( jumpData.GetFilename() not_eq filename )
            continue;
        long jumpLine = pControl->LineFromPosition(jumpData.GetPosition());
        long currLine = pControl->LineFromPosition(posn);
        if ( jumpLine == currLine)
           return ij;
    }

    return wxNOT_FOUND;
}
// ----------------------------------------------------------------------------
bool JumpTracker::JumpDataContains(const int indx, const wxString& filename, const long posn)
// ----------------------------------------------------------------------------
{
    // **Deprecated** //(ph 2023/01/19)
    size_t kount = m_ArrayOfJumpData.GetCount();
    if (not kount) return false;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(filename);
    if (not ed) return false;

    cbStyledTextCtrl* pstc = ed->GetControl();
    if (not pstc) return false;

    int halfPageSize = pstc->LinesOnScreen()>>1;

    JumpData& jumpData = m_ArrayOfJumpData.Item(indx);
    if ( jumpData.GetFilename() not_eq filename )
        return false;
    long jumpLine = pstc->LineFromPosition(jumpData.GetPosition());
    long currLine = pstc->LineFromPosition(posn);
    if ( halfPageSize > abs(jumpLine - currLine))
       return true;


    return false;
}
// ----------------------------------------------------------------------------
void JumpTracker::OnSearchLogDoubleClick(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    // User double clicked a seach log item. Record it as a jump point

    event.Skip();

    if (not m_pSearchLogControl) return;

    if (m_pSearchLogControl->GetSelectedItemCount() == 0)
        return;

    cbEditor* pcbEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (not pcbEd) return;
    wxFileName fnFile(pcbEd->GetFilename());
    if (not fnFile.Exists()) return;
    if (not fnFile.IsAbsolute())
        //?fnFile.MakeAbsolute(m_Base);
        fnFile.MakeAbsolute();

    wxArrayString columns;
    long selected = m_pSearchLogControl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selected != -1) {
        int colCount = m_pSearchLogControl->GetColumnCount();
        for (int col = 0; col < colCount; ++col) {
            columns.Add(m_pSearchLogControl->GetItemText(selected, col));
    }
    }
    wxString filenameCol = columns[0]; // not an asbsolute flename
    wxString lineCol = columns[1];
    wxString textCol = columns[2];

    long line;
    if (not lineCol.ToLong(&line))
        return;
    line -= 1; // wxScintilla lines are zero origin

    cbStyledTextCtrl* pControl = pcbEd->GetControl();
    if (pControl)
    {
        int posn = pControl->PositionFromLine(line);
        JumpDataAdd(fnFile.GetFullPath(), posn, line);
    }
}
// ----------------------------------------------------------------------------
void JumpTracker::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    GetSearchLog();
    if (m_pSearchLogControl)
    {
        // hook into double clicks of the log so we can record a jump line
        m_pSearchLogControl->Bind(
            wxEVT_LIST_ITEM_ACTIVATED,
            &JumpTracker::OnSearchLogDoubleClick,
                this);
    }
    // Ask DragScroll plugin to apply its support for this log
    wxWindow* pWindow = GetJumpTrackerView()->m_pControl;
    cbPlugin* pPlgn = Manager::Get()->GetPluginManager()->FindPluginByName(_T("cbDragScroll"));
    if (pWindow && pPlgn)
    {
        wxCommandEvent dsEvt(wxEVT_COMMAND_MENU_SELECTED, XRCID("idDragScrollAddWindow"));
        dsEvt.SetEventObject(pWindow);
        pPlgn->ProcessEvent(dsEvt);
    }

}
// ----------------------------------------------------------------------------
void JumpTracker::GetSearchLog()
// ----------------------------------------------------------------------------
{
    // If there is a "Search results" log already, remember it.
    m_pSearchLogControl = nullptr;
    int logIndex = GetSearchLogIndex("Search results");
    if (not (logIndex == wxNOT_FOUND))
    {
        LogManager* pLogMgr = Manager::Get()->GetLogManager();
        LogSlot& logslot = pLogMgr->Slot(logIndex);
        ListCtrlLogger* pLogger = (ListCtrlLogger*)logslot.GetLogger();
        if (pLogger)
            m_pSearchLogControl = (wxListCtrl*)pLogger->::ListCtrlLogger::CreateControl(0);
            // the (0) gets a read only wxListCtrl*
        return;
    }
}
// ----------------------------------------------------------------------------
int JumpTracker::GetSearchLogIndex (const wxString& logRequest)
// ----------------------------------------------------------------------------
{

    int nNumLogs = 16; //just a guess
    int searchResultsLogIndex = 0;

    LogManager* pLogMgr = Manager::Get()->GetLogManager();

    for (int ii = 0; ii < nNumLogs; ++ii)
    {
        LogSlot& logSlot = pLogMgr->Slot (ii);
        if (pLogMgr->FindIndex (logSlot.log) == pLogMgr->invalid_log) continue;
        {
            if ( logSlot.title.IsSameAs (wxT ("Search results") ) )
            {
                searchResultsLogIndex = ii;
                return searchResultsLogIndex;
            }
        }
    }//for

    return wxNOT_FOUND;
}

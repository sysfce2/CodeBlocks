/** \file wxstreectrl.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSDATAVIEWCTRL_H
#define WXSDATAVIEWCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsDataViewCtrl widget */
class wxsDataViewCtrl: public wxsWidget
{
    public:

        wxsDataViewCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long _Flags);
        virtual void OnEnumWidgetProperties(long _Flags);
        void FindAllImageLists(wxArrayString &aNames);
        void UpdateTreeItemList(void);

};

#endif

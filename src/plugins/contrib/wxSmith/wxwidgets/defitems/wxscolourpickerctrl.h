/**  \file wxscolourpickerctrl.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
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
*/

#ifndef WXSCOLOURPICKERCTRL_H
#define WXSCOLOURPICKERCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsColourPickerCtrl widget */
class wxsColourPickerCtrl: public wxsWidget
{
    public:

        wxsColourPickerCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long _Flags);
        virtual void OnEnumWidgetProperties(long _Flags);

        wxsColourData m_cdColour; //!< The selected colour.
};

#endif

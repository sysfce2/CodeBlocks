AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT
AM_CPPFLAGS += -DEXPORT_LIB
endif

noinst_LTLIBRARIES = libwxsmith_wxwidgets_defitems.la

libwxsmith_wxwidgets_defitems_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libwxsmith_wxwidgets_defitems_la_LIBADD = ../../../../../sdk/libcodeblocks.la

libwxsmith_wxwidgets_defitems_la_SOURCES = \
./wxsdatepickerctrl.cpp \
./wxsstatictext.cpp \
./wxslistctrl.cpp \
./wxsflexgridsizer.cpp \
./wxsboxsizer.cpp \
./wxsgenericdirctrl.cpp \
./wxsstaticbitmap.cpp \
./wxscheckbox.cpp \
./wxschoice.cpp \
./wxsspinctrl.cpp \
./wxscalendarctrl.cpp \
./wxsstaticline.cpp \
./wxsgridsizer.cpp \
./wxsstaticboxsizer.cpp \
./wxsbitmapbutton.cpp \
./wxstextctrl.cpp \
./wxstogglebutton.cpp \
./wxsspacer.cpp \
./wxsscrollbar.cpp \
./wxsdialog.cpp \
./wxsscrollingdialog.cpp \
./wxsspinbutton.cpp \
./wxsbutton.cpp \
./wxslistbox.cpp \
./wxseditablelistbox.cpp \
./wxsgauge.cpp \
./wxsradiobutton.cpp \
./wxspanel.cpp \
./wxsstaticbox.cpp \
./wxsradiobox.cpp \
./wxscombobox.cpp \
./wxschecklistbox.cpp \
./wxsslider.cpp \
./wxstreectrl.cpp \
./wxsframe.cpp \
./wxshtmlwindow.cpp \
./wxsscrolledwindow.cpp \
./wxsstddialogbuttonsizer.cpp \
./wxssplitterwindow.cpp \
./wxscustomwidget.cpp \
./wxsnotebook.cpp \
./wxschoicebook.cpp \
./wxslistbook.cpp \
./wxstimer.cpp \
./wxsmenubar.cpp \
./wxsmenu.cpp \
./wxsmenueditor.cpp \
./wxsmenuitem.cpp \
./wxsstatusbar.cpp \
./wxstoolbar.cpp \
./wxstoolbaritem.cpp \
./wxstoolbareditor.cpp \
./wxsgrid.cpp \
./wxscolourdialog.cpp \
./wxsdirdialog.cpp \
./wxsfiledialog.cpp \
./wxsmultichoicedialog.cpp \
./wxssinglechoicedialog.cpp \
./wxssashwindow.cpp \
./wxssashlayoutwindow.cpp \
./wxssingleinstancechecker.cpp \
./wxsstopwatch.cpp \
./wxssymbolpickerdialog.cpp \
./wxsglcanvas.cpp \
./wxshyperlinkctrl.cpp \
./wxsanimationctrl.cpp \
./wxsfindreplacedialog.cpp \
./wxsfontdialog.cpp \
./wxsmessagedialog.cpp \
./wxspagesetupdialog.cpp \
./wxspasswordentrydialog.cpp \
./wxsprintdialog.cpp \
./wxsprogressdialog.cpp \
./wxsrichtextformattingdialog.cpp \
./wxsrichtextstyleorganiserdialog.cpp \
./wxstextentrydialog.cpp \
./wxscolourpickerctrl.cpp \
./wxsdirpickerctrl.cpp \
./wxsfilepickerctrl.cpp \
./wxsfontpickerctrl.cpp \
./wxsbitmapcombobox.cpp \
./wxscontexthelpbutton.cpp \
./wxsdialupmanager.cpp \
./wxshtmleasyprinting.cpp \
./wxslistview.cpp \
./wxsrichtextctrl.cpp \
./wxssearchctrl.cpp \
./wxssimplehtmllistbox.cpp \
./wxstreebook.cpp \
./wxsimage.cpp \
./wxsimagelist.cpp \
./wxsrichtextstylecomboctrl.cpp \
./wxsrichtextstylelistbox.cpp \
./wxsrichtextstylelistctrl.cpp \
./wxsspinctrldouble.cpp \
./wxsdataviewctrl.cpp \
./wxsdataviewlistctrl.cpp \
./wxsdataviewtreectrl.cpp



noinst_HEADERS = \
./wxsgauge.h \
./wxsradiobutton.h \
./wxsstaticline.h \
./wxsstaticbox.h \
./wxscombobox.h \
./wxstextctrl.h \
./wxstogglebutton.h \
./wxsspacer.h \
./wxsslider.h \
./wxschecklistbox.h \
./wxsscrollbar.h \
./wxstreectrl.h \
./wxsframe.h \
./wxsdatepickerctrl.h \
./wxsstatictext.h \
./wxsdialog.h \
./wxsscrollingdialog.h \
./wxslistctrl.h \
./wxsspinbutton.h \
./wxsbutton.h \
./wxsstaticbitmap.h \
./wxscheckbox.h \
./wxschoice.h \
./wxsspinctrl.h \
./wxscalendarctrl.h \
./wxspanel.h \
./wxsgenericdirctrl.h \
./wxsgridsizer.h \
./wxsradiobox.h \
./wxsbitmapbutton.h \
./wxsstaticboxsizer.h \
./wxsboxsizer.h \
./wxsflexgridsizer.h \
./wxslistbox.h \
./wxseditablelistbox.h \
./wxshtmlwindow.h \
./wxsscrolledwindow.h \
./wxsstddialogbuttonsizer.h \
./wxssplitterwindow.h \
./wxscustomwidget.h \
./wxsnotebook.h \
./wxschoicebook.h \
./wxslistbook.h \
./wxstimer.h \
./wxsmenubar.h \
./wxsmenu.h \
./wxsmenueditor.h \
./wxsmenuitem.h \
./wxsstatusbar.h \
./wxstoolbar.h \
./wxstoolbaritem.h \
./wxstoolbareditor.h \
./wxsgrid.h \
./wxscolourdialog.h \
./wxsdirdialog.h \
./wxsfiledialog.h \
./wxsmultichoicedialog.h \
./wxssinglechoicedialog.h \
./wxssashwindow.h \
./wxssashlayoutwindow.h \
./wxssingleinstancechecker.h \
./wxsstopwatch.h \
./wxssymbolpickerdialog.h \
./wxsglcanvas.h \
./wxshyperlinkctrl.h \
./wxsanimationctrl.h \
./wxsfindreplacedialog.h \
./wxsfontdialog.h \
./wxsmessagedialog.h \
./wxspagesetupdialog.h \
./wxspasswordentrydialog.h \
./wxsprintdialog.h \
./wxsprogressdialog.h \
./wxsrichtextformattingdialog.h \
./wxsrichtextstyleorganiserdialog.h \
./wxstextentrydialog.h \
./wxscolourpickerctrl.h \
./wxsdirpickerctrl.h \
./wxsfilepickerctrl.h \
./wxsfontpickerctrl.h \
./wxsbitmapcombobox.h \
./wxscontexthelpbutton.h \
./wxsdialupmanager.h \
./wxshtmleasyprinting.h \
./wxslistview.h \
./wxsrichtextctrl.h \
./wxssearchctrl.h \
./wxssimplehtmllistbox.h \
./wxstreebook.h \
./wxsimage.h \
./wxsimagelist.h \
./wxsrichtextstylecomboctrl.h \
./wxsrichtextstylelistbox.h \
./wxsrichtextstylelistctrl.h \
./wxsspinctrldouble.h \
./wxsdataviewctrl.h \
./wxsdataviewtreectrl.h \
./wxsdataviewlistctrl.h

EXTRA_DIST = $(srcdir)/wxsmediactrl.*

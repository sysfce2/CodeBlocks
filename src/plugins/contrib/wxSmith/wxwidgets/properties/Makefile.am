AM_CPPFLAGS = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT
AM_CPPFLAGS += -DEXPORT_LIB
endif

noinst_LTLIBRARIES = libwxsmith_wxwidgets_properties.la

libwxsmith_wxwidgets_properties_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libwxsmith_wxwidgets_properties_la_LIBADD = ../../../../../sdk/libcodeblocks.la

libwxsmith_wxwidgets_properties_la_SOURCES = \
./wxspositionsizeproperty.cpp \
./wxsfonteditordlg.cpp \
./wxsarraystringcheckeditordlg.cpp \
./wxscolourproperty.cpp \
./wxsstyleproperty.cpp \
./wxsarraystringcheckproperty.cpp \
./wxsfontproperty.cpp \
./wxssizerflagsproperty.cpp \
./wxsfontfaceeditordlg.cpp \
./wxsbitmapiconeditordlg.cpp \
./wxsdimensionproperty.cpp \
./wxsbitmapiconproperty.cpp \
./wxssimplefonteditordlg.cpp \
./wxsimagelisteditordlg.cpp \
./wxsimagelistproperty.cpp \
./wxsimageproperty.cpp \
./wxsimagecomboeditordlg.cpp \
./wxsimagecomboproperty.cpp \
./wxsimagetreeeditordlg.cpp \
./wxsimagetreeproperty.cpp


sdkdir=$(includedir)/wxsmith/wxwidgets/properties/

sdk_HEADERS = \
./wxsdimensionproperty.h \
./wxsbitmapiconproperty.h \
./wxsarraystringcheckeditordlg.h \
./wxsfonteditordlg.h \
./wxspositionsizeproperty.h \
./wxsstyleproperty.h \
./wxscolourproperty.h \
./wxsarraystringcheckproperty.h \
./wxsfontproperty.h \
./wxssizerflagsproperty.h \
./wxsfontfaceeditordlg.h \
./wxsbitmapiconeditordlg.h \
./wxssimplefonteditordlg.h \
./wxsimagelisteditordlg.h \
./wxsimagelistproperty.h \
./wxsimageproperty.h \
./wxsimagecomboeditordlg.h \
./wxsimagecomboproperty.h \
./wxsimagetreeeditordlg.h \
./wxsimagetreeproperty.h


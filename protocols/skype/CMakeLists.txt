project( kopete_skype )

FIND_PACKAGE( Qt4 REQUIRED )
find_package( KDE4 REQUIRED )
find_package( KdepimLibs REQUIRED )
include ( KDE4Defaults )
include ( MacroLibrary )
include ( ${QT_USE_FILE} )

########### next target ###############

add_definitions ( ${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS} )

include_directories(
	${QT_QTDBUS_INCLUDE_DIR}

	${KDE4_INCLUDES}
	${KDE4_INCLUDE_DIR}/kopete
	${KDE4_INCLUDE_DIR}/kopete/ui

	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/libskype
	${CMAKE_CURRENT_SOURCE_DIR}/libskype/skypedbus
	${CMAKE_CURRENT_SOURCE_DIR}/skypebuttons
	${CMAKE_CURRENT_SOURCE_DIR}/skypebuttons/qtbrowserplugin

	${CMAKE_CURRENT_BUILD_DIR}
)

########### next target ###############

set(
	skypedbus_SRCS

	libskype/skypedbus/skypeconnection.cpp
)

QT4_ADD_DBUS_ADAPTOR(
	skypedbus_SRCS

	libskype/skypedbus/com.Skype.API.Client.xml
	libskype/skypedbus/skypeconnection.h
	SkypeConnection
)

set(
	libskype_SRCS

	libskype/skype.cpp
)

set(
	kopete_skype_ui_SRCS
)

kde4_add_ui_files(
	kopete_skype_ui_SRCS

	ui/skypeaddcontactbase.ui
	ui/skypecalldialogbase.ui
	ui/skypedetailsbase.ui
	ui/skypeeditaccountbase.ui
)

set(
	kopete_skype_SRCS

	${skypedbus_SRCS}
	${libskype_SRCS}
	${kopete_skype_ui_SRCS}
	skypeaccount.cpp
	skypeaddcontact.cpp
	skypecalldialog.cpp
	skypeconference.cpp
	skypecontact.cpp
	skypedetails.cpp
	skypeeditaccount.cpp
	skypechatsession.cpp
	skypeprotocol.cpp
)

kde4_add_plugin(
	kopete_skype

	${kopete_skype_SRCS}
)

target_link_libraries(
	kopete_skype

	kopete
	${KDE4_KDEUI_LIBS}
	${QT_QT3SUPPORT_LIBRARY}
)

########### next target ###############

add_subdirectory( icons )
# add_subdirectory( skypebuttons ) - not done all

########### install files ###############

install( TARGETS kopete_skype DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kopete_skype.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES skypeui.rc skypechatui.rc DESTINATION ${DATA_INSTALL_DIR}/kopete_skype )
install( PROGRAMS call_end call_start DESTINATION ${DATA_INSTALL_DIR}/kopete_skype )

add_subdirectory( liboscar )
add_subdirectory( aim )
add_subdirectory( icq )
add_subdirectory( icons )

include_directories(
    ${KOPETE_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/liboscar/
    ${CMAKE_CURRENT_SOURCE_DIR}/liboscar/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/liboscar/tasks
    )


########### next target ###############

set(kopete_oscar_LIB_SRCS
    oscarprotocol.cpp
    oscaraccount.cpp
    oscarcontact.cpp
    oscarmyselfcontact.cpp
    oscarencodingselectiondialog.cpp
    oscarlistnonservercontacts.cpp
    oscarversionupdater.cpp
    oscarprivacyengine.cpp
    aimcontactbase.cpp
    icqcontactbase.cpp
    oscarpresence.cpp
    oscarpresencesdataclasses.cpp
    oscarstatusmanager.cpp
    nscainfoevent.cpp)

ki18n_wrap_ui(kopete_oscar_LIB_SRCS
    oscarencodingselectionbase.ui
    oscarlistcontactsbase.ui
    )

add_library(kopete_oscar SHARED ${kopete_oscar_LIB_SRCS})

target_link_libraries(kopete_oscar KF5::KIOCore kopete oscar)

set_target_properties(kopete_oscar PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install(TARGETS kopete_oscar ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############






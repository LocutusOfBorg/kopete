include_directories(${KOPETE_INCLUDES})
set(chatwindowaccessiblewidgetfactory_SRCS chatwindowaccessiblewidgetfactory.cpp accessiblechatmessagepart.cpp)
add_library(chatwindowaccessiblewidgetfactory MODULE ${chatwindowaccessiblewidgetfactory_SRCS})
target_link_libraries(chatwindowaccessiblewidgetfactory KF5::KHtml )
install(TARGETS chatwindowaccessiblewidgetfactory DESTINATION ${KDE_INSTALL_PLUGINDIR}/accessible)

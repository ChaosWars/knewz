ADD_SUBDIRECTORY( search )
ADD_SUBDIRECTORY( icons )
ADD_SUBDIRECTORY( mimetype )
FIND_PACKAGE( PkgConfig REQUIRED )
INCLUDE( DBusMacros )
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
SET( SRC basemodel.cpp
         browserwidget.cpp
         connection.cpp
         dockbuttonwidget.cpp
         displaywidget.cpp
         downloadqueue.cpp
         file.cpp
         knewz.cpp
         knewzapplication.cpp
         knewzconfigdialog.cpp
         knewzmodel.cpp
#          knewztitlewidget.cpp
         knewzsearchmodel.cpp
         knewzview.cpp
         knewzwallet.cpp
         main.cpp
         nzbdialog.cpp
         nzbfile.cpp
         nzbhandler.cpp
         nzbmimedata.cpp
         nzbmodel.cpp
         nzbreader.cpp
         searchwidget.cpp
         segment.cpp
         serverwidget.cpp
         socket.cpp )
SET( DBUS_SERVICE_SRC com.googlecode.knewz.service.in )
PKGCONFIG_GETVAR( dbus-1 session_bus_services_dir dbus_service_file_install_dir )
KDE4_ADD_UI_FILES( SRC browserwidget.ui displaysettings.ui dockbuttonwidget.ui nzbdialog.ui searchwidget.ui serverwidget.ui )
KDE4_ADD_KCFG_FILES( SRC knewzsettings.kcfgc )
KDE4_ADD_APP_ICON( SRC "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-app-knewz.png" )
KDE4_ADD_EXECUTABLE( knewz ${SRC} )
TARGET_LINK_LIBRARIES( knewz ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} ${QT_QTWEBKIT_LIBRARY} )
DBUS_ADD_ACTIVATION_SERVICE( DBUS_SRC ${DBUS_SERVICE_SRC} )
INSTALL(TARGETS knewz DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS} )
INSTALL( FILES knewz.desktop  DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR} )
INSTALL( FILES knewzui.rc DESTINATION ${DATA_INSTALL_DIR}/knewz )
# INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/com.googlecode.knewz.service DESTINATION ${dbus_service_file_install_dir} )

FIND_PACKAGE( PkgConfig REQUIRED )
INCLUDE( DBusMacros )
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
SET( SRC basemodel.cpp
         downloadqueue.cpp
         file.cpp
         generalwidget.cpp
         knewz.cpp
         main.cpp
         knewzmodel.cpp
         knewzview.cpp
         modeltest.cpp
         nzbfile.cpp
         nzbhandler.cpp
#         nzbmimedata.cpp
         nzbreader.cpp
         nzbmodel.cpp
         nzbdialog.cpp
         segment.cpp
         serverwidget.cpp )
SET( DBUS_SERVICE_SRC org.kde.knewz.service.in )
PKGCONFIG_GETVAR( dbus-1 session_bus_services_dir dbus_service_file_install_dir )
KDE4_ADD_UI_FILES( SRC generalsettings.ui serversettings.ui )
KDE4_ADD_KCFG_FILES( SRC settings.kcfgc )
KDE4_ADD_EXECUTABLE( knewz ${SRC} )
TARGET_LINK_LIBRARIES( knewz ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} )
DBUS_ADD_ACTIVATION_SERVICE( dbus_service_files ${DBUS_SERVICE_SRC} )
INSTALL(TARGETS knewz DESTINATION ${BIN_INSTALL_DIR} )
INSTALL( FILES knewz.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
INSTALL( FILES knewzui.rc DESTINATION ${DATA_INSTALL_DIR}/knewz )
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/com.googlecode.knewz.service DESTINATION ${dbus_service_file_install_dir} )

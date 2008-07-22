SET( SRC downloadqueue.cpp
        file.cpp
        knewz.cpp
        main.cpp
        knewzmodel.cpp
        nzbfile.cpp
        nzbhandler.cpp
        nzbreader.cpp
        nzbmodel.cpp
        nzbdialog.cpp
        segment.cpp
        serverwidget.cpp )
KDE4_ADD_UI_FILES( SRC serversettings.ui )
KDE4_ADD_KCFG_FILES( SRC settings.kcfgc )
KDE4_ADD_EXECUTABLE( knewz ${SRC} )
TARGET_LINK_LIBRARIES( knewz ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} )
INSTALL(TARGETS knewz DESTINATION ${BIN_INSTALL_DIR} )
INSTALL( FILES knewzui.rc DESTINATION ${DATA_INSTALL_DIR}/knewz )

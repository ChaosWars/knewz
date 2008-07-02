FIND_PACKAGE( KDE4 REQUIRED )
INCLUDE( KDE4Defaults )
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )
SET( SRC main.cpp
        knewz.cpp
        downloadqueue.cpp
        downloadqueueobject.cpp
        segment.cpp
        file.cpp
        nzbfile.cpp
        nzbhandler.cpp
        nzbreader.cpp )
ADD_DEFINITIONS( ${KDE4_DEFINITIONS} ${QT_DEFINITIONS} )
KDE4_ADD_EXECUTABLE( knewz2 ${SRC} )
LINK_DIRECTORIES( knewz2 ${KDE4_LIB_DIR} )
TARGET_LINK_LIBRARIES( knewz2 ${KDE4_KDECORE_LIBS}
                              ${KDE4_KDEUI_LIBS}
                              ${KDE4_KFILE_LIBS}
                              ${KDE4_KIO_LIBS}
                              ${QT_QTCORE_LIBRARY} )
INSTALL(TARGETS knewz2
        RUNTIME DESTINATION ${KDE4_BIN_INSTALL_DIR}
        LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
        ARCHIVE DESTINATION ${PLUGIN_INSTALL_DIR} )
INSTALL( FILES knewz2ui.rc DESTINATION ${DATA_INSTALL_DIR}/knewz2 )

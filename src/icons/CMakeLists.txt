IF( WIN32 )
    QT4_ADD_RESOURCES( knewz.res knewz.rc )
ENDIF( WIN32 )

INSTALL( FILES knewz.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/48x48/apps )
INSTALL( FILES knewz_large.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/128x128/apps RENAME knewz.png )

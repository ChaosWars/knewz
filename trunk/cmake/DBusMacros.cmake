MACRO(PKGCONFIG_GETVAR _package _var _output_variable)
  SET(${_output_variable})

# if pkg-config has been found
  IF(PKGCONFIG_EXECUTABLE)

    EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --exists RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )

# and if the package of interest also exists for pkg-config, then get the information
    IF(NOT _return_VALUE)

      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --variable ${_var} OUTPUT_VARIABLE ${_output_variable} )

    ENDIF(NOT _return_VALUE)

  ENDIF(PKGCONFIG_EXECUTABLE)

ENDMACRO(PKGCONFIG_GETVAR _package _var _output_variable)

macro( DBUS_ADD_ACTIVATION_SERVICE _outfiles )
#     PKGCONFIG_GETVAR( dbus-1 session_bus_services_dir _install_dir )
    foreach( _current_file ${ARGN} )
        get_filename_component( _service_file ${_current_file} ABSOLUTE )
        string( REGEX REPLACE "\\.service.in$" ".service" _output_file ${_current_file} )
        set( _target ${CMAKE_CURRENT_BINARY_DIR}/${_output_file} )
        set( _outfiles "${_output_file}" )
        message( STATUS "_outfiles: " ${_outfiles} )
        configure_file( ${_service_file} ${_output_file} @ONLY )
        message( STATUS "Writing " ${_target} )
    endforeach( _current_file ${ARGN} )
endmacro( DBUS_ADD_ACTIVATION_SERVICE _outfiles )
# Macro to make sure we have a list of sources
macro (add_sources SRCS)
  file (RELATIVE_PATH _rel_path "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
  set(_srcs "")
  foreach (_src ${ARGN})
    list (APPEND _srcs "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
  endforeach()
  set_property(GLOBAL APPEND PROPERTY ${SRCS} ${_srcs})
endmacro()

# Macro to make sure we have a list of sources
macro (add_sources SRCS)
  file (RELATIVE_PATH _rel_path "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
  set(_srcs "")
  foreach (_src ${ARGN})
    list (APPEND _srcs "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
  endforeach()
  set_property(GLOBAL APPEND PROPERTY ${SRCS} ${_srcs})
endmacro()

# Create an executable for flashing on to a specified MCU
macro(mcu_target TARGET MCU COMMON_SOURCES NATIVE_SOURCES)
  add_compile_options("-mmcu=${MCU}")
  list(APPEND CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU}")

  get_property(_common_sources GLOBAL PROPERTY ${COMMON_SOURCES})
  get_property(_native_sources GLOBAL PROPERTY ${NATIVE_SOURCES})

  add_executable(${TARGET} ${_common_sources}  ${_native_sources})
endmacro()

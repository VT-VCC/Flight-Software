enable_language(ASM)

add_library(freertos STATIC
  croutine.c
  event_groups.c
  list.c
  queue.c
  tasks.c
  timers.c
  include/croutine.h
  include/deprecated_definitions.h
  include/event_groups.h
  include/FreeRTOS.h
  include/list.h
  include/mpu_prototypes.h
  #  include/mpu_wrappers.h
  include/portable.h
  include/projdefs.h
  include/queue.h
  include/semphr.h
  include/StackMacros.h
  include/task.h
  include/timers.h
  #  portable/Common/mpu_wrappers.c
  portable/MSP430X/port.c
  portable/MSP430X/portext.S
  portable/MSP430X/portmacro.h
  portable/MemMang/heap_4.c
)

set_target_properties(freertos PROPERTIES NO_SYSTEM_FROM_IMPORTED TRUE)

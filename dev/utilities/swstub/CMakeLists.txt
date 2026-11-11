# swstub: the patch stub utility.
kotor_executable(swstub
  src/swstub.c)

target_link_libraries(swstub PRIVATE kernel32.lib user32.lib advapi32.lib)

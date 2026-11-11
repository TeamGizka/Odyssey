# swkotor_release: writes release/swkotor.exe, the linked swkotor.exe with the header fields of the shipped
# executable (release_header.py: link time stamp, checksum, cleared DOS stub area).
set(KOTOR_RELEASE_EXE "${CMAKE_BINARY_DIR}/release/swkotor.exe")
add_custom_command(OUTPUT "${KOTOR_RELEASE_EXE}"
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/release/release_header.py"
          "$<TARGET_FILE:swmain>" "${KOTOR_RELEASE_EXE}"
  DEPENDS swmain "${CMAKE_SOURCE_DIR}/cmake/release/release_header.py"
  COMMENT "Writing release/swkotor.exe"
  VERBATIM)
add_custom_target(swkotor_release ALL DEPENDS "${KOTOR_RELEASE_EXE}")

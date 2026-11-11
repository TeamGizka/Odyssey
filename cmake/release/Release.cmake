# swkotor2_release: writes release/swkotor2.exe, the linked swkotor2.exe with the header fields of the shipped
# executable (release_header.py: link time stamp, checksum, cleared DOS stub area).
set(KOTOR_RELEASE_EXE "${CMAKE_BINARY_DIR}/release/swkotor2.exe")
set(KOTOR_RELEASE_TIME_DATE_STAMP 0x4226A88A)  # 2005-03-03 06:02:50 UTC
set(KOTOR_RELEASE_CHECKSUM 0)
add_custom_command(OUTPUT "${KOTOR_RELEASE_EXE}"
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/release/release_header.py"
          --time-date-stamp ${KOTOR_RELEASE_TIME_DATE_STAMP} --checksum ${KOTOR_RELEASE_CHECKSUM}
          "$<TARGET_FILE:swmain>" "${KOTOR_RELEASE_EXE}"
  DEPENDS swmain "${CMAKE_SOURCE_DIR}/cmake/release/release_header.py"
  COMMENT "Writing release/swkotor2.exe"
  VERBATIM)
add_custom_target(swkotor2_release ALL DEPENDS "${KOTOR_RELEASE_EXE}")

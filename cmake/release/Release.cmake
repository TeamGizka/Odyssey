# swkotor2_release: writes release/swkotor2.exe from the linked swkotor2.exe: the cursors (dev/game/swmain/res/
# cursors.res, extracted by scripts/extract_res.py) added with the Win32 resource-update API, which rewrites the
# resource section (release_resources.py), then the header fields of the shipped executable (release_header.py:
# link time stamp, checksum, cleared DOS stub area, uniprocessor flag).
set(KOTOR_RELEASE_EXE "${CMAKE_BINARY_DIR}/release/swkotor2.exe")
set(KOTOR_RELEASE_RESOURCES "${CMAKE_SOURCE_DIR}/dev/game/swmain/res/cursors.res")
set(KOTOR_RELEASE_STAGE "${CMAKE_BINARY_DIR}/release/swkotor2.resources.exe")
set(KOTOR_RELEASE_TIME_DATE_STAMP 0x4226A88A)  # 2005-03-03 06:02:50 UTC
set(KOTOR_RELEASE_CHECKSUM 0)
add_custom_command(OUTPUT "${KOTOR_RELEASE_EXE}"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/release"
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/release/release_resources.py"
          "$<TARGET_FILE:swmain>" "${KOTOR_RELEASE_RESOURCES}" "${KOTOR_RELEASE_STAGE}"
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/release/release_header.py"
          --time-date-stamp ${KOTOR_RELEASE_TIME_DATE_STAMP} --checksum ${KOTOR_RELEASE_CHECKSUM} --clear-stub
          --up-system-only
          "${KOTOR_RELEASE_STAGE}" "${KOTOR_RELEASE_EXE}"
  COMMAND "${CMAKE_COMMAND}" -E remove "${KOTOR_RELEASE_STAGE}"
  DEPENDS swmain "${KOTOR_RELEASE_RESOURCES}" "${CMAKE_SOURCE_DIR}/cmake/release/release_resources.py"
          "${CMAKE_SOURCE_DIR}/cmake/release/release_header.py"
  COMMENT "Writing release/swkotor2.exe"
  VERBATIM)
add_custom_target(swkotor2_release ALL DEPENDS "${KOTOR_RELEASE_EXE}")

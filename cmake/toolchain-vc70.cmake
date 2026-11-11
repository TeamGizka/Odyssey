# Toolchain: Visual Studio .NET 2002 (Visual C++ 7.0: cl 13.00.9466, link 7.00.9466) with the Platform SDK and the
# prerelease DirectX headers and libraries that ship with it.
#
# The environment variable VS2002_ROOT names the directory that holds the "Program Files/Microsoft Visual Studio .NET"
# tree of the install. CMakePresets.json takes PATH, INCLUDE and LIB from it.

if("$ENV{VS2002_ROOT}" STREQUAL "")
  message(FATAL_ERROR "Set the environment variable VS2002_ROOT to the Visual Studio .NET 2002 install "
                      "(the directory containing \"Program Files/Microsoft Visual Studio .NET\").")
endif()
file(TO_CMAKE_PATH "$ENV{VS2002_ROOT}" VS2002_ROOT)

set(VC70_ROOT "${VS2002_ROOT}/Program Files/Microsoft Visual Studio .NET/Vc7")
set(VC70_BIN "${VC70_ROOT}/bin")
if(NOT EXISTS "${VC70_BIN}/cl.exe")
  message(FATAL_ERROR "VS2002_ROOT is '${VS2002_ROOT}', but ${VC70_BIN}/cl.exe does not exist.")
endif()

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_C_COMPILER   "${VC70_BIN}/cl.exe")
set(CMAKE_CXX_COMPILER "${VC70_BIN}/cl.exe")
set(CMAKE_RC_COMPILER  "${VC70_BIN}/rc.exe")
set(CMAKE_ASM_MASM_COMPILER "${VC70_BIN}/ml.exe")
set(CMAKE_LINKER       "${VC70_BIN}/link.exe")
set(CMAKE_AR           "${VC70_BIN}/lib.exe")
set(CMAKE_MT           "")

# Headers: the Visual C++ 7.0 CRT and STL, the prerelease DirectX headers, then the Platform SDK.
# Libraries are found through LIB (CMakePresets.json), prerelease before the Platform SDK.
set(VC70_INCLUDE "${VC70_ROOT}/include")
set(PSDK70_INCLUDE "${VC70_ROOT}/PlatformSDK/common/Include")
set(VC70_INCLUDES "${VC70_INCLUDE}" "${PSDK70_INCLUDE}/prerelease" "${PSDK70_INCLUDE}")
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES   ${VC70_INCLUDES})
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${VC70_INCLUDES})
set(CMAKE_RC_STANDARD_INCLUDE_DIRECTORIES  ${VC70_INCLUDES})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

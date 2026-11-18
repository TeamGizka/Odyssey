# cmake -DINPUT=<Name>.h -DOUTPUT=<Name>.inl -P PixelShaderInitializer.cmake
#   Turns the header xsasm /h writes for one pixel shader (DWORD dw<Name>PixelShader[] = { ... };) into one braced
#   element of a D3DPIXELSHADERDEF array initializer ({ ... },), so a translation unit can build the shader table by
#   including the .inl files in table order. The values themselves are left as xsasm wrote them.
foreach(var INPUT OUTPUT)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "PixelShaderInitializer.cmake needs -D${var}=...")
  endif()
endforeach()
file(READ "${INPUT}" text)
string(REGEX REPLACE "DWORD[ \t]+dw[A-Za-z0-9_]+PixelShader\\[\\][ \t]*=[ \t]*{" "{" text "${text}")
string(REGEX REPLACE "}[ \t]*;([ \t\r\n]*)$" "},\\1" text "${text}")
if(NOT text MATCHES "^[ \t\r\n]*{" OR NOT text MATCHES "},[ \t\r\n]*$")
  message(FATAL_ERROR "${INPUT} is not a pixel shader header written by xsasm /h")
endif()
file(WRITE "${OUTPUT}" "${text}")

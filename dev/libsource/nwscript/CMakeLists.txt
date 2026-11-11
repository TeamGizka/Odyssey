# nwscript library.
kotor_library(nwscript
  nwscript.cpp
  nwvmachineinternal.cpp
  finalcode.cpp
  identspec.cpp
  lexical.cpp
  nwscriptinternal.cpp
  parsetree.cpp
  resnss.cpp)

target_compile_options(nwscript PRIVATE /YX)

kotor_automatic_pch_order(nwscript)

# exobase library.
set(sources
  exostring.cpp
  exobase.cpp
  exoini.cpp
  exofile.cpp
  exobaseinternal.cpp
  exotimers.cpp
  exocritsect.cpp
  exodebug.cpp
  exostringlist.cpp
  exolinkedlistinternal.cpp
  exolocstring.cpp
  exorand.cpp
  exoencrypt.cpp
  exoiniinternal.cpp
  exofileinternal.cpp
  exodebuginternal.cpp
  exolocstringinternal.cpp
  exorandinternal.cpp
  exocritsectinternal.cpp
  exotimersinternal.cpp
  exomemman.cpp)
kotor_library(exobase ${sources})

# No precompiled header. K2 emits header inlines such as CExoString::GetLength and CExoDebug::NoLog
# ahead of the unit's own functions (exofileinternal, exostring), which a /YX build does not do.
set_source_files_properties(exoiniinternal.cpp PROPERTIES COMPILE_OPTIONS "/Ob1")

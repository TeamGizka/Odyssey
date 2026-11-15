# netlayer library.
kotor_library(netlayer
  netlayer.cpp)

# No precompiled header: K2 emits CExoString::GetLength ahead of StartConnectToSession.

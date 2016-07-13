# - Find CryptOpp
# CRYPTOPP_ROOT hints a location
#
# This module defines
#  CRYPTOPP_INCLUDE_DIR, where to find LDAP headers
#  CRYPTOPP_STATIC_LIBRARY, the LDAP library to use.
#  cryptoppstatic, imported libraries

set(THIRDPARTY_CRYPTOPP $ENV{IMPALA_HOME}/thirdparty/cryptopp)

set(THIRDPARTY $ENV{IMPALA_HOME}/thirdparty)
set(CRYPTOPP_SEARCH_LIB_PATH
  ${CRYPTOPP_ROOT}
  ${THIRDPARTY_CRYPTOPP}
)

find_path(CRYPTOPP_INCLUDE_DIR aes.h PATHS
  ${CRYPTOPP_ROOT}
  ${THIRDPARTY_CRYPTOPP}
  NO_DEFAULT_PATH)

find_library(CRYPTOPP_STATIC_LIBRARY libcryptopp.a
  PATHS ${CRYPTOPP_SEARCH_LIB_PATH}
        NO_DEFAULT_PATH
        DOC   "Static Cryptopp library"
)

if (NOT CRYPTOPP_STATIC_LIBRARY OR NOT CRYPTOPP_INCLUDE_DIR)
  message(FATAL_ERROR "CRYPTOPP includes and libraries NOT found.")
  set(CRYPTOPP_FOUND FALSE)
else()
  set(CRYPTOPP_FOUND TRUE)
  add_library(cryptoppstatic STATIC IMPORTED)
  set_target_properties(cryptoppstatic PROPERTIES IMPORTED_LOCATION ${CRYPTOPP_STATIC_LIBRARY})
endif ()


mark_as_advanced(
  CRYPTOPP_STATIC_LIBRARY
  CRYPTOPP_INCLUDE_DIR
  cryptoppstatic
)

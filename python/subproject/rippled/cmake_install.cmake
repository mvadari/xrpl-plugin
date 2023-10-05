# Install script for directory: /Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Library/Developer/CommandLineTools/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/subproject/rippled/src/secp256k1/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/subproject/rippled/src/ed25519-donna/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/basics" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/algorithm.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Archive.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/base64.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/base_uint.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/BasicConfig.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Blob.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Buffer.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/ByteUtilities.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/chrono.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/comparators.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/CompressionAlgorithms.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/contract.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/CountedObject.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/DecayingSample.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Expected.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/FeeUnits.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/FileUtilities.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/hardened_hash.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/IOUAmount.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/join.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/KeyCache.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/LocalValue.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Log.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/make_SSLContext.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/MathUtilities.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/mulDiv.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Number.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/partitioned_unordered_map.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/PerfLog.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/random.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/RangeSet.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/README.md"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/ResolverAsio.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Resolver.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/safe_cast.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/scope.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/SHAMapHash.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/Slice.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/spinlock.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/strHex.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/StringUtilities.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/TaggedCache.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/tagged_integer.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/SubmitSync.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/ThreadSafetyAnalysis.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/ToString.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/UnorderedContainers.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/UptimeClock.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/basics/XRPAmount.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/crypto" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/crypto/RFC1751.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/crypto/csprng.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/crypto/secure_erase.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/json" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/JsonPropertyStream.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/Object.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/Output.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/Writer.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/json_forwards.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/json_reader.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/json_value.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/json_writer.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/to_string.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/json/impl" TYPE FILE FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/json/impl/json_assert.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/protocol" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/AccountID.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/AMMCore.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/AmountConversions.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Book.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/BuildInfo.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/ErrorCodes.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Feature.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Fees.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/HashPrefix.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Indexes.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/InnerObjectFormats.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Issue.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/KeyType.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Keylet.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/KnownFormats.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/LedgerFormats.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/LedgerHeader.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/NFTSyntheticSerializer.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/NFTokenID.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/NFTokenOfferID.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Protocol.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/PublicKey.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Quality.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/QualityFunction.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Rate.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Rules.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/SField.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/SOTemplate.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STAccount.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STAmount.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STIssue.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STArray.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STBase.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STBitString.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STBlob.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STExchange.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STInteger.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STLedgerEntry.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STObject.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STParsedJSON.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STPathSet.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STTx.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STValidation.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/STVector256.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/SecretKey.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Seed.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/SeqProxy.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Serializer.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/Sign.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/SystemParameters.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/TER.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/TxFlags.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/TxFormats.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/TxMeta.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/UintTypes.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/digest.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/jss.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/serialize.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/nft.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/nftPageMask.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/tokens.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/protocol/impl" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/impl/STVar.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/protocol/impl/secp256k1.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/clock" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/clock/abstract_clock.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/clock/basic_seconds_clock.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/clock/manual_clock.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/core" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/core/CurrentThreadName.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/core/LexicalCast.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/core/List.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/core/SemanticVersion.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/hash" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/hash/hash_append.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/hash/uhash.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/hash/xxhasher.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/hash/impl" TYPE FILE FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/hash/impl/xxhash.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/net" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/net/IPAddress.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/net/IPAddressConversion.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/net/IPAddressV4.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/net/IPAddressV6.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/net/IPEndpoint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/rfc2616.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/type_name.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/xor_shift_engine.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/unit_test" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/amount.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/dstream.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/global_suites.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/main.cpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/match.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/recorder.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/reporter.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/results.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/runner.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_info.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_list.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/thread.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/unit_test/detail" TYPE FILE FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/detail/const_container.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/utility" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/utility/Journal.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/utility/PropertyStream.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/utility/Zero.h"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/utility/rngfill.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/extras/unit_test" TYPE FILE FILES
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/amount.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/dstream.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/global_suites.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/match.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/recorder.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/reporter.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/results.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/runner.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_info.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_list.hpp"
    "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/thread.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/unit_test/detail" TYPE FILE FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/xrpl_plugin/rippled/src/ripple/beast/unit_test/detail/const_container.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/plugin_test_setregularkey.xrplugin")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_setregularkey.xrplugin" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_setregularkey.xrplugin")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_setregularkey.xrplugin")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/plugin_test_trustset.xrplugin")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_trustset.xrplugin" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_trustset.xrplugin")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_trustset.xrplugin")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/plugin_test_escrowcreate.xrplugin")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_escrowcreate.xrplugin" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_escrowcreate.xrplugin")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin_test_escrowcreate.xrplugin")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/proto" TYPE FILE FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/proto_gen/src/ripple/proto/ripple.pb.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/" TYPE DIRECTORY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/proto_gen_grpc/ripple" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/subproject/rippled/libxrpl_core.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxrpl_core.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxrpl_core.a")
    execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxrpl_core.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ripple/RippleTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ripple/RippleTargets.cmake"
         "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/subproject/rippled/CMakeFiles/Export/25f1b470b599c7567065c0c97c0f0760/RippleTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ripple/RippleTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ripple/RippleTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ripple" TYPE FILE FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/subproject/rippled/CMakeFiles/Export/25f1b470b599c7567065c0c97c0f0760/RippleTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ripple" TYPE FILE FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/subproject/rippled/CMakeFiles/Export/25f1b470b599c7567065c0c97c0f0760/RippleTargets-debug.cmake")
  endif()
endif()


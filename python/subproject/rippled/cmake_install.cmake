# Install script for directory: /opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled

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
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/algorithm.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Archive.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/base64.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/base_uint.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/BasicConfig.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Blob.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Buffer.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/ByteUtilities.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/chrono.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/comparators.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/CompressionAlgorithms.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/contract.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/CountedObject.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/DecayingSample.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Expected.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/FeeUnits.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/FileUtilities.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/hardened_hash.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/IOUAmount.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/join.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/KeyCache.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/LocalValue.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Log.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/make_SSLContext.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/MathUtilities.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/mulDiv.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Number.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/partitioned_unordered_map.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/PerfLog.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/random.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/RangeSet.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/README.md"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/ResolverAsio.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Resolver.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/safe_cast.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/scope.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/SHAMapHash.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/Slice.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/spinlock.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/strHex.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/StringUtilities.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/TaggedCache.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/tagged_integer.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/SubmitSync.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/ThreadSafetyAnalysis.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/ToString.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/UnorderedContainers.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/UptimeClock.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/basics/XRPAmount.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/crypto" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/crypto/RFC1751.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/crypto/csprng.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/crypto/secure_erase.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/json" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/JsonPropertyStream.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/Object.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/Output.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/Writer.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/json_forwards.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/json_reader.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/json_value.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/json_writer.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/to_string.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/json/impl" TYPE FILE FILES "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/json/impl/json_assert.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/protocol" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/AccountID.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/AMMCore.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/AmountConversions.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Book.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/BuildInfo.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/ErrorCodes.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Feature.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Fees.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/HashPrefix.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Indexes.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/InnerObjectFormats.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Issue.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/KeyType.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Keylet.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/KnownFormats.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/LedgerFormats.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/LedgerHeader.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/NFTSyntheticSerializer.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/NFTokenID.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/NFTokenOfferID.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Protocol.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/PublicKey.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Quality.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/QualityFunction.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Rate.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Rules.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/SField.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/SOTemplate.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STAccount.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STAmount.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STIssue.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STArray.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STBase.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STBitString.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STBlob.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STExchange.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STInteger.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STLedgerEntry.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STObject.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STParsedJSON.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STPathSet.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STTx.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STValidation.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/STVector256.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/SecretKey.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Seed.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/SeqProxy.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Serializer.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/Sign.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/SystemParameters.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/TER.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/TxFlags.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/TxFormats.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/TxMeta.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/UintTypes.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/digest.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/jss.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/serialize.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/nft.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/nftPageMask.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/tokens.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/protocol/impl" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/impl/STVar.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/protocol/impl/secp256k1.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/clock" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/clock/abstract_clock.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/clock/basic_seconds_clock.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/clock/manual_clock.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/core" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/core/CurrentThreadName.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/core/LexicalCast.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/core/List.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/core/SemanticVersion.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/hash" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/hash/hash_append.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/hash/uhash.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/hash/xxhasher.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/hash/impl" TYPE FILE FILES "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/hash/impl/xxhash.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/net" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/net/IPAddress.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/net/IPAddressConversion.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/net/IPAddressV4.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/net/IPAddressV6.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/net/IPEndpoint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/rfc2616.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/type_name.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/xor_shift_engine.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/unit_test" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/amount.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/dstream.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/global_suites.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/main.cpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/match.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/recorder.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/reporter.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/results.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/runner.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_info.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_list.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/thread.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/unit_test/detail" TYPE FILE FILES "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/detail/const_container.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/utility" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/utility/Journal.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/utility/PropertyStream.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/utility/Zero.h"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/utility/rngfill.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/extras/unit_test" TYPE FILE FILES
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/amount.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/dstream.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/global_suites.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/match.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/recorder.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/reporter.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/results.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/runner.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_info.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/suite_list.hpp"
    "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/thread.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ripple/beast/unit_test/detail" TYPE FILE FILES "/opt/homebrew/lib/python3.11/site-packages/xrpl_plugin/rippled/src/ripple/beast/unit_test/detail/const_container.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/libplugin_test_setregularkey.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_setregularkey.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_setregularkey.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_setregularkey.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/libplugin_test_trustset.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_trustset.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_trustset.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_trustset.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/javiromero/Documents/dev/xrpl-plugins/xrpl-plugin/python/libplugin_test_escrowcreate.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_escrowcreate.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_escrowcreate.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libplugin_test_escrowcreate.dylib")
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


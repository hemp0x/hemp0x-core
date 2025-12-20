// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Raven Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HEMP0X_HEMP0XCONSENSUS_H
#define HEMP0X_HEMP0XCONSENSUS_H

#include <stdint.h>

#if defined(BUILD_HEMP0X_INTERNAL) && defined(HAVE_CONFIG_H)
#include "config/hemp0x-config.h"
  #if defined(_WIN32)
    #if defined(DLL_EXPORT)
      #if defined(HAVE_FUNC_ATTRIBUTE_DLLEXPORT)
        #define EXPORT_SYMBOL __declspec(dllexport)
      #else
        #define EXPORT_SYMBOL
      #endif
    #endif
  #elif defined(HAVE_FUNC_ATTRIBUTE_VISIBILITY)
    #define EXPORT_SYMBOL __attribute__ ((visibility ("default")))
  #endif
#elif defined(MSC_VER) && !defined(STATIC_LIBHEMP0XCONSENSUS)
  #define EXPORT_SYMBOL __declspec(dllimport)
#endif

#ifndef EXPORT_SYMBOL
  #define EXPORT_SYMBOL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define HEMP0XCONSENSUS_API_VER 1

typedef enum hemp0xconsensus_error_t
{
    hemp0xconsensus_ERR_OK = 0,
    hemp0xconsensus_ERR_TX_INDEX,
    hemp0xconsensus_ERR_TX_SIZE_MISMATCH,
    hemp0xconsensus_ERR_TX_DESERIALIZE,
    hemp0xconsensus_ERR_AMOUNT_REQUIRED,
    hemp0xconsensus_ERR_INVALID_FLAGS,
} hemp0xconsensus_error;

/** Script verification flags */
enum
{
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_NONE                = 0,
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_P2SH                = (1U << 0), // evaluate P2SH (BIP16) subscripts
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_DERSIG              = (1U << 2), // enforce strict DER (BIP66) compliance
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_NULLDUMMY           = (1U << 4), // enforce NULLDUMMY (BIP147)
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY = (1U << 9), // enable CHECKLOCKTIMEVERIFY (BIP65)
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY = (1U << 10), // enable CHECKSEQUENCEVERIFY (BIP112)
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_WITNESS             = (1U << 11), // enable WITNESS (BIP141)
    hemp0xconsensus_SCRIPT_FLAGS_VERIFY_ALL                 = hemp0xconsensus_SCRIPT_FLAGS_VERIFY_P2SH | hemp0xconsensus_SCRIPT_FLAGS_VERIFY_DERSIG |
                                                               hemp0xconsensus_SCRIPT_FLAGS_VERIFY_NULLDUMMY | hemp0xconsensus_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY |
                                                               hemp0xconsensus_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY | hemp0xconsensus_SCRIPT_FLAGS_VERIFY_WITNESS
};

/// Returns 1 if the input nIn of the serialized transaction pointed to by
/// txTo correctly spends the scriptPubKey pointed to by scriptPubKey under
/// the additional constraints specified by flags.
/// If not nullptr, err will contain an error/success code for the operation
EXPORT_SYMBOL int hemp0xconsensus_verify_script(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen,
                                                 const unsigned char *txTo        , unsigned int txToLen,
                                                 unsigned int nIn, unsigned int flags, hemp0xconsensus_error* err);

EXPORT_SYMBOL int hemp0xconsensus_verify_script_with_amount(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen, int64_t amount,
                                    const unsigned char *txTo        , unsigned int txToLen,
                                    unsigned int nIn, unsigned int flags, hemp0xconsensus_error* err);

EXPORT_SYMBOL unsigned int hemp0xconsensus_version();

#ifdef __cplusplus
} // extern "C"
#endif

#undef EXPORT_SYMBOL

#endif // HEMP0X_HEMP0XCONSENSUS_H

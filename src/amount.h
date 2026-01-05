// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Raven Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HEMP0X_AMOUNT_H
#define HEMP0X_AMOUNT_H

#include <stdint.h>

/** Amount in corbies (Can be negative) */
typedef int64_t CAmount;

static const CAmount COIN = 100000000;
static const CAmount CENT = 1000000;

/** No amount larger than this (in satoshi) is valid.
 *
 * Note that this constant is *not* the total HEMP coin supply, which is 420M
 * and controlled by the emission schedule. This value serves as a sanity check
 * ceiling for validation, particularly for asset issuance quantities. As this
 * is used by consensus-critical validation code, the exact value of MAX_MONEY
 * is consensus critical; modification could lead to a fork.
 * */
static const CAmount MAX_MONEY = 21000000000 * COIN;
inline bool MoneyRange(const CAmount& nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

#endif //  HEMP0X_AMOUNT_H

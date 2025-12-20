// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Raven Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HEMP0X_QT_HEMP0XADDRESSVALIDATOR_H
#define HEMP0X_QT_HEMP0XADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class Hemp0xAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit Hemp0xAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Hemp0x address widget validator, checks for a valid hemp0x address.
 */
class Hemp0xAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit Hemp0xAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // HEMP0X_QT_HEMP0XADDRESSVALIDATOR_H

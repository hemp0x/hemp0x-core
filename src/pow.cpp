// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017-2020 The Raven Core developers
// Modified Copyright (c) 2025-2026 The Hemp0x developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"
#include "validation.h"
#include "chainparams.h"
#include "tinyformat.h"

unsigned int static DarkGravityWave(const CBlockIndex* pindexLast,
                                    const CBlockHeader* pblock,
                                    const Consensus::Params& params)
{
    assert(pindexLast != nullptr);

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    const unsigned int nPowLimitBits = bnPowLimit.GetCompact();

    const int64_t nPastBlocks = 180;

    const int nextHeight = pindexLast->nHeight + 1;

    // ---------------------------------------------------------------------
    //
    // If DGW got poisoned (insane difficulty) near height ~180, forcing only
    // one block (181) isn't enough because DGW uses the last 180 nBits values.
    // We force powLimit long enough to overwrite that history.
    //
    // ---------------------------------------------------------------------
    if (nextHeight >= params.nDGWFixHeight &&
        nextHeight <  params.nDGWFixHeight + nPastBlocks)
    {
        return nPowLimitBits;
    }

    // Need at least nPastBlocks blocks, otherwise return powLimit
    if (pindexLast->nHeight < nPastBlocks) {
        return nPowLimitBits;
    }

    // Special min-difficulty rules (unchanged)
    if (params.fPowAllowMinDifficultyBlocks && params.fPowNoRetargeting) {
        if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 2) {
            return nPowLimitBits;
        } else {
            const CBlockIndex* pindex = pindexLast;
            while (pindex->pprev &&
                   pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 &&
                   pindex->nBits == nPowLimitBits)
            {
                pindex = pindex->pprev;
            }
            return pindex->nBits;
        }
    }

    // Average targets over past blocks + count KAWPOW blocks
    const CBlockIndex* pindex = pindexLast;
    arith_uint256 bnPastTargetAvg;
    int nKAWPOWBlocksFound = 0;

    for (int i = 1; i <= nPastBlocks; ++i) {
        arith_uint256 bnTarget;
        bnTarget.SetCompact(pindex->nBits);

        if (i == 1) bnPastTargetAvg = bnTarget;
        else        bnPastTargetAvg = (bnPastTargetAvg * i + bnTarget) / (i + 1);

        if (pindex->nTime >= nKAWPOWActivationTime) nKAWPOWBlocksFound++;

        if (i != nPastBlocks) {
            assert(pindex->pprev);
            pindex = pindex->pprev;
        }
    }

    // KAWPOW transition safety (unchanged)
    if (pblock->nTime >= nKAWPOWActivationTime) {
        if (nKAWPOWBlocksFound != nPastBlocks) {
            return UintToArith256(params.kawpowLimit).GetCompact();
        }
    }

    arith_uint256 bnNew(bnPastTargetAvg);

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    const int64_t nTargetTimespan = nPastBlocks * params.nPowTargetSpacing;

    // Post-fix safety: never allow 0/negative timespans
    if (nextHeight >= params.nDGWFixHeight) {
        if (nActualTimespan < 1) nActualTimespan = 1;

        // Optional: keep extremely fast chains from causing wild spikes
        if (nActualTimespan < params.nPowTargetSpacing)
            nActualTimespan = params.nPowTargetSpacing;
    }

    // DGW clamp
    if (nActualTimespan < nTargetTimespan / 3) nActualTimespan = nTargetTimespan / 3;
    if (nActualTimespan > nTargetTimespan * 3) nActualTimespan = nTargetTimespan * 3;

    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > bnPowLimit) bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequiredBTC(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
//    int64_t nPrevBlockTime = (pindexLast->pprev ? pindexLast->pprev->GetBlockTime() : pindexLast->GetBlockTime());  //<- Commented out - fixes "not used" warning

    if (IsDGWActive(pindexLast->nHeight + 1)) {
//        LogPrint(BCLog::NET, "Block %s - version: %s: found next work required using DGW: [%s] (BTC would have been [%s]\t(%+d)\t(%0.3f%%)\t(%s sec))\n",
//                 pindexLast->nHeight + 1, pblock->nVersion, dgw, btc, btc - dgw, (float)(btc - dgw) * 100.0 / (float)dgw, pindexLast->GetBlockTime() - nPrevBlockTime);
        return DarkGravityWave(pindexLast, pblock, params);
    }
    else {
//        LogPrint(BCLog::NET, "Block %s - version: %s: found next work required using BTC: [%s] (DGW would have been [%s]\t(%+d)\t(%0.3f%%)\t(%s sec))\n",
//                  pindexLast->nHeight + 1, pblock->nVersion, btc, dgw, dgw - btc, (float)(dgw - btc) * 100.0 / (float)btc, pindexLast->GetBlockTime() - nPrevBlockTime);
        return GetNextWorkRequiredBTC(pindexLast, pblock, params);
    }

}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}

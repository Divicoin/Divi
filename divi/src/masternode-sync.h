// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MASTERNODE_SYNC_H
#define MASTERNODE_SYNC_H

class CMasternodeSync;
class CNode;
class CDataStream;
class MasternodeNetworkMessageManager;
class MasternodePaymentData;
class CSporkManager;
class CTxIn;

#include <map>
#include <uint256.h>
#include <stdint.h>
#include <string>
class I_PeerSyncQueryService;
class I_Clock;
class I_BlockchainSyncQueryService;
class CNetFulfilledRequestManager;
class CNode;
//
// CMasternodeSync : Sync masternode assets in stages
//
enum SyncStatus
{
    FAIL,
    SUCCESS,
    REQUEST_SYNC,
};

enum MasternodeSyncCode: int
{
    MASTERNODE_SYNC_INITIAL = 0,
    MASTERNODE_SYNC_SPORKS = 1,
    MASTERNODE_SYNC_LIST = 2,
    MASTERNODE_SYNC_MNW = 3,
    MASTERNODE_SYNC_FAILED = 998,
    MASTERNODE_SYNC_FINISHED = 999,
};

class CMasternodeSync
{
private:
    int64_t nTimeLastProcess = 0;
    CNetFulfilledRequestManager& networkFulfilledRequestManager_;
    const I_PeerSyncQueryService& peerSyncService_;
    const I_Clock& clock_;
    const I_BlockchainSyncQueryService& blockchainSync_;
    MasternodeNetworkMessageManager& networkMessageManager_;
    MasternodePaymentData& masternodePaymentData_;

    /** Set to true when we delay masternode syncing while the blockchain
     *  is still catching up.  */
    bool waitingForBlockchainSync;

public:
    std::map<uint256, int> mapSeenSyncMNB;
    std::map<uint256, int> mapSeenSyncMNW;

    int64_t timestampOfLastMasternodeListUpdate;
    int64_t timestampOfLastMasternodeWinnerUpdate;
    int64_t timestampOfLastFailedSync;
    int countOfFailedSyncAttempts;

    // sum of all counts
    int nominalNumberOfMasternodeBroadcastsReceived;
    int nominalNumberOfMasternodeWinnersReceived;
    // peers that reported counts
    int fulfilledMasternodeListSyncRequests;
    int fulfilledMasternodeWinnerSyncRequests;

    // Count peers we've requested the list from
    int currentMasternodeSyncStatus;
    int totalSuccessivePeerSyncRequests;

    // Time when current masternode asset sync started
    int64_t lastSyncStageStartTimestamp;

    CMasternodeSync(
        CNetFulfilledRequestManager& networkFulfilledRequestManager,
        const I_PeerSyncQueryService& peerSyncService,
        const I_Clock& clock,
        const I_BlockchainSyncQueryService& blockchainSync,
        MasternodeNetworkMessageManager& networkMessageManager,
        MasternodePaymentData& masternodePaymentData);

    void RecordMasternodeListUpdate(const uint256& hash);
    void RecordMasternodeWinnerUpdate(const uint256& hash);
    void ContinueToNextSyncStage();
    std::string GetSyncStatus();
    void ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);

    void Reset();
    uint32_t masternodeCount() const;
    void DsegUpdate(CNode* pnode);
    bool ShouldWaitForSync(const int64_t now);
    SyncStatus SyncAssets(CNode* pnode, const int64_t now,const int64_t lastUpdate, std::string assetType);
    bool MasternodeListIsSynced(CNode* pnode, const int64_t now);
    bool MasternodeWinnersListIsSync(CNode* pnode, const int64_t now);
    void Process(bool networkIsRegtest);
    bool IsSynced() const;
    bool IsMasternodeListSynced() const;
    void AskForMN(CNode* pnode, const CTxIn& vin) const;
    void ClearTimedOutAndExpiredRequests(bool forceExpiredRemoval = false);
};

#endif

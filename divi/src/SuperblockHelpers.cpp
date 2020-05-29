#include <SuperblockHelpers.h>
#include <chainparams.h>
#include <BlockRewards.h>

bool Legacy::IsValidLotteryBlockHeight(int nBlockHeight,const CChainParams& chainParams)
{
    return nBlockHeight >= chainParams.GetLotteryBlockStartBlock() &&
            ((nBlockHeight % chainParams.GetLotteryBlockCycle()) == 0);
}

bool Legacy::IsValidTreasuryBlockHeight(int nBlockHeight,const CChainParams& chainParams)
{
    return nBlockHeight >= chainParams.GetTreasuryPaymentsStartBlock() &&
            ((nBlockHeight % chainParams.GetTreasuryPaymentsCycle()) == 0);
}

bool IsValidLotteryBlockHeight(int nBlockHeight)
{
    LotteryAndTreasuryBlockSubsidyIncentives incentives(Params());
    return incentives.IsValidLotteryBlockHeight(nBlockHeight);
}

bool IsValidTreasuryBlockHeight(int nBlockHeight)
{
    LotteryAndTreasuryBlockSubsidyIncentives incentives(Params());
    return incentives.IsValidTreasuryBlockHeight(nBlockHeight);
}

int64_t GetTreasuryReward(const CBlockRewards &rewards)
{
    return rewards.nTreasuryReward * Params().GetTreasuryPaymentsCycle();
}

int64_t GetCharityReward(const CBlockRewards &rewards)
{
    return rewards.nCharityReward * Params().GetTreasuryPaymentsCycle();
}

int64_t GetLotteryReward(const CBlockRewards &rewards)
{
    // 50 coins every block for lottery
    return Params().GetLotteryBlockCycle() * rewards.nLotteryReward;
}

LotteryAndTreasuryBlockSubsidyIncentives::LotteryAndTreasuryBlockSubsidyIncentives(
    const CChainParams& chainParameters
    ): chainParameters_(chainParameters)
    , transitionHeight_(chainParameters_.GetLotteryBlockCycle()*chainParameters_.GetTreasuryPaymentsCycle())
    , superblockCycleLength_((chainParameters_.GetLotteryBlockCycle()+chainParameters_.GetTreasuryPaymentsCycle())/2)
{
}

bool LotteryAndTreasuryBlockSubsidyIncentives::IsValidLotteryBlockHeight(int nBlockHeight)
{
    if(nBlockHeight < transitionHeight_)
    {
        return Legacy::IsValidLotteryBlockHeight(nBlockHeight,chainParameters_);
    }
    else
    {
        return ((nBlockHeight - transitionHeight_) % superblockCycleLength_) == 0;
    }
}
bool LotteryAndTreasuryBlockSubsidyIncentives::IsValidTreasuryBlockHeight(int nBlockHeight)
{
    if(nBlockHeight < transitionHeight_)
    {
        return Legacy::IsValidTreasuryBlockHeight(nBlockHeight,chainParameters_);
    }
    else
    {
        return IsValidLotteryBlockHeight(nBlockHeight-1);
    }
}

int LotteryAndTreasuryBlockSubsidyIncentives::getTransitionHeight() const
{
    return transitionHeight_;
}

const CChainParams& LotteryAndTreasuryBlockSubsidyIncentives::getChainParameters() const
{
    return chainParameters_;
}

int LotteryAndTreasuryBlockSubsidyIncentives::getTreasuryCycleLength(int blockHeight) const
{
    if(blockHeight > transitionHeight_+1)
    {
        return superblockCycleLength_;
    }
    else
    {
        return chainParameters_.GetTreasuryPaymentsCycle() + ((blockHeight == transitionHeight_+1)? 1:0);
    }
}
int  LotteryAndTreasuryBlockSubsidyIncentives::getLotteryCycleLength(int blockHeight) const
{
    return (blockHeight <= transitionHeight_)? chainParameters_.GetLotteryBlockCycle():superblockCycleLength_;
}

int64_t LotteryAndTreasuryBlockSubsidyIncentives::GetTreasuryReward(const CBlockRewards &rewards, int nBlockHeight)
{
    return rewards.nTreasuryReward * getTreasuryCycleLength(nBlockHeight);
}

int64_t LotteryAndTreasuryBlockSubsidyIncentives::GetCharityReward(const CBlockRewards &rewards, int nBlockHeight)
{
    return rewards.nCharityReward * getTreasuryCycleLength(nBlockHeight);
}

int64_t LotteryAndTreasuryBlockSubsidyIncentives::GetLotteryReward(const CBlockRewards &rewards, int nBlockHeight)
{
    // 50 coins every block for lottery
    return rewards.nLotteryReward * getLotteryCycleLength(nBlockHeight);
}
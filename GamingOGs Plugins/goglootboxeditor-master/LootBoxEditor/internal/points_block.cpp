#include "points_block.h"

namespace LootBox
{

    PointsBlock::PointsBlock(Box* parent)
        : BlockNode(parent, NodeType::PointsBlock)
    {
    }

    PointsBlock::PointsBlock(Box* parent, const JSON& in)
        : BlockNode(parent, NodeType::PointsBlock, in)
    {
        min_amount_ = Utils::ExtractInt(in, "MinPoints");
        max_amount_ = Utils::ExtractInt(in, "MaxPoints");
    }

    qint32 PointsBlock::GetMinAmount() const
    {
        return min_amount_;
    }

    qint32 PointsBlock::GetMaxAmount() const
    {
        return max_amount_;
    }

    void PointsBlock::SetMinAmount(qint32 val)
    {
        min_amount_ = val;
    }

    void PointsBlock::SetMaxAmount(qint32 val)
    {
        max_amount_ = val;
    }

    JSON PointsBlock::ToJSON() const
    {
        JSON out = BlockNode::ToJSON();
        out["MinPoints"] = min_amount_;
        out["MaxPoints"] = max_amount_;
        return out;
    }

    bool PointsBlock::CheckValidity() const
    {
        return (BlockNode::CheckValidity() && min_amount_ > 0 && max_amount_ > 0 && max_amount_ >= min_amount_);
    }

}

#include "item.h"

namespace LootBox
{

Item::Item(BlockNode* parent)
    : PrimalNode(parent, NodeType::Item)
{
}

Item::Item(BlockNode* parent, const JSON& in)
    : PrimalNode(parent, NodeType::Item)
{
    min_quality_ = Utils::ExtractInt(in, "MinQuality");
    max_quality_ = Utils::ExtractInt(in, "MaxQuality");
    amount_ = Utils::ExtractInt(in, "Amount");
    add_stat_ = Utils::ExtractInt(in, "AddStat");
    bp_chance_ = Utils::ExtractInt(in, "BlueprintChance");
    bp_ = Utils::ExtractString(in, "Blueprint");
}

qint32 Item::GetMinQuality() const
{
    return min_quality_;
}

qint32 Item::GetMaxQuality() const
{
    return max_quality_;
}

qint32 Item::GetAmount() const
{
    return amount_;
}

qint32 Item::GetAddStatFlag() const
{
    return add_stat_;
}

qint32 Item::GetBPChance() const
{
    return bp_chance_;
}

QString Item::GetBP() const
{
    return bp_;
}

void Item::SetMinQuality(qint32 val)
{
    min_quality_ = val;
}

void Item::SetMaxQuality(qint32 val)
{
    max_quality_ = val;
}

void Item::SetAmount(qint32 val)
{
    amount_ = val;
}

void Item::SetAddStatFlag(qint32 val)
{
    add_stat_ = val;
}

void Item::SetBPChance(qint32 val)
{
    bp_chance_ = val;
}

void Item::SetBP(const QString& val)
{
    bp_ = val;
}

JSON Item::ToJSON() const
{
    JSON out = PrimalNode::ToJSON();
    out["MinQuality"] = min_quality_;
    out["MaxQuality"] = max_quality_;
    out["Amount"] = amount_;
    out["AddStat"] = add_stat_;
    out["BlueprintChance"] = bp_chance_;
    out["Blueprint"] = bp_.toStdString();
    return out;
}

bool Item::CheckValidity() const
{
    return (PrimalNode::CheckValidity() &&
            min_quality_ >= 0 &&
            min_quality_ <= 100 &&
            max_quality_ >= 0 &&
            max_quality_ <= 100 &&
            amount_ > 0 &&
            bp_chance_ >= 0 &&
            bp_chance_ <= 100 &&
            !bp_.isEmpty());
}

}

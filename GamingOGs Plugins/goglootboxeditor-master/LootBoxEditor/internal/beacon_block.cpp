#include "beacon_block.h"

namespace LootBox
{

    BeaconBlock::BeaconBlock(Box* parent)
        : BlockNode(parent, NodeType::BeaconBlock)
    {
    }

    BeaconBlock::BeaconBlock(Box* parent, const JSON& in)
        : BlockNode(parent, NodeType::BeaconBlock, in)
    {
        class_name_ = Utils::ExtractString(in, "ClassName");
    }

    QString BeaconBlock::GetClassName() const
    {
        return class_name_;
    }

    void BeaconBlock::SetClassName(const QString& val)
    {
        class_name_ = val;
    }

    JSON BeaconBlock::ToJSON() const
    {
        JSON out = BlockNode::ToJSON();
        out["ClassName"] = class_name_.toStdString();
        return out;
    }

    bool BeaconBlock::CheckValidity() const
    {
        return (BlockNode::CheckValidity() && !class_name_.isEmpty());
    }

}

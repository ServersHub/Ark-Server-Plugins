#include "node.h"

#include "item.h"
#include "dino.h"
#include "item_block.h"
#include "dino_block.h"
#include "command_block.h"
#include "beacon_block.h"
#include "points_block.h"

namespace LootBox
{

    INode::INode(NodeType type)
        : type_(type)
    {
    }

    bool INode::IsPrimal() const
    {
        return (type_ == NodeType::Item || type_ == NodeType::Dino);
    }

    bool INode::IsBlock() const
    {
        return !IsPrimal();
    }

    NodeType INode::GetType() const
    {
        return type_;
    }

    BlockNode::BlockNode(Box* parent, NodeType type)
        : INode(type), parent_(parent)
    {
    }

    BlockNode::BlockNode(Box* parent, NodeType type, const JSON& in)
        : INode(type), parent_(parent)
    {
        description_ = Utils::ExtractString(in, "Description");
        probability_ = Utils::ExtractInt(in, "Probability");
    }

    BlockNode* BlockNode::Create(Box* parent, const JSON& in)
    {
        const NodeType type = Utils::ExtractBlockNodeType(in);
        switch(type)
        {
        case NodeType::ItemBlock:
            return new ItemBlock(parent, in);
        case NodeType::DinoBlock:
            return new DinoBlock(parent, in);
        case NodeType::CommandBlock:
            return new CommandBlock(parent, in);
        case NodeType::BeaconBlock:
            return new BeaconBlock(parent, in);
        case NodeType::PointsBlock:
            return new PointsBlock(parent, in);
        default:
            return nullptr;
        }
    }

    BlockNode* BlockNode::Create(Box* parent, NodeType type)
    {
        switch(type)
        {
        case NodeType::ItemBlock:
            return new ItemBlock(parent);
        case NodeType::DinoBlock:
            return new DinoBlock(parent);
        case NodeType::CommandBlock:
            return new CommandBlock(parent);
        case NodeType::BeaconBlock:
            return new BeaconBlock(parent);
        case NodeType::PointsBlock:
            return new PointsBlock(parent);
        default:
            return nullptr;
        }
    }

    Box* BlockNode::GetParent()
    {
        return parent_;
    }

    QString BlockNode::GetDescription() const
    {
        return description_;
    }

    qint32 BlockNode::GetProbability() const
    {
        return probability_;
    }

    void BlockNode::SetParent(Box* val)
    {
        parent_ = val;
    }

    void BlockNode::SetDescription(const QString& val)
    {
        description_ = val;
    }

    void BlockNode::SetProbability(qint32 val)
    {
        probability_ = val;
    }

    JSON BlockNode::ToJSON() const
    {
        JSON out = JSON::object();
        out["Type"] = Utils::PrepareElementType(GetType()).toStdString();
        out["Description"] = description_.toStdString();
        out["Probability"] = probability_;
        return out;
    }

    bool BlockNode::CheckValidity() const
    {
        return (!(description_.isEmpty()) &&
                ((probability_ > 0) && (probability_ < 100)));
    }

    PrimalNode::PrimalNode(BlockNode* parent, NodeType type)
        : INode(type), parent_(parent)
    {
    }

    PrimalNode* PrimalNode::Create(BlockNode* parent, NodeType type, const JSON& in)
    {
        switch(type)
        {
        case NodeType::Item:
            return new Item(parent, in);
        case NodeType::Dino:
            return new Dino(parent, in);
        default:
            return nullptr;
        }
    }

    PrimalNode* PrimalNode::Create(BlockNode* parent, NodeType type)
    {
        switch(type)
        {
        case NodeType::Item:
            return new Item(parent);
        case NodeType::Dino:
            return new Dino(parent);
        default:
            return nullptr;
        }
    }

    BlockNode* PrimalNode::GetParent()
    {
        return parent_;
    }

    void PrimalNode::SetParent(BlockNode* ptr)
    {
        parent_ = ptr;
    }

    JSON PrimalNode::ToJSON() const
    {
        return JSON::object();
    }

    bool PrimalNode::CheckValidity() const
    {
        return true;
    }

}

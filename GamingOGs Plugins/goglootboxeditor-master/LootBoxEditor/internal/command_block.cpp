#include "command_block.h"

namespace LootBox
{

CommandBlock::CommandBlock(Box* parent)
    : BlockNode(parent, NodeType::CommandBlock)
{
}

CommandBlock::CommandBlock(Box* parent, const JSON& in)
    : BlockNode(parent, NodeType::CommandBlock, in)
{
    command_ = Utils::ExtractString(in, "Command");
}

QString CommandBlock::GetCommand() const
{
    return command_;
}

void CommandBlock::SetCommand(const QString& val)
{
    command_ = val;
}

JSON CommandBlock::ToJSON() const
{
    JSON out = BlockNode::ToJSON();
    out["Command"] = command_.toStdString();
    return out;
}

bool CommandBlock::CheckValidity() const
{
    return (BlockNode::CheckValidity() && !command_.isEmpty() && command_.count("{}") == 1);
}

}

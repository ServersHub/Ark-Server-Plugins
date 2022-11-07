#ifndef COMMAND_BLOCK_H
#define COMMAND_BLOCK_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

class Box;

class CommandBlock : public BlockNode
{
public:
    CommandBlock(Box* parent);
    CommandBlock(Box* parent, const JSON& in);
    ~CommandBlock() override final = default;

    QString GetCommand() const;
    void SetCommand(const QString& val);

    JSON ToJSON() const override final;
    bool CheckValidity() const override final;

private:
    QString command_;
};

}

Q_DECLARE_METATYPE(LootBox::CommandBlock*);

#endif // COMMAND_BLOCK_H

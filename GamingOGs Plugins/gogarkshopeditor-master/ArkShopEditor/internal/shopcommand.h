#ifndef SHOPCOMMAND_H
#define SHOPCOMMAND_H

#include <QMetaType>
#include <QtGlobal>

#include "json_utils.h"
#include "node.h"

namespace Shop
{

    class ShopCommand : public ShopNode
    {
    public:
        explicit ShopCommand(const QString& name)
            : ShopNode(NodeType::ShopCommand, name), commands_(std::vector<QString>{})
        {
        }

        ShopCommand(const QString& name, const JSON& in)
            : ShopNode(NodeType::ShopCommand, name, in)
        {
            for(const auto& command : in["Items"])
            {
                std::vector<QString> cmds{};
                cmds.push_back(QString::fromStdString(command["Command"].get<std::string>()));
                commands_ = std::move(cmds);
            }
        }

        ~ShopCommand() override = default;

        Field<std::vector<QString>>& Commands()
        {
            return commands_;
        }

        JSON Dump() const override final
        {
            JSON out = ShopNode::Dump();
            JSON commands = JSON::array();
            for(const auto& command : *commands_)
            {
                commands.push_back(JSON{{"Command", command.toStdString()}});
            }
            out["Items"] = std::move(commands);
            return out;
        }

        bool IsValid() const override final
        {
            return ShopNode::IsValid() && commands_;
        }

    private:
        Field<std::vector<QString>> commands_;
    };

}

Q_DECLARE_METATYPE(Shop::ShopCommand*);

#endif // SHOPCOMMAND_H

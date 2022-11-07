#ifndef SHOPENGRAM_H
#define SHOPENGRAM_H

#include <QMetaType>
#include <QtGlobal>

#include "json_utils.h"
#include "node.h"

namespace Shop
{

    class ShopEngram : public ShopNode
    {
    public:
        explicit ShopEngram(const QString& name)
            : ShopNode(NodeType::ShopEngram, name), blueprints_(std::vector<QString>{})
        {
        }

        ShopEngram(const QString& name, const JSON& in)
            : ShopNode(NodeType::ShopEngram, name, in)
        {
            for(const auto& blueprint : in["Items"])
            {
                std::vector<QString> bps{};
                bps.push_back(QString::fromStdString(blueprint["Blueprint"].get<std::string>()));
                blueprints_ = std::move(bps);
            }
        }

        ~ShopEngram() override = default;

        Field<std::vector<QString>>& Blueprints()
        {
            return blueprints_;
        }

        JSON Dump() const override final
        {
            JSON out = ShopNode::Dump();
            JSON blueprints = JSON::array();
            for(const auto& blueprint : *blueprints_)
            {
                blueprints.push_back(JSON{{"Blueprint", blueprint.toStdString()}});
            }
            out["Items"] = std::move(blueprints);
            return out;
        }

        bool IsValid() const override final
        {
            return ShopNode::IsValid() && blueprints_;
        }

    private:
        Field<std::vector<QString>> blueprints_;
    };

}

Q_DECLARE_METATYPE(Shop::ShopEngram*);

#endif // SHOPENGRAM_H

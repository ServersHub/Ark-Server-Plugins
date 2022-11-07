#ifndef SHOPEXPERIENCE_H
#define SHOPEXPERIENCE_H

#include <QMetaType>
#include <QtGlobal>

#include "json_utils.h"
#include "node.h"

namespace Shop
{

    class ShopExperience : public ShopNode
    {
    public:
        explicit ShopExperience(const QString& name)
            : ShopNode(NodeType::ShopExperience, name)
        {
        }

        ShopExperience(const QString& name, const JSON& in)
            : ShopNode(NodeType::ShopExperience, name, in)
        {
            give_to_dino_.FromOptional(Utils::ExtractBool(in, "GiveToDino"));
            amount_.FromOptional(Utils::ExtractInt(in, "Amount"));
        }

        ~ShopExperience() override = default;

        Field<bool>& GiveToDino()
        {
            return give_to_dino_;
        }

        Field<qint32>& Amount()
        {
            return amount_;
        }

        JSON Dump() const override final
        {
            JSON out = ShopNode::Dump();
            Utils::DumpBaseField(out, give_to_dino_, "GiveToDino");
            Utils::DumpBaseField(out, amount_, "Amount");
            return out;
        }

        bool IsValid() const override final
        {
            return ShopNode::IsValid() &&
                    give_to_dino_ &&
                    amount_ &&
                    *amount_ > 0;
        }

    private:
        Field<bool> give_to_dino_;
        Field<qint32> amount_;
    };

}

Q_DECLARE_METATYPE(Shop::ShopExperience*);

#endif // SHOPEXPERIENCE_H

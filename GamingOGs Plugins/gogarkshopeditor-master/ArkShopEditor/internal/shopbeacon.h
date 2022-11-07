#ifndef SHOPBEACON_H
#define SHOPBEACON_H

#include <QMetaType>
#include <QtGlobal>

#include "json_utils.h"
#include "node.h"

namespace Shop
{

    class ShopBeacon : public ShopNode
    {
    public:
        explicit ShopBeacon(const QString& name)
            : ShopNode(NodeType::ShopBeacon, name)
        {
        }

        ShopBeacon(const QString& name, const JSON& in)
            : ShopNode(NodeType::ShopBeacon, name, in)
        {
            class_name_.FromOptional(Utils::ExtractString(in, "ClassName"));
        }

        ~ShopBeacon() override = default;

        Field<QString>& ClassName()
        {
            return class_name_;
        }

        JSON Dump() const override final
        {
            JSON out = ShopNode::Dump();
            Utils::DumpBaseField(out, class_name_, "ClassName");
            return out;
        }

        bool IsValid() const override final
        {
            return ShopNode::IsValid() &&
                    class_name_ &&
                    !(*class_name_).isEmpty();
        }

    private:
        Field<QString> class_name_;
    };

}

Q_DECLARE_METATYPE(Shop::ShopBeacon*);

#endif // SHOPBEACON_H

#ifndef SELLSTOREITEM_H
#define SELLSTOREITEM_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace Shop
{

    class SellStoreItem : public INode
    {
    public:
        SellStoreItem()
            : INode(NodeType::SellStoreItem)
        {
        }

        SellStoreItem(const QString& name, const JSON& in)
            : INode(NodeType::SellStoreItem)
        {
            name_ = name;
            type_ = "item";
            description_.FromOptional(Utils::ExtractString(in, "Description"));
            price_.FromOptional(Utils::ExtractInt(in, "Price"));
            amount_.FromOptional(Utils::ExtractInt(in, "Amount"));
            blueprint_.FromOptional(Utils::ExtractString(in, "Blueprint"));
        }

        SellStoreItem(const QString& name)
            : INode(NodeType::SellStoreItem), name_(name)
        {
        }

        ~SellStoreItem() override = default;

        Field<QString>& Name()
        {
            return name_;
        }

        Field<QString>& Type()
        {
            return type_;
        }

        Field<QString>& Description()
        {
            return description_;
        }

        Field<qint32>& Price()
        {
            return price_;
        }

        Field<qint32>& Amount()
        {
            return amount_;
        }

        Field<QString>& Blueprint()
        {
            return blueprint_;
        }

        JSON Dump() const override final
        {
            auto out = JSON::object();
            Utils::DumpBaseField(out, type_, "Type");
            Utils::DumpBaseField(out, description_, "Description");
            Utils::DumpBaseField(out, price_, "Price");
            Utils::DumpBaseField(out, amount_, "Amount");
            Utils::DumpBaseField(out, blueprint_, "Blueprint");
            return out;
        }

        bool IsValid() const override final
        {
            return type_ &&
                    price_ &&
                    *price_ >= 0 &&
                    name_ &&
                    !(*name_).isEmpty();
        }

    private:
        Field<QString> name_;
        Field<QString> type_;
        Field<QString> description_;
        Field<qint32> price_;
        Field<qint32> amount_;
        Field<QString> blueprint_;
    };

}

Q_DECLARE_METATYPE(Shop::SellStoreItem*);

#endif // SELLSTOREITEM_H

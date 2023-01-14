#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace Shop
{

    class Item : public INode
    {
    public:
        Item()
            : INode(NodeType::Item)
        {
        }

        Item(const JSON& in)
            : INode(NodeType::Item)
        {
            quality_.FromOptional(Utils::ExtractInt(in, "Quality"));
            amount_.FromOptional(Utils::ExtractInt(in, "Amount"));
            blueprint_.FromOptional(Utils::ExtractString(in, "Blueprint"));
            force_bp_.FromOptional(Utils::ExtractBool(in, "ForceBlueprint"));
        }

        ~Item() override = default;

        Field<qint32>& Quality()
        {
            return quality_;
        }

        Field<qint32>& Amount()
        {
            return amount_;
        }

        Field<QString>& Blueprint()
        {
            return blueprint_;
        }

        Field<bool>& ForceBlueprint()
        {
            return force_bp_;
        }

        JSON Dump() const override final
        {
            JSON out = JSON::object();
            Utils::DumpBaseField(out, quality_, "Quality");
            Utils::DumpBaseField(out, amount_, "Amount");
            Utils::DumpBaseField(out, blueprint_, "Blueprint");
            Utils::DumpBaseField(out, force_bp_, "ForceBlueprint");
            return out;
        }

        bool IsValid() const override final
        {
            return quality_ &&
                   *quality_ >= 0 &&
                   *quality_ <= 100 &&
                   amount_ &&
                   *amount_ > 0 &&
                   blueprint_ &&
                   !(*blueprint_).isEmpty() &&
                   force_bp_;
        }

    private:
        Field<qint32> quality_;
        Field<qint32> amount_;
        Field<QString> blueprint_;
        Field<bool> force_bp_;
    };

};

Q_DECLARE_METATYPE(Shop::Item*);

#endif // ITEM_H

#ifndef SHOPDINO_H
#define SHOPDINO_H

#include <QMetaType>
#include <QtGlobal>

#include "json_utils.h"
#include "node.h"

namespace Shop
{

    class ShopDino : public ShopNode
    {
    public:
        explicit ShopDino(const QString& name)
            : ShopNode(NodeType::ShopDino, name)
        {
        }

        ShopDino(const QString& name, const JSON& in)
            : ShopNode(NodeType::ShopDino, name, in)
        {
            level_.FromOptional(Utils::ExtractInt(in, "Level"));
            neutered_.FromOptional(Utils::ExtractBool(in, "Neutered"));
            blueprint_.FromOptional(Utils::ExtractString(in, "Blueprint"));
        }

        ~ShopDino() override = default;

        Field<qint32>& Level()
        {
            return level_;
        }

        Field<bool>& Neutered()
        {
            return neutered_;
        }

        Field<QString>& Blueprint()
        {
            return blueprint_;
        }

        JSON Dump() const override final
        {
            JSON out = ShopNode::Dump();
            Utils::DumpBaseField(out, level_, "Level");
            Utils::DumpBaseField(out, neutered_, "Neutered");
            Utils::DumpBaseField(out, blueprint_, "Blueprint");
            return out;
        }

        bool IsValid() const override final
        {
            return ShopNode::IsValid() &&
                    level_ && *level_ > 0 &&
                    blueprint_ && !(*blueprint_).isEmpty();
        }

    private:
        Field<qint32> level_;
        Field<bool> neutered_;
        Field<QString> blueprint_;
    };

}

Q_DECLARE_METATYPE(Shop::ShopDino*);

#endif // SHOPDINO_H

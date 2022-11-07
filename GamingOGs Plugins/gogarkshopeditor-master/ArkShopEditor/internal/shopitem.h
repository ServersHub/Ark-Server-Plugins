#ifndef SHOPITEM_H
#define SHOPITEM_H

#include <QMetaType>
#include <QtGlobal>

#include "json_utils.h"
#include "node.h"
#include "item.h"

namespace Shop
{

    class ShopItem : public ShopNode
    {
    public:
        explicit ShopItem(const QString& name)
            : ShopNode(NodeType::ShopItem, name), items_(std::vector<Item*>{})
        {
        }

        ShopItem(const QString& name, const JSON& in)
            : ShopNode(NodeType::ShopItem, name, in)
        {
            for(const auto& item : in["Items"])
            {
                (*items_).push_back(new Item(item));
            }
        }

        ~ShopItem() override
        {
            for(auto item : *items_)
            {
                delete item;
            }
        }

        Field<std::vector<Item*>>& Items()
        {
            return items_;
        }

        JSON Dump() const override final
        {
            JSON out = ShopNode::Dump();
            JSON items = JSON::array();
            for(const auto item : *items_)
            {
                items.push_back(item->Dump());
            }
            out["Items"] = std::move(items);
            return out;
        }

        bool IsValid() const override final
        {
            for(const auto item : *items_)
            {
                if(!item->IsValid())
                {
                    return false;
                }
            }
            return ShopNode::IsValid();
        }


    private:
        Field<std::vector<Item*>> items_;
    };

}

Q_DECLARE_METATYPE(Shop::ShopItem*);

#endif // SHOPITEM_H

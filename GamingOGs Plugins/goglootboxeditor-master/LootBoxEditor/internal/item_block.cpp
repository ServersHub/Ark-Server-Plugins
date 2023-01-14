#include "item_block.h"

#include "item.h"

namespace LootBox
{

    ItemBlock::ItemBlock(Box* parent)
        : BlockNode(parent, NodeType::ItemBlock)
    {
    }

    ItemBlock::ItemBlock(Box* parent, const JSON& in)
        : BlockNode(parent, NodeType::ItemBlock, in)
    {
        if(auto it = in.find("Items"); it != in.end())
        {
            for(const auto& item : *it)
            {
                PrimalNode* item_ptr = PrimalNode::Create(reinterpret_cast<BlockNode*>(this), NodeType::Item, item);
                items_.push_back(reinterpret_cast<Item*>(item_ptr));
            }
        }
    }

    ItemBlock::~ItemBlock()
    {
        for(Item* item : items_)
        {
            delete item;
        }
    }

    std::vector<Item *>& ItemBlock::GetItems()
    {
        return items_;
    }

    void ItemBlock::AddItem(Item* item)
    {
        items_.push_back(item);
    }

    void ItemBlock::RemoveItem(Item* item)
    {
        const auto it = std::find_if(items_.cbegin(), items_.cend(),
                                     [item](const Item* ptr) { return item == ptr; });
        if(it != items_.cend())
        {
            items_.erase(it);
        }
    }

    JSON ItemBlock::ToJSON() const
    {
        JSON out = BlockNode::ToJSON();
        out["Items"] = JSON::array();
        for(const auto& item : items_)
        {
            out["Items"].push_back(item->ToJSON());
        }
        return out;
    }

    bool ItemBlock::CheckValidity() const
    {
        if(!BlockNode::CheckValidity())
        {
            return false;
        }

        for(const auto item : items_)
        {
            if(!item->CheckValidity())
            {
                return false;
            }
        }

        return true;
    }

}

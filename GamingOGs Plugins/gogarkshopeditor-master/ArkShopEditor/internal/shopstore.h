#ifndef SHOPSTORE_H
#define SHOPSTORE_H

#include "json_utils.h"
#include "node.h"
#include "shopitem.h"
#include "shopdino.h"
#include "shopbeacon.h"
#include "shopexperience.h"
#include "shopengram.h"
#include "shopcommand.h"

namespace Shop {

class ShopStore
{
public:
    ShopStore(const JSON& in)
    {
        for(const auto& item : in.items())
        {
            const std::string type = item.value()["Type"].get<std::string>();
            if(type == "item")
            {
                items_.push_back(new ShopItem(QString::fromStdString(item.key()), item.value()));
            }
            else if(type == "dino")
            {
                items_.push_back(new ShopDino(QString::fromStdString(item.key()), item.value()));
            }
            else if(type == "beacon")
            {
                items_.push_back(new ShopBeacon(QString::fromStdString(item.key()), item.value()));
            }
            else if(type == "experience")
            {
                items_.push_back(new ShopExperience(QString::fromStdString(item.key()), item.value()));
            }
            else if(type == "unlockengram")
            {
                items_.push_back(new ShopEngram(QString::fromStdString(item.key()), item.value()));
            }
            else if(type == "command")
            {
                items_.push_back(new ShopCommand(QString::fromStdString(item.key()), item.value()));
            }
        }
    }

    ~ShopStore()
    {
        for(auto item : items_)
        {
            delete item;
        }
    }

    ShopStore() = delete;

    void AddItem(ShopNode* item)
    {
        items_.push_back(item);
    }

    void RemoveItem(ShopNode* item)
    {
        const auto it = std::find_if(items_.cbegin(), items_.cend(), [item](const ShopNode* ptr) { return item == ptr; });
        if(it != items_.end())
        {
            items_.erase(it);
        }
    }

    bool IsValid() const
    {
        for(const auto item : items_)
        {
            if(!item->IsValid())
            {
                return false;
            }
        }
        return true;
    }

    JSON Dump() const
    {
        JSON out = JSON::object();
        for(const auto item : items_)
        {
            out[(*item->Name()).toStdString()] = item->Dump();
        }
        return out;
    }

    std::vector<ShopNode*>& GetItems()
    {
        return items_;
    }

private:
    std::vector<ShopNode*> items_;
};

}

#endif // SHOPSTORE_H

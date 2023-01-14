#ifndef SELLSTORE_H
#define SELLSTORE_H

#include "json_utils.h"
#include "sellstoreitem.h"

namespace Shop
{

    class SellStore
    {
    public:
        SellStore() = delete;

        explicit SellStore(const JSON& in)
        {
            for(const auto& item : in.items())
            {
                auto item_ptr = new SellStoreItem(QString::fromStdString(item.key()), item.value());
                items_.push_back(item_ptr);
            }
        }

        ~SellStore()
        {
            for(auto item : items_)
            {
                delete item;
            }
        }

        void AddItem(SellStoreItem* item)
        {
            items_.push_back(item);
        }

        void RemoveItem(SellStoreItem* item)
        {
            const auto it = std::find_if(items_.cbegin(), items_.cend(),
                                         [item](const SellStoreItem* ptr) { return item == ptr; });
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

        std::vector<SellStoreItem*>& GetItems()
        {
            return items_;
        }

    private:
        std::vector<SellStoreItem*> items_;
    };

}

#endif // SELLSTORE_H

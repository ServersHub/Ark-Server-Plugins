#ifndef KITS_H
#define KITS_H

#include <vector>
#include "json_utils.h"
#include "kit.h"

namespace Shop
{

    class Kits
    {
    public:
        explicit Kits(const JSON& in)
        {
            for(const auto& kit : in.items())
            {
                auto kit_ptr = new Kit(QString::fromStdString(kit.key()), kit.value());
                kits_.push_back(kit_ptr);
            }
        }

        bool IsValid() const
        {
            for(const auto kit : kits_)
            {
                if(!kit->IsValid())
                {
                    return false;
                }
            }
            return true;
        }

        JSON Dump() const
        {
            JSON out = JSON::object();
            for(const auto kit : kits_)
            {
                out[(*kit->Name()).toStdString()] = kit->Dump();
            }
            return out;
        }

        void AddKit(Kit* kit)
        {
            kits_.push_back(kit);
        }

        void RemoveKit(Kit* kit)
        {
            const auto it = std::find_if(kits_.cbegin(), kits_.cend(), [kit](const Kit* item) { return item == kit; });
            if(it != kits_.end())
            {
                kits_.erase(it);
            }
        }

        std::vector<Kit*>& GetKits()
        {
            return kits_;
        }

    private:
        std::vector<Kit*> kits_;
    };

}

#endif // KITS_H

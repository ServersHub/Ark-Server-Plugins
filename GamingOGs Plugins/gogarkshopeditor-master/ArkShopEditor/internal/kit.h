#ifndef KIT_H
#define KIT_H

#include <QMetaType>
#include <QtGlobal>
#include <vector>
#include "json_utils.h"
#include "item.h"
#include "dino.h"

namespace Shop
{

    class Kit
    {
    public:
        Kit(const QString& name, const JSON& in)
        {
            name_ = name;
            default_amount_.FromOptional(Utils::ExtractInt(in, "DefaultAmount"));
            price_.FromOptional(Utils::ExtractInt(in, "Price"));
            min_level_.FromOptional(Utils::ExtractInt(in, "MinLevel"));
            max_level_.FromOptional(Utils::ExtractInt(in, "MaxLevel"));
            permissions_.FromOptional(Utils::ExtractString(in, "Permissions"));
            description_.FromOptional(Utils::ExtractString(in, "Description"));
            only_from_spawn_.FromOptional(Utils::ExtractBool(in, "OnlyFromSpawn"));

            if(const auto it = in.find("Items"); it != in.end())
            {
                for(const auto& item : (*it))
                {
                    Item* new_item = new Item(item);
                    (*items_).push_back(new_item);
                }
            }

            if(const auto it = in.find("Dinos"); it != in.end())
            {
                for(const auto& dino : (*it))
                {
                    Dino* new_dino = new Dino(dino);
                    (*dinos_).push_back(new_dino);
                }
            }
        }

        Kit(const QString& name)
        {
            name_ = name;
        }

        Field<QString>& Name()
        {
            return name_;
        }

        Field<qint32>& DefaultAmount()
        {
            return default_amount_;
        }

        Field<qint32>& Price()
        {
            return price_;
        }

        Field<qint32>& MinLevel()
        {
            return min_level_;
        }

        Field<qint32>& MaxLevel()
        {
            return max_level_;
        }

        Field<QString>&Permissions()
        {
            return permissions_;
        }

        Field<QString>& Description()
        {
            return description_;
        }

        Field<bool>& IsOnlyFromSpawn()
        {
            return only_from_spawn_;
        }

        Field<std::vector<Item*>>& Items()
        {
            return items_;
        }

        Field<std::vector<Dino*>>& Dinos()
        {
            return dinos_;
        }

        JSON Dump() const
        {
            auto out = JSON::object();
            Utils::DumpBaseField(out, default_amount_, "DefaultAmount");
            Utils::DumpBaseField(out, price_, "Price");
            Utils::DumpBaseField(out, min_level_, "MinLevel");
            Utils::DumpBaseField(out, max_level_, "MaxLevel");
            Utils::DumpBaseField(out, permissions_, "Permissions");
            Utils::DumpBaseField(out, description_, "Description");
            Utils::DumpBaseField(out, only_from_spawn_, "OnlyFromSpawn");

            if(!(*items_).empty())
            {
                JSON items_array = JSON::array();
                for(const auto& item : *items_)
                {
                    items_array.push_back(item->Dump());
                }
                out["Items"] = std::move(items_array);
            }

            if(!(*dinos_).empty())
            {
                JSON dinos_array = JSON::array();
                for(const auto& dino : *dinos_)
                {
                    dinos_array.push_back(dino->Dump());
                }
                out["Dinos"] = std::move(dinos_array);
            }

            return out;
        }

        bool IsValid() const
        {
            return !(*name_).isEmpty();
        }

    private:
        Field<QString> name_;
        Field<qint32> default_amount_;
        Field<qint32> price_;
        Field<qint32> min_level_;
        Field<qint32> max_level_;
        Field<QString> permissions_;
        Field<QString> description_;
        Field<bool> only_from_spawn_;
        Field<std::vector<Item*>> items_;
        Field<std::vector<Dino*>> dinos_;
    };

}

Q_DECLARE_METATYPE(Shop::Kit*);

#endif // KIT_H

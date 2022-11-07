#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <QString>
#include <optional>
#include "json.hpp"
#include "fifo_map.hpp"

namespace Shop
{
    template<class K, class V, class dummy_compare, class A>
    using LessCompareIgnore = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
    using JSON = nlohmann::basic_json<LessCompareIgnore>;

    enum class NodeType
    {
        Unknown = 0,
        Item,
        Dino,
        SellStoreItem,
        ShopItem,
        ShopDino,
        ShopBeacon,
        ShopExperience,
        ShopEngram,
        ShopCommand
    };

}

namespace Shop
{

    template <class _Ty>
    class Field
    {
    public:
        Field() : value_(_Ty()), is_null_(true)
        {
        }

        explicit Field(const _Ty& value) : value_(value), is_null_(false)
        {
        }

        _Ty operator*() const
        {
            return value_;
        }

        _Ty& operator*()
        {
            return value_;
        }

        operator bool() const
        {
            return !is_null_;
        }

        Field<_Ty>& operator=(_Ty value)
        {
            value_ = std::move(value);
            is_null_ = false;
            return *this;
        }

        void FromOptional(std::optional<_Ty> value)
        {
            if(value.has_value())
            {
                value_ = std::move(value.value());
                is_null_ = false;
            }
            else
            {
                is_null_ = true;
            }
        }

        void setNull(bool val = true)
        {
            is_null_ = val;
        }

    private:
        _Ty value_;
        bool is_null_;
    };

    namespace Utils
    {

        inline std::optional<bool> ExtractBool(const JSON& in, const QString& key)
        {
            if(in.find(key.toStdString()) != in.end())
            {
                return std::make_optional<bool>(in[key.toStdString()].get<bool>());
            }
            return std::nullopt;
        }

        inline std::optional<QString> ExtractString(const JSON& in, const QString& key)
        {
            if(in.find(key.toStdString()) != in.end())
            {
                return std::make_optional<QString>(in[key.toStdString()].get<std::string>().c_str());
            }
            return std::nullopt;
        }

        inline std::optional<qint32> ExtractInt(const JSON& in, const QString& key)
        {
            if(in.find(key.toStdString()) != in.end())
            {
                return std::make_optional<qint32>(in[key.toStdString()].get<qint32>());
            }
            return std::nullopt;
        }

        inline QString UnescapeString(const QString& str)
        {
            QString unescaped = str;
            if(unescaped.startsWith("\""))
            {
                unescaped.remove(0, 1);
            }
            if(unescaped.endsWith("\""))
            {
                unescaped.remove(unescaped.size() - 1, 1);
            }
            return unescaped;
        }

        template <class _Ty>
        inline void DumpBaseField(JSON& container, const Field<_Ty>& field, const QString& key)
        {
            if(field)
            {
                container[key.toStdString()] = *field;
            }
        }

        template <>
        inline void DumpBaseField(JSON& container, const Field<QString>& field, const QString& key)
        {
            if(field)
            {
                container[key.toStdString()] = (*field).toStdString();
            }
        }

        inline QString TypeEnum2String(NodeType type)
        {
            switch (type)
            {
            case NodeType::ShopItem:
                return "item";
            case NodeType::ShopDino:
                return "dino";
            case NodeType::ShopBeacon:
                return "beacon";
            case NodeType::ShopExperience:
                return "experience";
            case NodeType::ShopEngram:
                return "unlockengram";
            case NodeType::ShopCommand:
                return "command";
            default:
                return "unknown";
            }
        }

    }

}

#endif // JSON_UTILS_H

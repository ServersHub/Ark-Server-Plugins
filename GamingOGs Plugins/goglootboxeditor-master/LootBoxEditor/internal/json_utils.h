#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <QString>
#include "json.hpp"
#include "fifo_map.hpp"

namespace LootBox
{
    template<class K, class V, class dummy_compare, class A>
    using LessCompareIgnore = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
    using JSON = nlohmann::basic_json<LessCompareIgnore>;

    enum class NodeType
    {
        Unknown = 0,
        Item,
        Dino,
        ItemBlock,
        DinoBlock,
        CommandBlock,
        BeaconBlock,
        PointsBlock
    };

    enum class BooleanValue
    {
        None = 0,
        True,
        False
    };

}

namespace LootBox::Utils
{

    inline QString BlockNodeTypeAsString(NodeType type)
    {
        switch(type)
        {
        case NodeType::ItemBlock:
            return "item";
        case NodeType::DinoBlock:
            return "dino";
        case NodeType::BeaconBlock:
            return "beacon";
        case NodeType::CommandBlock:
            return "command";
        case NodeType::PointsBlock:
            return "points";
        default:
            return "";
        }
    }

    inline NodeType BlockNodeTypeAsEnum(const QString& str)
    {
        if(str == "item")
        {
            return NodeType::ItemBlock;
        }
        else if(str == "dino")
        {
            return NodeType::DinoBlock;
        }
        else if(str == "beacon")
        {
            return NodeType::BeaconBlock;
        }
        else if(str == "command")
        {
            return NodeType::CommandBlock;
        }
        else if(str == "points")
        {
            return NodeType::PointsBlock;
        }
        return NodeType::Unknown;
    }

    inline bool BooleanToNative(BooleanValue val)
    {
        if(val == BooleanValue::True)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    inline BooleanValue NativeToBoolean(bool val)
    {
        if(val)
        {
            return BooleanValue::True;
        }
        else
        {
            return BooleanValue::False;
        }
    }

    inline NodeType ExtractBlockNodeType(const JSON& in, const QString& key = "Type")
    {
        QString type_string = "";
        if(in.find(key.toStdString()) != in.end())
        {
            type_string = QString::fromStdString(in[key.toStdString()]);
            return BlockNodeTypeAsEnum(type_string);
        }
        return NodeType::Unknown;
    }

    inline QString PrepareElementType(NodeType type)
    {
        return BlockNodeTypeAsString(type);
    }

    inline BooleanValue ExtractBool(const JSON& in, const QString& key)
    {
        if(in.find(key.toStdString()) != in.end())
        {
            return in[key.toStdString()].get<bool>() ? BooleanValue::True : BooleanValue::False;
        }
        return BooleanValue::None;
    }

    inline bool PrepareBool(BooleanValue value)
    {
        return BooleanToNative(value);
    }

    inline QString ExtractString(const JSON& in, const QString& key)
    {
        if(in.find(key.toStdString()) != in.end())
        {
            return in[key.toStdString()].get<std::string>().c_str();
        }
        return "";
    }

    inline qint32 ExtractInt(const JSON& in, const QString& key)
    {
        if(in.find(key.toStdString()) != in.end())
        {
            return in[key.toStdString()].get<qint32>();
        }
        else
        {
            return 0;
        }
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

}

#endif // JSON_UTILS_H

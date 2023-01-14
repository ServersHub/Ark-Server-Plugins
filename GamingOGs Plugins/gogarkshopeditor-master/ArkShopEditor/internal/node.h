#ifndef NODE_H
#define NODE_H

#include <QtGlobal>
#include <QMetaType>
#include "json_utils.h"

namespace Shop
{

    class NamedTypeTrait
    {
    public:
        NamedTypeTrait() = delete;

        NamedTypeTrait(const QString& name)
            : name_(name)
        {
        }

        virtual ~NamedTypeTrait() {}

        Field<QString>& Name()
        {
            return name_;
        }

    private:
        Field<QString> name_;
    };

    class INode
    {
    public:
        INode() = delete;
        INode(NodeType type)
            : type_(type)
        {
        }

        virtual ~INode() {}

        NodeType GetType() const
        {
            return type_;
        }

        virtual JSON Dump() const = 0;

        virtual bool IsValid() const = 0;

    private:
        NodeType type_;
    };

    class ShopNode : public INode, public NamedTypeTrait
    {
    public:
        ShopNode(NodeType type, const QString& name)
            : INode(type), NamedTypeTrait(name)
        {
        }

        ShopNode(NodeType type, const QString& name, const JSON& in)
            : INode(type), NamedTypeTrait(name)
        {
            description_.FromOptional(Utils::ExtractString(in, "Description"));
            min_level_.FromOptional(Utils::ExtractInt(in, "MinLevel"));
            max_level_.FromOptional(Utils::ExtractInt(in, "MaxLevel"));
            price_.FromOptional(Utils::ExtractInt(in, "Price"));
        }

        Field<QString>& Description()
        {
            return description_;
        }

        Field<qint32>& MinLevel()
        {
            return min_level_;
        }

        Field<qint32>& MaxLevel()
        {
            return max_level_;
        }

        Field<qint32>& Price()
        {
            return price_;
        }

        JSON Dump() const override
        {
            JSON out = JSON::object();
            switch (GetType())
            {
            case NodeType::ShopItem:
                out["Type"] = "item";
                break;
            case NodeType::ShopDino:
                out["Type"] = "dino";
                break;
            case NodeType::ShopBeacon:
                out["Type"] = "beacon";
                break;
            case NodeType::ShopExperience:
                out["Type"] = "experience";
                break;
            case NodeType::ShopEngram:
                out["Type"] = "unlockengram";
                break;
            case NodeType::ShopCommand:
                out["Type"] = "command";
                break;
            default:
                out["Type"] = "unknown";
                break;
            }
            Utils::DumpBaseField(out, description_, "Description");
            Utils::DumpBaseField(out, min_level_, "MinLevel");
            Utils::DumpBaseField(out, max_level_, "MaxLevel");
            Utils::DumpBaseField(out, price_, "Price");
            return out;
        }

        virtual bool IsValid() const override
        {
            return (!min_level_ || *min_level_ > 0) &&
                    (!max_level_ || (*max_level_ > 0 && *max_level_ >= *min_level_)) &&
                    price_ && *price_ > 0;
        }

    private:
        Field<QString> description_;
        Field<qint32> min_level_;
        Field<qint32> max_level_;
        Field<qint32> price_;
    };

}

Q_DECLARE_METATYPE(Shop::INode*);

#endif // NODE_H

#ifndef DINO_H
#define DINO_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace Shop
{

    class Dino : public INode
    {
    public:
        Dino()
            : INode(NodeType::Dino)
        {
        }

        explicit Dino(const JSON& in)
            : INode(NodeType::Dino)
        {
            level_.FromOptional(Utils::ExtractInt(in, "Level"));
            neutered_.FromOptional(Utils::ExtractBool(in, "Neutered"));
            blueprint_.FromOptional(Utils::ExtractString(in, "Blueprint"));
        }

        ~Dino() override = default;

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
            JSON out = JSON::object();
            Utils::DumpBaseField(out, level_, "Level");
            Utils::DumpBaseField(out, neutered_, "Neutered");
            Utils::DumpBaseField(out, blueprint_, "Blueprint");
            return out;
        }

        bool IsValid() const override final
        {
            return level_ &&
                   *level_ > 0 &&
                   !(*blueprint_).isEmpty();
        }

    private:
        Field<qint32> level_;
        Field<bool> neutered_;
        Field<QString> blueprint_;
    };

}

Q_DECLARE_METATYPE(Shop::Dino*);

#endif // DINO_H

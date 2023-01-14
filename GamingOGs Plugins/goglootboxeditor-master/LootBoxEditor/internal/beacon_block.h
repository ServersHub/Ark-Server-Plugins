#ifndef BEACON_BLOCK_H
#define BEACON_BLOCK_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

    class Box;

    class BeaconBlock : public BlockNode
    {
    public:
        BeaconBlock(Box* parent);
        BeaconBlock(Box* parent, const JSON& in);
        ~BeaconBlock() override final = default;

        QString GetClassName() const;
        void SetClassName(const QString& val);

        JSON ToJSON() const override final;
        bool CheckValidity() const override final;

    private:
        QString class_name_;

    };

}

Q_DECLARE_METATYPE(LootBox::BeaconBlock*);

#endif // BEACON_BLOCK_H

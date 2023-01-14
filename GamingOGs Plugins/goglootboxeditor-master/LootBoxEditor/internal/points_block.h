#ifndef POINTS_BLOCK_H
#define POINTS_BLOCK_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

    class Box;

    class PointsBlock : public BlockNode
    {
    public:
        PointsBlock(Box* parent);
        PointsBlock(Box* parent, const JSON& in);
        ~PointsBlock() override final = default;

        qint32 GetMinAmount() const;
        qint32 GetMaxAmount() const;
        void SetMinAmount(qint32 val);
        void SetMaxAmount(qint32 val);

        JSON ToJSON() const override final;
        bool CheckValidity() const override final;

    public:
        qint32 min_amount_;
        qint32 max_amount_;
    };

}

Q_DECLARE_METATYPE(LootBox::PointsBlock*);

#endif // POINTS_BLOCK_H

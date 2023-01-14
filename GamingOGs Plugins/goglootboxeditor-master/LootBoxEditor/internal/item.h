#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

    class Item : public PrimalNode
    {
    public:
        Item() = delete;
        Item(BlockNode* parent);
        Item(BlockNode* parent, const JSON& in);
        ~Item() override = default;

        qint32 GetMinQuality() const;
        qint32 GetMaxQuality() const;
        qint32 GetAmount() const;
        qint32 GetAddStatFlag() const;
        qint32 GetBPChance() const;
        QString GetBP() const;

        void SetMinQuality(qint32 val);
        void SetMaxQuality(qint32 val);
        void SetAmount(qint32 val);
        void SetAddStatFlag(qint32 val);
        void SetBPChance(qint32 val);
        void SetBP(const QString& val);

        JSON ToJSON() const override final;
        bool CheckValidity() const override final;

    private:
        qint32 min_quality_ = 0;
        qint32 max_quality_ = 0;
        qint32 amount_ = 0;
        qint32 add_stat_ = 0;
        qint32 bp_chance_ = 0;
        QString bp_ = "";
    };

};

Q_DECLARE_METATYPE(LootBox::Item*);

#endif // ITEM_H

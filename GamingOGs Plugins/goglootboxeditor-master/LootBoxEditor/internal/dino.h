#ifndef DINO_H
#define DINO_H

#include <QString>
#include <QtGlobal>
#include <QMetaType>

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

    class Dino : public PrimalNode
    {
    public:
        Dino() = delete;
        Dino(BlockNode* parent);
        Dino(BlockNode* parent, const JSON& in);
        ~Dino() override = default;

        qint32 GetLevel() const;
        BooleanValue GetBabyFlag() const;
        qint32 GetImprintAmount() const;
        BooleanValue GetNeuteredFlag() const;
        QString GetBP() const;
        QString GetSaddleBP() const;
        qint32 GetSaddleQuality() const;

        void SetLevel(qint32 val);
        void SetBabyFlag(BooleanValue val);
        void SetImprintAmount(qint32 val);
        void SetNeuteredFlag(BooleanValue val);
        void SetBP(const QString& val);
        void SetSaddleBP(const QString& val);
        void SetSaddleQuality(qint32 val);

        JSON ToJSON() const override final;
        bool CheckValidity() const override final;

    private:
        qint32 level_ = 0;
        BooleanValue is_baby_ = BooleanValue::None;
        qint32 imprint_amount_ = 0;
        BooleanValue neutered_ = BooleanValue::None;
        QString bp_ = "";
        QString saddle_bp_ = "";
        qint32 saddle_quality_ = 0;
    };

}

Q_DECLARE_METATYPE(LootBox::Dino*);

#endif // DINO_H

#include "dino.h"

namespace LootBox
{

    Dino::Dino(BlockNode* parent)
        : PrimalNode(parent, NodeType::Dino)
    {
    }

    Dino::Dino(BlockNode* parent, const JSON& in)
        : PrimalNode(parent, NodeType::Dino)
    {
        level_ = Utils::ExtractInt(in, "Level");
        is_baby_ = Utils::ExtractBool(in, "BabyDino");
        imprint_amount_ = Utils::ExtractInt(in, "ImprintAmount");
        neutered_ = Utils::ExtractBool(in, "Neutered");
        bp_ = Utils::ExtractString(in, "Blueprint");
        saddle_bp_ = Utils::ExtractString(in, "SaddleBlueprint");
        saddle_quality_ = Utils::ExtractInt(in, "SaddleQuality");
    }

    qint32 Dino::GetLevel() const
    {
        return level_;
    }

    BooleanValue Dino::GetBabyFlag() const
    {
        return is_baby_;
    }

    qint32 Dino::GetImprintAmount() const
    {
        return imprint_amount_;
    }

    BooleanValue Dino::GetNeuteredFlag() const
    {
        return neutered_;
    }

    QString Dino::GetBP() const
    {
        return bp_;
    }

    QString Dino::GetSaddleBP() const
    {
        return saddle_bp_;
    }

    qint32 Dino::GetSaddleQuality() const
    {
        return saddle_quality_;
    }

    void Dino::SetLevel(qint32 val)
    {
        level_ = val;
    }

    void Dino::SetBabyFlag(BooleanValue val)
    {
        is_baby_ = val;
    }

    void Dino::SetImprintAmount(qint32 val)
    {
        imprint_amount_ = val;
    }

    void Dino::SetNeuteredFlag(BooleanValue val)
    {
        neutered_ = val;
    }

    void Dino::SetBP(const QString& val)
    {
        bp_ = val;
    }

    void Dino::SetSaddleBP(const QString& val)
    {
        saddle_bp_ = val;
    }

    void Dino::SetSaddleQuality(qint32 val)
    {
        saddle_quality_ = val;
    }

    JSON Dino::ToJSON() const
    {
        JSON out = PrimalNode::ToJSON();
        out["Level"] = level_;
        out["BabyDino"] = Utils::PrepareBool(is_baby_);
        out["ImprintAmount"] = imprint_amount_;
        out["Neutered"] = Utils::PrepareBool(neutered_);
        out["Blueprint"] = bp_.toStdString();
        out["SaddleBlueprint"] = saddle_bp_.toStdString();
        out["SaddleQuality"] = saddle_quality_;
        return out;
    }

    bool Dino::CheckValidity() const
    {
        return (PrimalNode::CheckValidity() &&
                level_ > 0 &&
                is_baby_ != BooleanValue::None &&
                (imprint_amount_ >= 0 && imprint_amount_ <= 100) &&
                neutered_ != BooleanValue::None &&
                !bp_.isEmpty() &&
                ((!saddle_bp_.isEmpty() && saddle_quality_ > 0) || (saddle_bp_.isEmpty() && saddle_quality_ == 0)));
    }

}

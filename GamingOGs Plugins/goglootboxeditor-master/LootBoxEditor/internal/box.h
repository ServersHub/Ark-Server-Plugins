#ifndef BOX_H
#define BOX_H

#include <QMetaType>
#include <QtGlobal>
#include <vector>

#include "json_utils.h"

namespace LootBox
{

    class BlockNode;

    class Box
    {
    public:
        Box() = delete;
        Box(QString&& box_name);
        Box(QString&& box_name, const JSON& in);
        ~Box();

        QString GetBoxName() const;
        qint32 GetGamblePercent() const;
        qint32 GetGamblePrice() const;
        QString GetPermission() const;
        qint32 GetMinAmount() const;
        qint32 GetMaxAmount() const;

        std::vector<BlockNode*>& GetNodes();
        void AddNode(BlockNode* node);
        void RemoveNode(BlockNode* node);

        void SetBoxName(const QString& val);
        void SetGamblePercent(qint32 val);
        void SetGamblePrice(qint32 val);
        void SetPermission(const QString& val);
        void SetMinAmount(qint32 val);
        void SetMaxAmount(qint32 val);

        JSON ToJSON() const;
        bool CheckValidity() const;

    private:
        QString box_name_ = "";
        qint32 gamble_percent_ = 0;
        qint32 gamble_price_ = 0;
        QString permission_ = "";
        qint32 min_amount_ = 0;
        qint32 max_amount_ = 0;
        std::vector<BlockNode*> nodes_;
    };

}

Q_DECLARE_METATYPE(LootBox::Box*);
Q_DECLARE_METATYPE(LootBox::Box**);

#endif // BOX_H

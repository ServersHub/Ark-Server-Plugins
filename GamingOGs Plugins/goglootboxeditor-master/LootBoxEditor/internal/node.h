#ifndef NODE_H
#define NODE_H

#include <QtGlobal>
#include <QMetaType>
#include "json_utils.h"

namespace LootBox
{

    class Box;

    class INode
    {
    public:
        INode() = delete;
        INode(NodeType type);
        virtual ~INode() = default;

        bool IsPrimal() const;
        bool IsBlock() const;
        NodeType GetType() const;
        virtual JSON ToJSON() const = 0;
        virtual bool CheckValidity() const = 0;

    private:
        NodeType type_;
    };

    class BlockNode : public INode
    {
    public:
        BlockNode() = delete;
        BlockNode(Box* parent, NodeType type);
        BlockNode(Box* parent, NodeType type, const JSON& in);
        ~BlockNode() override = default;

        static BlockNode* Create(Box* parent, const JSON& in);
        static BlockNode* Create(Box* parent, NodeType type);

        Box* GetParent();
        QString GetDescription() const;
        qint32 GetProbability() const;

        void SetParent(Box* val);
        void SetDescription(const QString& val);
        void SetProbability(qint32 val);

        JSON ToJSON() const override;
        bool CheckValidity() const override;

    private:
        Box* parent_ = nullptr;
        QString description_ = "";
        qint32 probability_ = 0;
    };

    class PrimalNode : public INode
    {
    public:
        PrimalNode() = delete;
        PrimalNode(BlockNode* parent, NodeType type);
        ~PrimalNode() override = default;

        static PrimalNode* Create(BlockNode* parent, NodeType type, const JSON& in);
        static PrimalNode* Create(BlockNode* parent, NodeType type);

        BlockNode* GetParent();
        void SetParent(BlockNode* ptr);

        JSON ToJSON() const override;
        bool CheckValidity() const override;

    private:
        BlockNode* parent_;
    };

}

Q_DECLARE_METATYPE(LootBox::INode*);
Q_DECLARE_METATYPE(LootBox::PrimalNode*);
Q_DECLARE_METATYPE(LootBox::BlockNode*);

#endif // NODE_H

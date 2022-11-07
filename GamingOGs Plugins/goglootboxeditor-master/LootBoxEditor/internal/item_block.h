#ifndef ITEM_BLOCK_H
#define ITEM_BLOCK_H

#include <QtGlobal>
#include <QMetaType>
#include <vector>

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

    class Box;
    class Item;

    class ItemBlock : public BlockNode
    {
    public:
        ItemBlock(Box* parent);
        ItemBlock(Box* parent, const JSON& in);
        ~ItemBlock() override final;

        std::vector<Item*>& GetItems();
        void AddItem(Item* item);
        void RemoveItem(Item* item);

        JSON ToJSON() const override final;
        bool CheckValidity() const override final;

    private:
        std::vector<Item*> items_;
    };

}

Q_DECLARE_METATYPE(LootBox::ItemBlock*);

#endif // ITEM_BLOCK_H

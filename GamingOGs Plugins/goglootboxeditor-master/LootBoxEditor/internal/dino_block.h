#ifndef DINO_BLOCK_H
#define DINO_BLOCK_H

#include <QtGlobal>
#include <QMetaType>
#include <vector>

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

    class Box;
    class Dino;

    class DinoBlock : public BlockNode
    {
    public:
        DinoBlock(Box* parent);
        DinoBlock(Box* parent, const JSON& in);
        ~DinoBlock() override final;

        std::vector<Dino*>& GetDinos();
        void AddDino(Dino* dino);
        void RemoveDino(Dino* dino);

        JSON ToJSON() const override final;
        bool CheckValidity() const override final;

    private:
        std::vector<Dino*> dinos_;
    };

}

Q_DECLARE_METATYPE(LootBox::DinoBlock*);

#endif // DINO_BLOCK_H

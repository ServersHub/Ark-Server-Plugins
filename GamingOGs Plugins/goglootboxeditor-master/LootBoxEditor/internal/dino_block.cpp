#include "dino_block.h"

#include "dino.h"

namespace LootBox
{

    DinoBlock::DinoBlock(Box* parent)
        : BlockNode(parent, NodeType::DinoBlock)
    {
    }

    DinoBlock::DinoBlock(Box* parent, const JSON& in)
        : BlockNode(parent, NodeType::DinoBlock, in)
    {
        if(auto it = in.find("Dinos"); it != in.end())
        {
            for(const auto& dino : *it)
            {
                PrimalNode* dino_ptr = PrimalNode::Create(reinterpret_cast<BlockNode*>(this), NodeType::Dino, dino);
                dinos_.push_back(reinterpret_cast<Dino*>(dino_ptr));
            }
        }
    }

    DinoBlock::~DinoBlock()
    {
        for(Dino* dino : dinos_)
        {
            delete dino;
        }
    }

    std::vector<Dino*>& DinoBlock::GetDinos()
    {
        return dinos_;
    }

    void DinoBlock::AddDino(Dino* dino)
    {
        dinos_.push_back(dino);
    }

    void DinoBlock::RemoveDino(Dino* dino)
    {
        const auto it = std::find_if(dinos_.cbegin(), dinos_.cend(),
                                     [dino](const Dino* ptr) { return dino == ptr; });
        if(it != dinos_.cend())
        {
            dinos_.erase(it);
        }
    }

    JSON DinoBlock::ToJSON() const
    {
        JSON out = BlockNode::ToJSON();
        out["Dinos"] = JSON::array();
        for(const auto& dino : dinos_)
        {
            out["Dinos"].push_back(dino->ToJSON());
        }
        return out;
    }

    bool DinoBlock::CheckValidity() const
    {
        if(!BlockNode::CheckValidity())
        {
            return false;
        }

        for(const auto& dino : dinos_)
        {
            if(!dino->CheckValidity())
            {
                return false;
            }
        }

        return true;
    }

}

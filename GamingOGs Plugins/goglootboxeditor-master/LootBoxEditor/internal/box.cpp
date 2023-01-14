#include "box.h"

#include "node.h"
#include "json_utils.h"

namespace LootBox
{

    Box::Box(QString&& box_name)
        : box_name_(box_name)
    {
    }

    Box::Box(QString&& box_name, const JSON& in)
        : box_name_(box_name)
    {
        gamble_percent_ = Utils::ExtractInt(in, "GamblePercent");
        gamble_price_ = Utils::ExtractInt(in, "GamblePrice");
        permission_ = Utils::ExtractString(in, "Permission");
        min_amount_ = Utils::ExtractInt(in, "MinAmount");
        max_amount_ = Utils::ExtractInt(in, "MaxAmount");

        if(auto it = in.find("Items"); it != in.end())
        {
            for(const auto& node : *it)
            {
                BlockNode* node_ptr = BlockNode::Create(this, node);
                if(node_ptr)
                {
                    nodes_.push_back(node_ptr);
                }
            }
        }
    }

    Box::~Box()
    {
        for(BlockNode* node : nodes_)
        {
            if(node)
            {
                delete node;
            }
        }
    }

    QString Box::GetBoxName() const
    {
        return box_name_;
    }

    qint32 Box::GetGamblePercent() const
    {
        return gamble_percent_;
    }

    qint32 Box::GetGamblePrice() const
    {
        return gamble_price_;
    }

    QString Box::GetPermission() const
    {
        return permission_;
    }

    qint32 Box::GetMinAmount() const
    {
        return min_amount_;
    }

    qint32 Box::GetMaxAmount() const
    {
        return max_amount_;
    }

    std::vector<BlockNode*>& Box::GetNodes()
    {
        return nodes_;
    }

    void Box::AddNode(BlockNode* node)
    {
        nodes_.push_back(node);
    }

    void Box::RemoveNode(BlockNode* node)
    {
        const auto it = std::find_if(nodes_.cbegin(), nodes_.cend(),
                                     [node](const BlockNode* ptr) { return node == ptr; });
        if(it != nodes_.end())
        {
            nodes_.erase(it);
        }
    }

    void Box::SetBoxName(const QString& val)
    {
        box_name_ = val;
    }

    void Box::SetGamblePercent(qint32 val)
    {
        gamble_percent_ = val;
    }

    void Box::SetGamblePrice(qint32 val)
    {
        gamble_price_ = val;
    }

    void Box::SetPermission(const QString& val)
    {
        permission_ = val;
    }

    void Box::SetMinAmount(qint32 val)
    {
        min_amount_ = val;
    }

    void Box::SetMaxAmount(qint32 val)
    {
        max_amount_ = val;
    }

    JSON Box::ToJSON() const
    {
        JSON out = JSON::object();
        out["GamblePercent"] = gamble_percent_;
        out["GamblePrice"] = gamble_price_;
        out["Permission"] = permission_.toStdString();
        out["MinAmount"] = min_amount_;
        out["MaxAmount"] = max_amount_;

        out["Items"] = JSON::array();
        if(!nodes_.empty())
        {
            for(const auto& item : nodes_)
            {
                out["Items"].push_back(item->ToJSON());
            }
        }

        return out;
    }

    bool Box::CheckValidity() const
    {
        bool is_valid = true;
        if(!(gamble_percent_ > 0) || !(gamble_price_ > 0) || !(max_amount_ >= min_amount_))
        {
            is_valid = false;
        }

        for(BlockNode* node : nodes_)
        {
            if(!node->CheckValidity())
            {
                is_valid = false;
            }
        }

        return is_valid;
    }

}

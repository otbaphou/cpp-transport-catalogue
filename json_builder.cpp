#include "json_builder.h"
#include <iostream>
namespace json
{
    Node* Builder::GetLastNode()
    {
        return node_stack_.back();
    }

    Builder::BaseContext Builder::Value(Node::Value val)
    {
        if (root_.IsNull())
        {
            root_.GetValue() = val;
        }
        else
        {
            Node* last_node = GetLastNode();
            if (last_node->IsArray())
            {
                Node tmp{};
                tmp.GetValue() = val;

                last_node->AsArray().push_back(tmp);
            }

            if (last_node->IsDict())
            {
                Node tmp{};
                tmp.GetValue() = val;

                last_node->AsDict().insert({ current_key, tmp });
                key_available = false;
            }
        }
        return *this;
    }



    Builder::DictItemContext Builder::Key(std::string str)
    {
        BaseContext{ *this }.Key(std::move(str));
        return BaseContext{ *this };
    }

    Builder::DictStartContext Builder::StartDict()
    {
        BaseContext{ *this }.StartDict();
        return BaseContext{ *this };
    }

    Builder::ArrayStartContext Builder::StartArray()
    {
        BaseContext{ *this }.StartArray();
        return BaseContext{ *this };
    }

    Builder& Builder::EndDict()
    {
        BaseContext{ *this }.EndDict();
        return *this;
    }

    Builder& Builder::EndArray()
    {
        BaseContext{ *this }.EndArray();
        return *this;
    }

    Node& Builder::Build()
    {
        return this->root_;
    }
}
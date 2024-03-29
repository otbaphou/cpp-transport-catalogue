#include "json.h"

namespace json
{
    class Builder
    {
        class BaseContext;
        class DictStartContext;
        class ArrayStartContext;
        class DictItemContext;
        class DictValueContext;
        class ArrayItemContext;

    public:

        Builder() = default;

        DictItemContext Key(std::string);

        DictStartContext StartDict();
        ArrayStartContext StartArray();

        Builder& EndDict();
        Builder& EndArray();

        BaseContext Value(Node::Value val);

        Node& Build();

    private:

        Node* GetLastNode();

        bool key_available = false;
        std::string current_key = "";

        Node root_;
        std::vector<Node*> node_stack_;


        class BaseContext
        {
        public:

            BaseContext(Builder& builder)
                :builder_(builder) {}

            DictItemContext Key(std::string key)
            {
                builder_.current_key = key;
                builder_.key_available = true;
                return BaseContext{ *this };
            }

            DictStartContext StartDict()
            {

                if (builder_.root_.IsNull())
                {
                    builder_.root_.GetValue() = Dict{};
                    builder_.node_stack_.push_back(&(builder_.root_));
                }
                else
                {
                    Node* last_node = builder_.GetLastNode();

                    if (last_node->IsArray())
                    {
                        last_node->AsArray().emplace_back(Dict{});
                        builder_.node_stack_.push_back(&(last_node->AsArray().back()));
                    }

                    if (last_node->IsDict())
                    {
                        last_node->AsDict().insert({ builder_.current_key, Dict{} });
                        builder_.node_stack_.push_back(&(last_node->AsDict()[builder_.current_key]));
                        builder_.key_available = false;
                    }
                }
                return BaseContext{ *this };
            }

            ArrayStartContext StartArray()
            {
                if (builder_.root_.IsNull())
                {
                    builder_.root_.GetValue() = Array{};
                    builder_.node_stack_.push_back(&(builder_.root_));
                }
                else
                {

                    Node* last_node = builder_.GetLastNode();

                    if (last_node->IsArray())
                    {
                        last_node->AsArray().emplace_back(Array{});
                        builder_.node_stack_.push_back(&(last_node->AsArray().back()));
                    }

                    if (last_node->IsDict())
                    {

                        last_node->AsDict().insert({ builder_.current_key, Array{} });
                        builder_.node_stack_.push_back(&(last_node->AsDict()[builder_.current_key]));
                        builder_.key_available = false;
                    }
                }
                return BaseContext{ *this };
            }

            Builder& EndDict()
            {

                if (builder_.GetLastNode()->IsDict())
                {
                    builder_.node_stack_.pop_back();
                }
                else
                {
                    throw std::logic_error("Nice try. End Array next time, or maybe don't touch anything at all with those hands of yours?");
                }

                return builder_;
            }

            Builder& EndArray()
            {

                if (builder_.GetLastNode()->IsArray())
                {
                    builder_.node_stack_.pop_back();
                }
                else
                {
                    throw std::logic_error("Nice try. End Dict next time, or maybe don't touch anything at all with those hands of yours?");
                }

                return builder_;
            }

            BaseContext Value(Node::Value value)
            {
                return builder_.Value(std::move(value));
            }

            Node& Build()
            {
                if (builder_.node_stack_.size() > 0)
                    throw std::logic_error("Didn't expect me to get you here, huh?");

                return builder_.root_;
            }

        private:

            Builder& builder_;
        };

        class DictStartContext : public BaseContext
        {
        public:

            DictStartContext(BaseContext base)
                :BaseContext(base) {}

            DictStartContext StartDict() = delete;
            ArrayStartContext StartArray() = delete;

            //Builder& EndDict();
            Builder& EndArray() = delete;


            Node& Build() = delete;
        };

        class ArrayStartContext : public BaseContext
        {
        public:

            ArrayStartContext(BaseContext base)
                :BaseContext(base) {}

            DictItemContext Key(std::string) = delete;

            ArrayItemContext Value(Node::Value value)
            {
                return BaseContext::Value(std::move(value));
            }

            //DictStartContext StartDict() = delete;
            //ArrayStartContext StartArray() = delete;

            Builder& EndDict() = delete;
            //Builder& EndArray() = delete;


            Node& Build() = delete;
        };

        class DictItemContext : public BaseContext
        {
        public:

            DictItemContext(BaseContext base)
                :BaseContext(base) {}

            DictItemContext Key(std::string) = delete;

            DictValueContext Value(Node::Value value)
            {
                return BaseContext::Value(std::move(value));
            }


            //DictStartContext StartDict() = delete;
            //ArrayStartContext StartArray() = delete;

            Builder& EndDict() = delete;
            Builder& EndArray() = delete;


            Node& Build() = delete;
        };

        class DictValueContext : public BaseContext
        {
        public:

            DictValueContext(BaseContext base)
                :BaseContext(base) {}

            BaseContext Value(Node::Value value) = delete;

            DictStartContext StartDict() = delete;
            ArrayStartContext StartArray() = delete;

            //Builder& EndDict() = delete;
            Builder& EndArray() = delete;


            Node& Build() = delete;
        };

        class ArrayItemContext : public BaseContext
        {
        public:

            ArrayItemContext(BaseContext base)
                :BaseContext(base) {}

            DictItemContext Key(std::string) = delete;

            ArrayItemContext Value(Node::Value value)
            {
                return BaseContext::Value(std::move(value));
            }

            //DictStartContext StartDict() = delete;
            //ArrayStartContext StartArray() = delete;

            Builder& EndDict() = delete;
            //Builder& EndArray() = delete;


            Node& Build() = delete;
        };
    };
}
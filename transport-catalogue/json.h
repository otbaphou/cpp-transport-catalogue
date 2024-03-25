#pragma once

#include <algorithm>
#include <cmath>
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <string_view>

namespace json
{

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node
    {
    public:
        /* Реализуйте Node, используя std::variant */
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
        Node() = default;

        //Node(Value val);

        Node(int val);
        Node(double val);
        Node(bool val);
        Node(const std::string val);
        Node(std::nullptr_t val);
        Node(const Array& val);
        Node(const Dict& val);

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;

        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const Value& GetValue() const;

        bool operator==(const Node& other) const;
        bool operator!=(const Node& other) const;

    private:

        Value value_;

    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& other) const;
        bool operator!=(const Document& other) const;

    private:
        Node root_;
    };

    struct OstreamSolutionPrinter
    {
        std::ostream& out;

        void operator()(std::nullptr_t) const
        {
            out << "null";
        }

        void operator()(int i) const
        {
            out << i;
        }

        void operator()(double d) const
        {
            out << d;
        }

        void operator()(const std::string& s) const
        {
            out << '\"';
            for (const char& c : s)
            {
                if (c == '"')
                {
                    out << '\\';
                }

                if (c == '\n')
                {
                    out << "\\";
                    out << 'n';
                    continue;
                }

                if (c == '\t')
                {
                    out << "\\";
                    out << 't';
                    continue;
                }

                if (c == '\r')
                {
                    out << "\\";
                    out << 'r';
                    continue;
                }

                if (c == '\\')
                {
                    out << "\\\\";
                    continue;
                }

                out << c;

            }
            out << '\"';
        }

        void operator()(bool b) const
        {
            if (b)
                out << "true";
            else
                out << "false";
        }

        void operator()(const Array arr) const
        {
            out << '[';
            size_t size_ = arr.size();
            for (size_t i = 0; i < size_; ++i)
            {
                std::visit(OstreamSolutionPrinter{ out }, arr[i].GetValue());

                if (i != size_ - 1)
                    out << ',';
            }
            out << ']';
        }

        void operator()(const Dict dic) const
        {
            out << "{ ";
            size_t i = 0;
            size_t size_ = dic.size();
            for (const auto& entry : dic)
            {
                out << '"' << entry.first << '"' << ": ";
                std::visit(OstreamSolutionPrinter{ out }, entry.second.GetValue());
                if (i != size_ - 1)
                    out << ", ";
                ++i;
            }
            out << " }";
        }
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json
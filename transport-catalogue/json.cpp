#include "json.h"

using namespace std;

namespace json {

    namespace
    {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input)
        {
            Array result;

            if (input.peek() == std::char_traits<char>::eof())
                throw json::ParsingError("Unexpected End Of The Line!");

            for (char c; input >> c && c != ']';)
            {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            return Node(move(result));
        }

        Node ParseBool(std::istream& input) 
        {
            std::string s;
            
            for (char c; input >> c && c != ',';)
            {
                if (c == ']' || c == '}')
                {
                    input.putback(c);
                    break;
                }

                s.push_back(c);
            }
            /*char e;
            input >> e;
            std::cout << "Debeetling " << e << std::endl;
            input >> e;
            std::cout << "Debeetling " << e << std::endl;
            input >> e;
            std::cout << "Debeetling " << e << std::endl;*/
            if (s == "true")
            {
                return Node(true);
            }
            else if (s == "false") {
                return Node(false);
            }
            else {
                throw json::ParsingError("Invalid boolean value");
                //std::cout << "CULPRIT: " << s << std::endl;
            }
        }

        Node ParseNull(istream& input)
        {
            std::string s;

            for (char c; input >> c && c != ',';)
            {
                if (c == ']' || c == '}')
                {
                    input.putback(c);
                    break;
                }

                s.push_back(c);
            }

            if (s == "null")
                return Node();
            else
                throw json::ParsingError("The Argument Is Invalid!");
        }

        class ParsingError : public std::runtime_error
        {
        public:
            using runtime_error::runtime_error;
        };

        using Number = std::variant<int, double>;

        Number LoadNumber(std::istream& input)
        {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
                };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
                };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }
        /*Node LoadNum(istream& input)
        {
            bool is_double = false;
            string s;

            std::getline(input, s);

            if (s.find('.') != std::string::npos || s.find('e') != std::string::npos
                || s.find('+') != std::string::npos)
                is_double = true;

            if (is_double)
            {
                return Node(std::stod(s));
            }
            else
                return Node(std::stoi(s));
        }*/

        Node LoadString(std::istream& input) 
        {
            //std::cout << "PROCESSING STRING" << std::endl;
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw json::ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') 
                {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(s);
        }

        Node LoadDict(istream& input)
        {
            Dict result;

            if (input.peek() == std::char_traits<char>::eof())
                throw json::ParsingError("Unexpected End Of The Line!");

            for (char c; input >> c && c != '}';)
            {
                if (c == '"')
                {
                    string key = LoadString(input).AsString();
                    input >> c;
                    result.insert({ move(key), LoadNode(input) });
                }
                else
                {
                    if (c != ',' && c != ' ')
                    {
                        //throw json::ParsingError("Unexpected Argument!");
                        std::cout << "Unexpected Character: \'" << c << "\'" << std::endl;
                    }
                }
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input)
        {

            char c;
            input >> c;

            if (c == '[')
            {
                return LoadArray(input);
            }
            else if (c == '{')
            {
                return LoadDict(input);
            }
            else if (c == '"')
            {
                return LoadString(input);
            }
            else if (c == 'n')
            {
                input.putback(c);
                return ParseNull(input);
            }
            else if (c == 't' || c == 'f')
            {
                input.putback(c);
                return ParseBool(input);
            }
            else if (((c >= '0') && (c <= '9')) || c == '-')
            {
                input.putback(c);
                const auto solution = LoadNumber(input);
                if (holds_alternative<int>(solution))
                {
                    return Node(get<int>(solution));
                }
                else
                {
                    return Node(get<double>(solution));
                }
            }
            else
            {
                throw json::ParsingError("Invalid Argument!");
                //std::cout << c << std::endl;
            }
            return Node();
        }

    }  // namespace

    //Node::Node(Value val)
        //:value_(std::move(val)) {}

    Node::Node(int val)
        :value_(val) {}

    Node::Node(double val)
        :value_(val) {}

    Node::Node(bool val)
        :value_(val) {}

    Node::Node(const std::string val)
        :value_(std::move(val)) {}

    Node::Node(std::nullptr_t val)
        :value_(val) {}

    Node::Node(const Array& val)
        :value_(std::move(val)) {}

    Node::Node(const Dict& val)
        :value_(std::move(val)) {}

    bool Node::IsInt() const
    {
        return std::holds_alternative<int>(value_);
    }

    bool Node::IsDouble() const
    {
        return (std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_));
    }

    bool Node::IsPureDouble() const
    {
        return std::holds_alternative<double>(value_);
    }

    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(value_);
    }

    bool Node::IsString() const
    {
        return std::holds_alternative<std::string>(value_);
    }

    bool Node::IsNull() const
    {
        return std::holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsArray() const
    {
        return std::holds_alternative<Array>(value_);
    }

    bool Node::IsMap() const
    {
        return std::holds_alternative<Dict>(value_);
    }

    int Node::AsInt() const
    {
        if (IsInt())
            return std::get<int>(value_);
        else
            throw std::logic_error("We Don't Do That Here");
    }

    bool Node::AsBool() const
    {
        if (IsBool())
            return std::get<bool>(value_);
        else
            throw std::logic_error("We Don't Do That Here");
    }

    double Node::AsDouble() const
    {
        if (IsPureDouble())
            return std::get<double>(value_);
        if (IsDouble())
            return static_cast<double>(std::get<int>(value_));
        else
            throw std::logic_error("We Don't Do That Here");
    }

    const std::string& Node::AsString() const
    {
        if (IsString())
            return std::get<std::string>(value_);
        else
            throw std::logic_error("We Don't Do That Here");
    }

    const Array& Node::AsArray() const
    {
        if (IsArray())
            return std::get<Array>(value_);
        else
            throw std::logic_error("We Don't Do That Here");
    }

    const Dict& Node::AsMap() const
    {
        if (IsMap())
            return std::get<Dict>(value_);
        else
            throw std::logic_error("We Don't Do That Here");
    }

    const Node::Value& Node::GetValue() const
    {
        return value_;
    }

    bool Node::operator==(const Node& other) const
    {
        return this->GetValue() == other.GetValue();
    }

    bool Node::operator!=(const Node& other) const
    {
        return !(*this == other);
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document& other) const
    {
        return this->GetRoot() == other.GetRoot();
    }

    bool Document::operator!=(const Document& other) const
    {
        return !(*this == other);
    }

    Document Load(istream& input)
    {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output)
    {
        std::visit(OstreamSolutionPrinter{ output }, doc.GetRoot().GetValue());
    }

}  // namespace json4
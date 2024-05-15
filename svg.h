#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <deque>
#include <optional>
#include <variant>
#include <iomanip>

namespace svg
{

    struct Rgb
    {
        Rgb() = default;
        Rgb(uint8_t r, uint8_t g, uint8_t b)
            : red(r), green(g), blue(b) {}

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba
    {
        Rgba() = default;
        Rgba(uint8_t r, uint8_t g, uint8_t b, double a)
            : red(r), green(g), blue(b), opacity(a) {}

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1;
    };

    struct Colorout
    {
        std::ostream& out;

        void operator()(std::monostate) const { out << "none"; }

        void operator()(std::string str) const { out << str; }

        void operator()(Rgb rgb) const
        {
            out << "rgb(" << std::to_string(rgb.red) << ',' << std::to_string(rgb.green) << ',' << std::to_string(rgb.blue) << ')';
        }

        void operator()(Rgba rgba) const
        {
            out << "rgba(" << std::to_string(rgba.red) << ',' << std::to_string(rgba.green) << ',' << std::to_string(rgba.blue) << ',' << rgba.opacity << ')';
        }
    };

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

    inline const Color NoneColor{ "none" };

    enum class StrokeLineCap
    {
        BUTT = 1,
        ROUND = 2,
        SQUARE = 3,
    };

    enum class StrokeLineJoin
    {
        ARCS = 1,
        BEVEL = 2,
        MITER = 3,
        MITER_CLIP = 4,
        ROUND = 5,
    };

    inline std::ostream& operator<<(std::ostream& out, const StrokeLineCap& obj)
    {
        switch (obj)
        {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;

        case StrokeLineCap::ROUND:
            out << "round";
            break;

        case StrokeLineCap::SQUARE:
            out << "square";
            break;
        }
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& obj)
    {
        switch (obj)
        {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;

        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;

        case StrokeLineJoin::MITER:
            out << "miter";
            break;

        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;

        case StrokeLineJoin::ROUND:
            out << "round";
            break;
        }
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const Color& color)
    {
        std::visit(Colorout{ out }, color);
        return out;
    }

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    template <typename Owner>
    class PathProps
    {

    public:

        Owner& SetFillColor(Color color)
        {
            fill_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeColor(Color color)
        {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width)
        {
            stroke_width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap)
        {
            line_cap_ = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join)
        {
            line_join_ = line_join;
            return AsOwner();
        }

    protected:

        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const
        {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }
            if (stroke_width_) {
                out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
            }
            if (line_cap_) {
                out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
            }
            if (line_join_) {
                out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
            }
        }

    private:

        Owner& AsOwner()
        {
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;

        std::optional<double> stroke_width_;

        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;

    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:

        virtual void RenderObject(const RenderContext& context) const = 0;

    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle>
    {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline : public Object, public PathProps<Polyline>
    {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

        /*
         * Прочие методы и данные, необходимые для реализации элемента <polyline>
         */
    private:

        void RenderObject(const RenderContext& context) const override;

        std::deque<Point> points_;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text : public Object, public PathProps<Text>
    {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

        // Прочие данные и методы, необходимые для реализации элемента <text>

    private:

        void RenderObject(const RenderContext& context) const override;

        Point position_ = { 0, 0 };
        Point offset_ = { 0, 0 };
        uint32_t size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string text_ = "";
    };

    // Интерфейс Graphics предоставляет методы для рисования графических примитивов
    class ObjectContainer
    {
    public:

        template <typename Obj>
        void Add(const Obj obj)
        {
            AddPtr(std::make_unique<Obj>(std::move(obj)));
        }

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    protected:

        ~ObjectContainer() = default;

        std::deque<std::unique_ptr<Object>> objects_;
    };

    // Интерфейс Drawable задаёт объекты, которые можно нарисовать с помощью Graphics
    class Drawable
    {
    public:

        virtual void Draw(ObjectContainer& container) const = 0;

        virtual ~Drawable() = default;
    };

    class Document : public ObjectContainer
    {
    public:
        /*
         Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
         Пример использования:
         Document doc;
         doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // Выводит в ostream svg-представление документа
        void Render([[maybe_unused]] std::ostream& out) const;

        // Прочие методы и данные, необходимые для реализации класса Document

    };

}  // namespace svg
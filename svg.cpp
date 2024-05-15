#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const
    {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)
    {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)
    {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_;

        out << "\""sv;

        RenderAttrs(out);

        out << "/>"sv;
    }

    // ---------- Text --------------------

    Text& Text::SetPosition(Point pos)
    {
        position_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        text_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<text";

        RenderAttrs(out);

        out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << size_ << "\""sv;

        if (!font_family_.empty())
            out << " font-family=\""sv << font_family_ << "\""sv;

        if (!font_weight_.empty())
            out << " font-weight=\""sv << font_weight_ << "\""sv;

        out << ">"sv << text_ << "</text>"sv;
    }

    // ---------- Polyline ----------------

    Polyline& Polyline::AddPoint(Point point)
    {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<polyline points=\""sv;

        //for(const Point& p : points_)
        for (size_t e = 0; e < points_.size(); ++e)
        {
            out << points_[e].x << ',' << points_[e].y;
            if (e != points_.size() - 1)
                out << ' ';
        }

        out << "\""sv;

        RenderAttrs(out);

        out << "/>"sv;
    }

    // ---------- Document ----------------    

    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_.emplace_back(std::move(obj));
    }

    void ObjectContainer::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        for (const auto& obj : objects_)
        {
            RenderContext ctx(out, 2, 2);
            obj.get()->Render(ctx);
        }

        out << "</svg>"sv;
    }

}  // namespace svg  
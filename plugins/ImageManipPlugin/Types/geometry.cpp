#include "geometry.h"



double ImageManip::Types::PI()
{
    return 3.14159265358979323846;
}


ImageManip::Types::Point::Point(double x_, double y_)
{
    x = x_;
    y = y_;
}


bool ImageManip::Types::Point::operator==(const Point& other) const
{
    return (x == other.x && y == other.y);
}


ImageManip::Types::Point ImageManip::Types::Point::operator-(const Point& other) const
{
    return {x - other.x, y - other.y};
}


ImageManip::Types::Point ImageManip::Types::Point::operator+(const Point& other) const
{
    return {x + other.x, y + other.y};
}


ImageManip::Types::Point ImageManip::Types::Point::Round(unsigned int decimals) const
{
    return {ROUND(x, decimals), ROUND(y, decimals)};
}


QPoint ImageManip::Types::Point::ToQPoint() const
{
    return QPoint(ROUND(x, 0), ROUND(y, 0));
}


QPointF ImageManip::Types::Point::ToQPointF() const
{
    return QPointF(x, y);
}


ImageManip::Types::Segment::Segment(Point p1_, Point p2_)
{
    p1 = p1_;
    p2 = p2_;
}


bool ImageManip::Types::Segment::operator==(const Segment& other) const
{
    return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
}


ImageManip::Types::Triangle::Triangle(const Point& a_, const Point& b_, const Point& c_)
{
    a = a_;
    b = b_;
    c = c_;
}


bool ImageManip::Types::Triangle::operator==(const Triangle& other) const
{
    auto segments = Segments();
    auto segmentsOther = Segments();

    for (const auto& seg : Segments())
    {
        auto iter = std::find(segmentsOther.begin(), segmentsOther.end(), seg);
        if (iter == segmentsOther.end())
        {
            return false;
        }

        segmentsOther.erase(iter);
    }

    return true;
}


std::vector<ImageManip::Types::Segment> ImageManip::Types::Triangle::Segments() const
{
    return {
            Segment(a, b),
            Segment(b, c),
            Segment(c, a)
    };
}


bool ImageManip::Types::Triangle::HasSegment(const Segment& other) const
{
    auto segments = Segments();
    for (const auto& segment : segments)
    {
        if (segment == other)
        {
            return true;
        }
    }

    return false;
}


std::vector<ImageManip::Types::Point> ImageManip::Types::Triangle::Points() const
{
    return {a, b, c};
}


bool ImageManip::Types::IsInsideCircle(const Circle& circle, const Point& p)
{
    return Distance(p.x, p.y, circle.x, circle.y) <= circle.radius;
}


ImageManip::Types::Circle ImageManip::Types::CircumCircle(
        const Point& pa, const Point& pb, const Point& pc)
{
    double D = 2 * (pa.x * (pb.y -  pc.y) + pb.x * (pc.y - pa.y) + pc.x * (pa.y - pb.y));

    double centerX = 1 / D * (
            (std::pow(pa.x, 2) + std::pow(pa.y, 2)) * (pb.y - pc.y) +
            (std::pow(pb.x, 2) + std::pow(pb.y, 2)) * (pc.y - pa.y) +
            (std::pow(pc.x, 2) + std::pow(pc.y, 2)) * (pa.y - pb.y)
    );

    double centerY = 1 / D * (
            (std::pow(pa.x, 2) + std::pow(pa.y, 2)) * (pc.x - pb.x) +
            (std::pow(pb.x, 2) + std::pow(pb.y, 2)) * (pa.x - pc.x) +
            (std::pow(pc.x, 2) + std::pow(pc.y, 2)) * (pb.x - pa.x)
    );

    double radius = Distance(pa.x, pa.y, centerX,centerY);

    return {centerX, centerY, radius};
}


double ImageManip::Types::Distance(double x1, double y1, double x2, double y2)
{
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}


ImageManip::Types::Point Normalized(const ImageManip::Types::Point vector)
{
    double vNorm = std::sqrt(std::pow(vector.x, 2) + std::pow(vector.y, 2));

    return {vector.x / vNorm, vector.y / vNorm};
}


double ImageManip::Types::NormalizedAngle(
        const Point& vector1, const Point& vector2
)
{
//    double v1Length = Distance(p1.x, p1.y, p2.x, p2.y);
//    double v2Length = Distance(p1.x, p1.y, p3.x, p3.y);

    double v1Norm = std::sqrt(std::pow(vector1.x, 2) + std::pow(vector1.y, 2));
    double v2Norm = std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2));

    double scalarProduct = vector1.x * vector2.x + vector1.y * vector2.y;

    double p = scalarProduct / (v1Norm * v2Norm);

    double ac = std::acos(p);
//    return std::acos((v1.x * v2.x) + (v1.y * v2.y));
    return ac;
}


double ImageManip::Types::Angle(
        const ImageManip::Types::Point& vector1,
        const ImageManip::Types::Point& vector2
)
{
//    double v1Length = Distance(p1.x, p1.y, p2.x, p2.y);
//    double v2Length = Distance(p1.x, p1.y, p3.x, p3.y);

    double v1Norm = std::sqrt(std::pow(vector1.x, 2) + std::pow(vector1.y, 2));
    double v2Norm = std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2));

    double scalarProduct = vector1.x * vector2.x + vector1.y * vector2.y;

    double p = scalarProduct / (v1Norm * v2Norm);

    double ac = std::acos(p);
//    return std::acos((v1.x * v2.x) + (v1.y * v2.y));
    return ac;
}


double ImageManip::Types::TrigoAngle(
        const Point& vector
)
{
    double angle = Angle({1, 0}, vector);

    if (vector.y < 0)
    {
        angle = (2 * PI()) - angle;
    }

    return angle;
}


double ImageManip::Types::TrigoAngle(
        const Point& root, const Point& vector
)
{
    double angle = Angle(root + Point(1, 0), vector);

    if (vector.y < root.y)
    {
        angle = (2 * PI()) - angle;
    }

    double deg = angle * 57.2958;
    return angle;
}


ImageManip::Types::Point ImageManip::Types::AveragePoint(
        std::vector<Point> points)
{
    double sumX = 0;
    double sumY = 0;
    for (const Point& p : points)
    {
        sumX += p.x;
        sumY += p.y;
    }

    return {sumX / static_cast<double>(points.size()),
            sumY / static_cast<double>(points.size())};
}


double PointRootAngle(const ImageManip::Types::Point& root,
                      const ImageManip::Types::Point& point)
{
    ImageManip::Types::Point vector = point - root;
    ImageManip::Types::Point refVec = (root + ImageManip::Types::Point(1, 0)) - root;

    double angle = Angle(refVec, vector);
    if (point.y < root.y)
    {
        angle = 2 * ImageManip::Types::PI() - angle;
    }

    return angle;
}



void ImageManip::Types::Polygon::Sort()
{
    auto cmp = [this](const Point& lhs, const Point& rhs)
    {
        return PointRootAngle(center, lhs) < PointRootAngle(center, rhs);
    };

    std::sort(points.begin(), points.end(), cmp);
}


void ImageManip::Types::Polygon::ComputeCenter()
{
    center = AveragePoint(points);
}


size_t ImageManip::Types::Polygon::Hash() const
{
    Polygon normalized = Normalized();

    size_t hash = 0;
    for (const Point& p : normalized.points)
    {
        hash_combine(hash, p.Round(3));
    }

    return hash;
}


ImageManip::Types::Polygon ImageManip::Types::Polygon::Normalized() const
{
    Polygon p;
    p.center = {0, 0};

    for (const auto& point : points)
    {
        p.points.push_back(point - center);
    }

    return p;
}


bool ImageManip::Types::Polygon::operator==(const Polygon& other) const
{
    return (Hash() == other.Hash());
}


QPolygon ImageManip::Types::Polygon::ToQPolygon() const
{
    QPolygon p;

    for (const auto& pnt : points)
    {
        p.emplace_back(pnt.x, pnt.y);
    }

    return p;
}




size_t ImageManip::Types::HashPolygon(
        std::vector<Point> polygon)
{
    double sumX = 0;
    double sumY = 0;
    for (const auto& point : polygon)
    {
        sumX += point.x;
        sumY += point.y;
    }

    ImageManip::Types::Point center(
            sumX / (double)polygon.size(),
            sumY / (double)polygon.size()
    );

    size_t hash = 0;
    for (const auto& point : polygon)
    {
        ImageManip::Types::Point vec = Normalized(point  - center);
        hash_combine(hash, vec);
    }

    return hash;
}





void ImageManip::Types::PointHandler::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document) const
{
    Point p = std::any_cast<Point>(v);
    value.PushBack(p.x, document.GetAllocator());
    value.PushBack(p.y, document.GetAllocator());
}

std::any ImageManip::Types::PointHandler::DeserializeValue(
        const std::any& v, rapidjson::Value& value) const
{
    return Point(value[0].GetDouble(), value[1].GetDouble());
}

void ImageManip::Types::PointHandler::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document) const
{

}

std::any ImageManip::Types::PointHandler::DeserializeConstruction(
        rapidjson::Value& value) const
{
    return {};
}

std::any ImageManip::Types::PointHandler::InitValue() const
{
    return Point();
}

bool ImageManip::Types::PointHandler::CompareValue(const std::any& v1, const std::any& v2) const
{
    return std::any_cast<Point>(v1) == std::any_cast<Point>(v2);
}

std::any ImageManip::Types::PointHandler::FromPython(const boost::python::object&) const
{
    return {};
}

boost::python::object ImageManip::Types::PointHandler::ToPython(const std::any&) const
{
    return {};
}

std::any ImageManip::Types::PointHandler::CopyValue(const std::any& source) const
{
    return Point(std::any_cast<Point>(source));
}

size_t ImageManip::Types::PointHandler::Hash() const
{
    return typeid(Point).hash_code();
}

std::string ImageManip::Types::PointHandler::PythonName() const
{
    return "Point";
}

std::string ImageManip::Types::PointHandler::ApiName() const
{
    return "ImageManip::Point";
}

size_t ImageManip::Types::PointHandler::ValueHash(const std::any& val) const
{
    return std::hash<Point>()(std::any_cast<Point>(val));
}

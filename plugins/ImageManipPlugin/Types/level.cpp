#include "level.h"

#define CHECK_VALUE(var, min, max) if (var < min) var = min; else if (var > max) var = max;


ImageManip::Types::LevelMap::LevelMap(
        double low_, double high_, double mid_,
        double clampLow_, double clampHigh_
        )
{
    low = low_;
    high = high_;
    mid = mid_;
    clampLow = clampLow_;
    clampHigh = clampHigh_;
}

ImageManip::Types::LevelMap::LevelMap(const LevelMap& other)
{
    low = other.low;
    high = other.high;
    mid = other.mid;
    clampLow = other.clampLow;
    clampHigh = other.clampHigh;
}

double ImageManip::Types::LevelMap::Low() const
{
    return low;
}

void ImageManip::Types::LevelMap::SetLow(double low_)
{
    CHECK_VALUE(low_, 0, 1)

    low = low_;
    if (low > high)
    {
        high = low;
    }
}

double ImageManip::Types::LevelMap::High() const
{
    return high;
}

void ImageManip::Types::LevelMap::SetHigh(double high_)
{
    CHECK_VALUE(high_, 0, 1)

    high = high_;
    if (high < low)
    {
        low = high;
    }
}

double ImageManip::Types::LevelMap::Mid() const
{
    return mid;
}

void ImageManip::Types::LevelMap::SetMid(double mid_)
{
    CHECK_VALUE(mid_, 0, 1)

    mid = mid_;
}

double ImageManip::Types::LevelMap::ClampLow() const
{
    return clampLow;
}

void ImageManip::Types::LevelMap::SetClampLow(double clamp)
{
    CHECK_VALUE(clamp, 0, 1)

    clampLow = clamp;
}

double ImageManip::Types::LevelMap::ClampHigh() const
{
    return clampHigh;
}

void ImageManip::Types::LevelMap::SetClampHigh(double clamp)
{
    CHECK_VALUE(clamp, 0, 1)

    clampHigh = clamp;
}

bool ImageManip::Types::LevelMap::operator==(const LevelMap& other) const
{
    return (other.low == low && other.mid == mid && other.high == high &&
            other.clampLow == clampLow && other.clampHigh == clampHigh);
}



std::any ImageManip::Types::LevelMapHandler::InitValue() const
{
    return std::make_any<ImageManip::Types::LevelMap>(LevelMap());
}


std::any ImageManip::Types::LevelMapHandler::FromPython(const boost::python::object& o) const
{
    ImageManip::Types::LevelMap map_ = boost::python::extract<
            ImageManip::Types::LevelMap>(o);

    return std::make_any<ImageManip::Types::LevelMap>(map_);
}


boost::python::object ImageManip::Types::LevelMapHandler::ToPython(const std::any& v) const
{
    return boost::python::object(std::any_cast<ImageManip::Types::LevelMap>(v));
}


std::any ImageManip::Types::LevelMapHandler::CopyValue(const std::any& source) const
{
    return std::make_any<ImageManip::Types::LevelMap>(LevelMap(
            std::any_cast<ImageManip::Types::LevelMap>(source)));
}


size_t ImageManip::Types::LevelMapHandler::Hash() const
{
    return typeid(LevelMap).hash_code();
}


std::string ImageManip::Types::LevelMapHandler::PythonName() const
{
    return "LevelMap";
}


std::string ImageManip::Types::LevelMapHandler::ApiName() const
{
    return "LevelMap";
}


size_t ImageManip::Types::LevelMapHandler::ValueHash(const std::any& val) const
{
    return 0;
}


bool ImageManip::Types::LevelMapHandler::CompareValue(const std::any& v1, const std::any& v2) const
{
    return (std::any_cast<LevelMap>(v1) == std::any_cast<LevelMap>(v2));
}


void ImageManip::Types::LevelMapHandler::SerializeValue(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document) const
{
    auto level = std::any_cast<LevelMap>(v);

    rapidjson::Value& low = rapidjson::Value().SetDouble(level.Low());
    rapidjson::Value& mid = rapidjson::Value().SetDouble(level.Mid());
    rapidjson::Value& high = rapidjson::Value().SetDouble(level.High());
    rapidjson::Value& clampLow = rapidjson::Value().SetDouble(level.ClampLow());
    rapidjson::Value& clampHigh = rapidjson::Value().SetDouble(level.ClampHigh());

    value.PushBack(low.Move(), document.GetAllocator());
    value.PushBack(mid.Move(), document.GetAllocator());
    value.PushBack(high.Move(), document.GetAllocator());
    value.PushBack(clampLow.Move(), document.GetAllocator());
    value.PushBack(clampHigh.Move(), document.GetAllocator());
}

std::any ImageManip::Types::LevelMapHandler::DeserializeValue(
        const std::any& v, rapidjson::Value& value) const
{
    LevelMap level(
            value[0].GetDouble(),
            value[1].GetDouble(),
            value[2].GetDouble(),
            value[3].GetDouble(),
            value[4].GetDouble()
            );

    return std::make_any<LevelMap>(level);
}

void ImageManip::Types::LevelMapHandler::SerializeConstruction(
        const std::any& v, rapidjson::Value& value,
        rapidjson::Document& document) const
{

}

std::any ImageManip::Types::LevelMapHandler::DeserializeConstruction(
        rapidjson::Value& value) const
{
    return InitValue();
}
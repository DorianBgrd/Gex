#include "Types/geometry.h"
#include <iostream>


int main(int argc, char** argv)
{
    ImageManip::Types::Triangle t1{
            ImageManip::Types::Point(0, 0),
            ImageManip::Types::Point(0, 15),
            ImageManip::Types::Point(15, 0),
    };

    ImageManip::Types::Triangle t2{
            ImageManip::Types::Point(10, 10),
            ImageManip::Types::Point(10, 25),
            ImageManip::Types::Point(25, 10),
    };

    std::hash<ImageManip::Types::Triangle> hasher;
    assert(hasher(t1) == hasher(t2));

    ImageManip::Types::Point ref(0, 1);

    ImageManip::Types::Point v2(1, 1);
    ImageManip::Types::Point v3(1, 1.5);
    ImageManip::Types::Point v4(0, 1);
    ImageManip::Types::Point v5(-1, 1);
    ImageManip::Types::Point v6(-1, 0);
    ImageManip::Types::Point v7(-1, -1);
    ImageManip::Types::Point v8(0, -1);
    ImageManip::Types::Point v9(1, -1);

    std::cout << "Angle V2 " << ImageManip::Types::Angle(ref, v2) << std::endl;
    std::cout << "Angle V3 " << ImageManip::Types::Angle(ref, v3) << std::endl;
    std::cout << "Angle V4 " << ImageManip::Types::Angle(ref, v4) << std::endl;
    std::cout << "Angle V5 " << ImageManip::Types::Angle(ref, v5) << std::endl;
    std::cout << "Angle V6 " << ImageManip::Types::Angle(ref, v6) << std::endl;
    std::cout << "Angle V7 " << ImageManip::Types::Angle(ref, v7) << std::endl;
    std::cout << "Angle V8 " << ImageManip::Types::Angle(ref, v8) << std::endl;
    std::cout << "Angle V9 " << ImageManip::Types::Angle(ref, v9) << std::endl;
    std::cout << std::endl;
    std::cout << "Angle V2 " << ImageManip::Types::TrigoAngle(v2) << std::endl;
    std::cout << "Angle V3 " << ImageManip::Types::TrigoAngle(v3) << std::endl;
    std::cout << "Angle V4 " << ImageManip::Types::TrigoAngle(v4) << std::endl;
    std::cout << "Angle V5 " << ImageManip::Types::TrigoAngle(v5) << std::endl;
    std::cout << "Angle V6 " << ImageManip::Types::TrigoAngle(v6) << std::endl;
    std::cout << "Angle V7 " << ImageManip::Types::TrigoAngle(v7) << std::endl;
    std::cout << "Angle V8 " << ImageManip::Types::TrigoAngle(v8) << std::endl;
    std::cout << "Angle V9 " << ImageManip::Types::TrigoAngle(v9) << std::endl;


    ImageManip::Types::Triangle t3{
            ImageManip::Types::Point(10, 10),
            ImageManip::Types::Point(10, 25),
            ImageManip::Types::Point(25, 10),
    };

    ImageManip::Types::Triangle t4{
            ImageManip::Types::Point(-10, -10),
            ImageManip::Types::Point(-10, 5),
            ImageManip::Types::Point(5, -10),
    };

    auto c1 = ImageManip::Types::CircumCircle(t1.a, t1.b, t1.c);
    auto c2 = ImageManip::Types::CircumCircle(t2.a, t2.b, t2.c);
    auto c3 = ImageManip::Types::CircumCircle(t3.a, t3.b, t3.c);
    auto c4 = ImageManip::Types::CircumCircle(t4.a, t4.b, t4.c);

    size_t h1 = 0;
    hash_combine(h1, t1);
    hash_combine(h1, t2);

    size_t h2 = 0;
    hash_combine(h2, t1);
    hash_combine(h2, t2);

    assert(h1 == h2);

    size_t h3 = 0;
    hash_combine(h3, t4);
    hash_combine(h3, t3);

    assert(h3 == h1);
    assert(h3 == h2);


    ImageManip::Types::Point p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
    p1 = {0, 0};
    p2 = {5, 0};
    p3 = {5, 5};
    p4 = {-5, 5};
    p5 = {-5, 0};

    p6 = {10, 10};
//    p6 = {100, 10};
    p7 = {15, 10};
    p8 = {15, 15};
    p9 = {5, 15};
    p10 = {5, 10};

    ImageManip::Types::Polygon poly1;
    poly1.points.push_back(p1);
    poly1.points.push_back(p2);
    poly1.points.push_back(p3);
    poly1.points.push_back(p4);
    poly1.points.push_back(p5);
    poly1.center = ImageManip::Types::AveragePoint(poly1.points);

    ImageManip::Types::Polygon poly2;
    poly2.points.push_back(p6);
    poly2.points.push_back(p7);
    poly2.points.push_back(p9);
    poly2.points.push_back(p10);
    poly2.points.push_back(p8);
    poly2.center = ImageManip::Types::AveragePoint(poly2.points);

    poly1.Sort();
    poly2.Sort();

    size_t polyHash1 = poly1.Hash();
    size_t polyHash2 = poly2.Hash();

    ImageManip::Types::Polygon poly1N = poly1.Normalized();
    ImageManip::Types::Polygon poly2N = poly2.Normalized();

    assert(polyHash1 == polyHash2);


    ImageManip::Types::Polygon polyS;
    polyS.center = {5, 0};
    polyS.points = {{3, 0}, {7, 0}};

    auto polySN = polyS.Normalized();
}
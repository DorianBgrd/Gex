#include "Manip/noises.h"
#include <iostream>


int main(int argc, char** argv)
{
    ImageManip::Manip::Triangle t1{
            ImageManip::Manip::Point(0, 0),
            ImageManip::Manip::Point(0, 15),
            ImageManip::Manip::Point(15, 0),
    };

    ImageManip::Manip::Triangle t2{
            ImageManip::Manip::Point(10, 10),
            ImageManip::Manip::Point(10, 25),
            ImageManip::Manip::Point(25, 10),
    };

    std::hash<ImageManip::Manip::Triangle> hasher;
    assert(hasher(t1) == hasher(t2));

    ImageManip::Manip::Point ref(0, 1);

    ImageManip::Manip::Point v2(1, 1);
    ImageManip::Manip::Point v3(1, 1.5);
    ImageManip::Manip::Point v4(0, 1);
    ImageManip::Manip::Point v5(-1, 1);
    ImageManip::Manip::Point v6(-1, 0);
    ImageManip::Manip::Point v7(-1, -1);
    ImageManip::Manip::Point v8(0, -1);
    ImageManip::Manip::Point v9(1, -1);

    std::cout << "Angle V2 " << ImageManip::Manip::Angle(ref, v2) << std::endl;
    std::cout << "Angle V3 " << ImageManip::Manip::Angle(ref, v3) << std::endl;
    std::cout << "Angle V4 " << ImageManip::Manip::Angle(ref, v4) << std::endl;
    std::cout << "Angle V5 " << ImageManip::Manip::Angle(ref, v5) << std::endl;
    std::cout << "Angle V6 " << ImageManip::Manip::Angle(ref, v6) << std::endl;
    std::cout << "Angle V7 " << ImageManip::Manip::Angle(ref, v7) << std::endl;
    std::cout << "Angle V8 " << ImageManip::Manip::Angle(ref, v8) << std::endl;
    std::cout << "Angle V9 " << ImageManip::Manip::Angle(ref, v9) << std::endl;
    std::cout << std::endl;
    std::cout << "Angle V2 " << ImageManip::Manip::TrigoAngle(v2) << std::endl;
    std::cout << "Angle V3 " << ImageManip::Manip::TrigoAngle(v3) << std::endl;
    std::cout << "Angle V4 " << ImageManip::Manip::TrigoAngle(v4) << std::endl;
    std::cout << "Angle V5 " << ImageManip::Manip::TrigoAngle(v5) << std::endl;
    std::cout << "Angle V6 " << ImageManip::Manip::TrigoAngle(v6) << std::endl;
    std::cout << "Angle V7 " << ImageManip::Manip::TrigoAngle(v7) << std::endl;
    std::cout << "Angle V8 " << ImageManip::Manip::TrigoAngle(v8) << std::endl;
    std::cout << "Angle V9 " << ImageManip::Manip::TrigoAngle(v9) << std::endl;


    ImageManip::Manip::Triangle t3{
            ImageManip::Manip::Point(10, 10),
            ImageManip::Manip::Point(10, 25),
            ImageManip::Manip::Point(25, 10),
    };

    ImageManip::Manip::Triangle t4{
            ImageManip::Manip::Point(-10, -10),
            ImageManip::Manip::Point(-10, 5),
            ImageManip::Manip::Point(5, -10),
    };

    auto c1 = ImageManip::Manip::CircumCircle(t1.a, t1.b, t1.c);
    auto c2 = ImageManip::Manip::CircumCircle(t2.a, t2.b, t2.c);
    auto c3 = ImageManip::Manip::CircumCircle(t3.a, t3.b, t3.c);
    auto c4 = ImageManip::Manip::CircumCircle(t4.a, t4.b, t4.c);

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


    ImageManip::Manip::Point p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
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

    ImageManip::Manip::Polygon poly1;
    poly1.points.push_back(p1);
    poly1.points.push_back(p2);
    poly1.points.push_back(p3);
    poly1.points.push_back(p4);
    poly1.points.push_back(p5);
    poly1.center = ImageManip::Manip::AveragePoint(poly1.points);

    ImageManip::Manip::Polygon poly2;
    poly2.points.push_back(p6);
    poly2.points.push_back(p7);
    poly2.points.push_back(p9);
    poly2.points.push_back(p10);
    poly2.points.push_back(p8);
    poly2.center = ImageManip::Manip::AveragePoint(poly2.points);

    poly1.Sort();
    poly2.Sort();

    size_t polyHash1 = poly1.Hash();
    size_t polyHash2 = poly2.Hash();

    ImageManip::Manip::Polygon poly1N = poly1.Normalized();
    ImageManip::Manip::Polygon poly2N = poly2.Normalized();

    assert(polyHash1 == polyHash2);


    ImageManip::Manip::Polygon polyS;
    polyS.center = {5, 0};
    polyS.points = {{3, 0}, {7, 0}};

    auto polySN = polyS.Normalized();
}
#include "Point.h"
#include "Trip_point.h"
#include "Instance.h"
#include <cmath>
#include <iostream>
#include <iterator>

Point::Point()
{
    //ctor
}

 Point::Point(const Point &p):name(p.name), id(p.id),x(p.x), y(p.y), score(p.score), service_time(p.service_time), opening_time(p.opening_time),
    closing_time(p.closing_time), max_delay(p.closing_time - p.service_time){}

Point::Point(std::string n, int id, float x, float y, float s, float st, float ot, float ct)
{
    //ctor
    Point::name = n;
    Point::id = id;
    Point::x = x;
    Point::y = y;
    Point::score = s;
    Point::service_time = st;
    Point::opening_time = ot;
    Point::closing_time = ct;
    Point::max_delay = ct - st;
}

Point::~Point()
{
    //dtor
}

bool Point::operator < (const Point& poi) const{
    return (this->score > poi.score);
}

 float Point::Distance(Point &p){
    return sqrt(pow(this->Getx() -  p.Getx(),2) + pow(this->Gety() -  p.Gety(),2));
 }

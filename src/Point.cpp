#include "Point.h"
#include "Instance.h"
#include <cmath>

Point::Point()
{
    //ctor
}

 Point::Point(const Point &p):name(p.name), id(p.id),x(p.x), y(p.y), score(p.score), service_time(p.service_time), opening_time(p.opening_time),
    closing_time(p.closing_time){}

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
}

Point::~Point()
{
    //dtor
}

bool Point::operator < (const Point& poi) const{
    return (this->opening_time < poi.opening_time);
}

//Add a point into a trip
 bool Point::AddPoint(Instance &inst){
    float arriving_time;
    for(std::vector<Trip>::iterator it = inst.trips.begin(); it != inst.trips.end(); it++){
        //try to insert after start hotel
        arriving_time = it->Getstart_hotel().Getarriving_time() + inst.poi.at(it->Getstart_hotel().Getpoint_id()).Getservice_time() + this->Distance(inst.poi.at(it->Getstart_hotel().Getpoint_id()));

    }
 }

 float Point::Distance(Point &p){
    return sqrt(pow(this->Getx() -  p.Getx(),2) + pow(this->Gety() -  p.Gety(),2));
 }

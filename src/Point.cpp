#include "Point.h"
#include "Instance.h"
#include <cmath>
#include <iostream>

Point::Point()
{
    //ctor
}

 Point::Point(const Point &p):name(p.name), id(p.id),x(p.x), y(p.y), score(p.score), service_time(p.service_time), opening_time(p.opening_time),
    closing_time(p.closing_time), max_delay(p.opening_time + p.service_time){}

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
    Point::max_delay = ot + st;
}

Point::~Point()
{
    //dtor
}

bool Point::operator < (const Point& poi) const{
    return (this->opening_time < poi.opening_time);
}

//Add a point into a trip
 bool Point::InsertPoint(Instance &inst){
    float arriving_time, dist_next_pt;

    for(std::vector<Trip>::iterator it = inst.trips.begin(); it != inst.trips.end(); it++){
        //try to insert after start hotel
        arriving_time = it->Getstart_hotel().Getarriving_time() + inst.poi.at(it->Getstart_hotel().Getpoint_id()).Getservice_time() + this->Distance(inst.poi.at(it->Getstart_hotel().Getpoint_id()));
        std::cout << "Arr>:" << arriving_time << std::endl;
        if(arriving_time <= this->Getmax_delay()){
            //If you arrive earlier than POI opening you should wait it opens
            if(arriving_time < this->Getopening_time()) arriving_time = this->Getopening_time();
            dist_next_pt = this->Distance(inst.poi.at(it->points.at(0).Getpoint_id()));
            if(it->start_hotel.Getfree_time() >= arriving_time + this->Getservice_time() + dist_next_pt){
                it->points.insert(it->points.begin(), Trip_point(this->Getid(), arriving_time, it->points.at(0).Getarriving_time() - (arriving_time + this->Getservice_time())));
                it->start_hotel.Setfree_time(arriving_time - ( inst.hotels.at(it->start_hotel.Getpoint_id()).Getservice_time() ));
                std::cout << "Inserted " << inst.poi.at(this->Getid()).Getname() << std::endl;
                return true;
            }

        }
    }
    return false;
 }

 float Point::Distance(Point &p){
    return sqrt(pow(this->Getx() -  p.Getx(),2) + pow(this->Gety() -  p.Gety(),2));
 }

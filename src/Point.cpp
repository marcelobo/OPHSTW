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
    std::vector<Trip_point>::iterator next_tp;

    for(std::vector<Trip>::iterator it = inst.trips.begin(); it != inst.trips.end(); it++){
        //try to insert after start hotel
        next_tp = it->points.begin();
        arriving_time = it->Getstart_hotel().Getarriving_time() + inst.poi.at(it->Getstart_hotel().Getpoint_id()).Getservice_time() + this->Distance(inst.poi.at(it->Getstart_hotel().Getpoint_id()));
        if(arriving_time <= this->Getmax_delay()){
            //If you arrive earlier than POI opening you should wait it opens
            if(arriving_time < this->Getopening_time()) arriving_time = this->Getopening_time();
            dist_next_pt = this->Distance(inst.poi.at(it->points.at(0).Getpoint_id()));
            //If can  insert without delaying other POI visits
            if(it->start_hotel.Getfree_time() >= arriving_time + this->Getservice_time() + dist_next_pt){
                it->points.insert(it->points.begin(), Trip_point(this->Getid(), arriving_time, it->points.at(0).Getarriving_time() - (arriving_time + this->Getservice_time())));
                it->start_hotel.Setfree_time(arriving_time - ( inst.hotels.at(it->start_hotel.Getpoint_id()).Getservice_time() ));
                return true;
            }else{
                if(this->Can_insert(inst.poi, next_tp, (arriving_time + this->Getservice_time() + dist_next_pt) - it->start_hotel.Getfree_time() )){
                    it->points.insert(it->points.begin(), Trip_point(this->Getid(), arriving_time, it->points.at(0).Getarriving_time() - (arriving_time + this->Getservice_time())));
                    it->start_hotel.Setfree_time(arriving_time - ( inst.hotels.at(it->start_hotel.Getpoint_id()).Getservice_time() ));
                    return true;
                }
            }
        }

        //try to insert in the middle of the trip
        for(std::vector<Trip_point>::iterator itp = it->points.begin(); itp != it->points.end(); itp++){
            arriving_time = itp->Getarriving_time() + inst.poi.at(itp->Getpoint_id()).Getservice_time() + this->Distance(inst.poi.at(itp->Getpoint_id()));
            if(arriving_time <= this->Getmax_delay()){
                //If you arrive earlier than POI opening you should wait it opens
                if(arriving_time < this->Getopening_time()) arriving_time = this->Getopening_time();
                next_tp = itp + 1;
                dist_next_pt = this->Distance(inst.poi.at(next_tp->Getpoint_id()));
                //If can  insert without delaying other POI visits
                if(itp->Getfree_time() >= arriving_time + this->Getservice_time() + dist_next_pt){
                    it->points.insert(next_tp, Trip_point(this->Getid(), arriving_time, next_tp->Getarriving_time() - (arriving_time + this->Getservice_time())));
                    itp->Setfree_time(arriving_time - ( itp->Getarriving_time() + inst.poi.at(itp->Getpoint_id()).Getservice_time() ));
                    return true;
                }else{
                    if(this->Can_insert(inst.poi, next_tp, (arriving_time + this->Getservice_time() + dist_next_pt) - itp->Getfree_time() )){
                        it->points.insert(next_tp, Trip_point(this->Getid(), arriving_time, next_tp->Getarriving_time() - (arriving_time + this->Getservice_time())));
                        itp->Setfree_time(arriving_time - ( inst.poi.at(itp->Getpoint_id()).Getservice_time() ));
                        return true;
                    }
                }
            }
        }
    }
    return false;
 }

 float Point::Distance(Point &p){
    return sqrt(pow(this->Getx() -  p.Getx(),2) + pow(this->Gety() -  p.Gety(),2));
 }

 bool Point::Can_insert(std::vector<Point> &inst_poi, std::vector<Trip_point>::iterator &curr_trip_point, float remaining_time){
    float curr_freetime = 0;
    std::vector<Trip_point>::iterator next_tp = curr_trip_point + 1;
    //If current POI visit can be delayed
    if(curr_trip_point->Getarriving_time() + remaining_time <= inst_poi.at(curr_trip_point->Getpoint_id()).Getmax_delay() ){
        curr_freetime = curr_trip_point->Getfree_time();
        if(curr_freetime > 0 ){
            if(curr_freetime >= remaining_time){
                curr_trip_point->Setarriving_time(curr_trip_point->Getarriving_time() + remaining_time);
                curr_trip_point->Setfree_time(curr_freetime - remaining_time);
                return true;
            }else{
                if(this->Can_insert(inst_poi, next_tp, remaining_time - curr_trip_point->Getfree_time())){
                    curr_trip_point->Setarriving_time(curr_trip_point->Getarriving_time() + remaining_time);
                    curr_trip_point->Setfree_time(0);
                    return true;
                }
            }
        }else{
            if(this->Can_insert(inst_poi, next_tp, remaining_time)){
                curr_trip_point->Setarriving_time(curr_trip_point->Getarriving_time() + remaining_time);
                return true;
            }
        }
    }
    return false;
 }

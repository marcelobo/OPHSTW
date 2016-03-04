#include "Tour.h"
#include "Instance.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <vector>

Tour::Tour(){
    //ctor
}

Tour::~Tour(){
    //dtor
}

void Tour::Initialize_tour(int num_trips){
    trip_length.resize(num_trips, 0);
    trip_score.resize(num_trips, 0);
    trip.resize(num_trips);
}

void Tour::Initialize_hotels(std::vector<Point> &hotels, std::vector<float> &length){
    Trip_point tp_aux;
    int i, num_hotel = 0, num_trips = this->trip.size();
    //Add the start hotel as the start hotel of the first trip
    tp_aux = Trip_point(0, 0, length.at(0) - hotels.at(0).Getservice_time());
    this->trip.at(0).push_back(tp_aux);

    //Hotels random selection
    for(i = 1; i < num_trips; i++){
        num_hotel = rand() % hotels.size();
        //set end hotel from previous trip
        this->trip.at(i - 1).push_back(Trip_point(num_hotel, length.at(i - 1) - hotels.at(num_hotel).Getservice_time(), 0.0));
        //set start hotel from current trip
        this->trip.at(i).push_back(Trip_point(num_hotel, 0.0, length.at(i) - hotels.at(num_hotel).Getservice_time()));
        //Update trip length for previous trip
        this->Settrip_length(i - 1, hotels.at(this->Getstart_hotel(i - 1).Getpoint_id()).Distance(hotels.at(this->Getend_hotel(i - 1).Getpoint_id())));
    }

    //Add the end hotel as the end hotel of the last trip
    tp_aux = Trip_point(1, length.back() - hotels.at(1).Getservice_time(), 0);
    this->trip.back().push_back(tp_aux);
    this->Settrip_length(num_trips - 1, hotels.at(this->Getstart_hotel(num_trips - 1).Getpoint_id()).Distance(hotels.at(tp_aux.Getpoint_id())));
}

/**
  * Try to insert a given POI into best trip position
 */

 bool Tour::Insert_point(Instance &inst, int point_id){
    float arriving_time, dist_next_pt;
    int num_trip = 0, tp_pos, best_trip, best_position;
    Trip_point *next_tp, *previous_tp;
    Point insert_point = inst.poi.at(point_id), prev, next;
    float curr_insert_cost, best_insert_cost = inst.tour_length;

    // for every trip
    for(std::vector<std::vector<Trip_point> >::iterator it = this->trip.begin(); it != this->trip.end(); it++, num_trip++){
        tp_pos = 0;
        //for every point in the trip
        for(std::vector<Trip_point>::iterator itp = it->begin(); itp != it->end() - 1; itp++, tp_pos++){
            int itp_id = itp->Getpoint_id();
            if(itp == it->begin()) prev = inst.hotels.at(itp_id);
            else prev = inst.poi.at(itp_id);
            arriving_time = itp->Getarriving_time() + prev.Getservice_time() + insert_point.Distance(prev);
            if(arriving_time <= insert_point.Getmax_delay()){
                //If you arrive earlier than POI opening you should wait it opens
                if(arriving_time < insert_point.Getopening_time()) arriving_time = insert_point.Getopening_time();
                if(itp + 2 == it->end()) next = inst.hotels.at((itp + 1)->Getpoint_id());
                else next = inst.poi.at((itp + 1)->Getpoint_id());
                dist_next_pt = insert_point.Distance(next);
                curr_insert_cost = (arriving_time + dist_next_pt) - itp->Getfree_time();
                //If POI can be inserted
                if(itp->Getfree_time() >= arriving_time + insert_point.Getservice_time() + dist_next_pt ||
                this->Can_insert(inst.poi, this->trip.at(num_trip), tp_pos, (arriving_time + insert_point.Getservice_time() + dist_next_pt) - itp->Getfree_time() )){
                    if(best_insert_cost >= curr_insert_cost){
                        best_trip = num_trip;
                        best_position = tp_pos;
                        best_insert_cost = curr_insert_cost;
                    }
                }
            }
        }
    }
    if(best_insert_cost != inst.tour_length){
        std::cout << "best" << std::endl;
        previous_tp = &(this->trip.at(best_trip).at(best_position));
        next_tp = &(this->trip.at(best_trip).at(best_position + 1));
        if(best_position == 0) prev = inst.hotels.at(previous_tp->Getpoint_id());
        else prev = inst.poi.at(previous_tp->Getpoint_id());

        if(best_position == this->trip.at(best_trip).size() - 2) next = inst.hotels.at(next_tp->Getpoint_id());
        else next = inst.poi.at(next_tp->Getpoint_id());

        arriving_time = previous_tp->Getarriving_time() + prev.Getservice_time() + insert_point.Distance(prev);
        if(arriving_time < insert_point.Getopening_time()) arriving_time = insert_point.Getopening_time();
        this->trip.at(best_trip).insert(trip.at(best_trip).begin() + best_position + 1,
                Trip_point(point_id, arriving_time, next_tp->Getarriving_time() - (arriving_time + insert_point.Getservice_time())));
        previous_tp->Setfree_time(arriving_time - (previous_tp->Getarriving_time() + prev.Getservice_time()));
        this->trip_score.at(best_trip) += inst.poi.at(point_id).Getscore();
        this->trip_length.at(best_trip) += prev.Distance(inst.poi.at(point_id)) + inst.poi.at(point_id).Getservice_time() + inst.poi.at(point_id).Distance(next) - prev.Distance(next);
        return true;
    }
    return false;
 }

bool Tour::Can_insert(std::vector<Point> inst_poi, std::vector<Trip_point> &curr_trip, int tp_pos, float remaining_time){
    float curr_freetime = 0;
    if(tp_pos >= curr_trip.size() - 1) return false;
    Trip_point curr_pt = curr_trip.at(tp_pos);
    //If current POI visit can be delayed
    if(curr_trip.at(tp_pos).Getarriving_time() + remaining_time <= inst_poi.at(curr_trip.at(tp_pos).Getpoint_id()).Getmax_delay() ){
        curr_freetime = curr_trip.at(tp_pos).Getfree_time();
        if(curr_freetime > 0 ){
            if(curr_freetime >= remaining_time){
                curr_trip.at(tp_pos).Setarriving_time(curr_trip.at(tp_pos).Getarriving_time() + remaining_time);
                curr_trip.at(tp_pos).Setfree_time(curr_freetime - remaining_time);
                return true;
            }else{
                if(this->Can_insert(inst_poi, curr_trip, tp_pos + 1 , remaining_time - curr_trip.at(tp_pos).Getfree_time())){
                    curr_trip.at(tp_pos).Setarriving_time(curr_trip.at(tp_pos).Getarriving_time() + remaining_time);
                    curr_trip.at(tp_pos).Setfree_time(0);
                    return true;
                }
            }
        }else{
            if(this->Can_insert(inst_poi, curr_trip, tp_pos, remaining_time)){
                curr_trip.at(tp_pos).Setarriving_time(curr_trip.at(tp_pos).Getarriving_time() + remaining_time);
                return true;
            }
        }
    }
    return false;
 }

void Tour::Print_tour(Instance inst){
    for(int i = 0; i < inst.num_trips; i++){
        std::cout << " ---------- Trip #" << i + 1 << " ---------- " << std::endl;
        std::cout << "Current length: " << this->Gettrip_length(i) << std::endl;
        std::cout << "Current score: " << this->Gettrip_score(i) << std::endl;

        //Print start hotel
        std::cout << "Start hotel: " << inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Getname() << std::endl;
        std::cout << "\t Start time: " << this->trip.at(i).at(0).Getarriving_time() << std::endl;
        std::cout << "\t Free time: " << this->trip.at(i).at(0).Getfree_time() << std::endl;

        //Print POI
        int j = 0;
        if(this->trip.at(i).size() > 2){
            for(std::vector<Trip_point>::iterator it = this->trip.at(i).begin() + 1; it != this->trip.at(i).end() - 1; it++, j++) {
                std::cout << "POI " << j + 1 << std::endl;
                std::cout << "\t Name: " << inst.poi[it->Getpoint_id()].Getname() << std::endl;
                std::cout << "\t Start time: " << it->Getarriving_time() << std::endl;
                std::cout << "\t Free time: " << it->Getfree_time() << std::endl;
            }
        }

        //Print end hotel
        std::cout << "End hotel: " << inst.hotels[this->trip.at(i).back().Getpoint_id()].Getname() << std::endl;

    }
}

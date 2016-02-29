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
    tp_list.resize(num_trips);
    hotel_list.resize(num_trips + 1);
}

/**
  * Try to insert a given POI into best trip position
 */

 bool Tour::Insert_point(Instance &inst, int point_id){
    float arriving_time, dist_next_pt;
    int num_trip = 0, tp_pos, best_trip, best_position;
    Trip_point *next_tp, *previous_tp;
    Point insert_point = inst.poi.at(point_id);
    float curr_insert_cost, best_insert_cost = inst.tour_length;
    std::vector<Trip_point>::iterator it_best_pos;

    // for every trip
    for(std::vector<std::vector<Trip_point> >::iterator it = this->tp_list.begin(); it != this->tp_list.end(); it++, num_trip++){
        tp_pos = 0;
        //try to insert after start hotel
        previous_tp = &(this->hotel_list.at(num_trip));
        //if tp_list is empty end hotel is next point
        if(it->begin() == it->end()) next_tp = &(this->hotel_list.at(num_trip + 1));
        else next_tp = &*(it->begin());
        arriving_time = previous_tp->Getarriving_time() + inst.hotels.at(previous_tp->Getpoint_id()).Getservice_time() + insert_point.Distance(inst.hotels.at(previous_tp->Getpoint_id()));
        if(arriving_time <= insert_point.Getmax_delay()){
            //If you arrive earlier than POI opening you should wait it opens
            if(arriving_time < insert_point.Getopening_time()) arriving_time = insert_point.Getopening_time();

            if(it->begin() == it->end())    dist_next_pt = insert_point.Distance(inst.hotels.at(next_tp->Getpoint_id()));
            else    dist_next_pt = insert_point.Distance(inst.poi.at(next_tp->Getpoint_id()));

            curr_insert_cost = (arriving_time + dist_next_pt) - previous_tp->Getfree_time();
            //If can  insert without delaying other POI visits
            if(previous_tp->Getfree_time() >= arriving_time + insert_point.Getservice_time() + dist_next_pt ||
            ((next_tp != &*(it->end()) && this->Can_insert(inst.poi, this->tp_list.at(num_trip), 0, (arriving_time + insert_point.Getservice_time() + dist_next_pt) - previous_tp->Getfree_time() )))){
                if(best_insert_cost >= curr_insert_cost){
                    best_trip = num_trip;
                    best_position = 0;
                    best_insert_cost = curr_insert_cost;
                }
            }
        }

        //for every point in the trip
        for(std::vector<Trip_point>::iterator itp = it->begin(); itp != it->end(); itp++, tp_pos++){
            int itp_id = itp->Getpoint_id();
            arriving_time = itp->Getarriving_time() + inst.poi.at(itp_id).Getservice_time() + insert_point.Distance(inst.poi.at(itp_id));
            if(arriving_time <= insert_point.Getmax_delay()){
                //If you arrive earlier than POI opening you should wait it opens
                if(arriving_time < insert_point.Getopening_time()) arriving_time = insert_point.Getopening_time();
                if(itp + 1 == it->end())
                    dist_next_pt = insert_point.Distance(inst.hotels.at(this->hotel_list.at(num_trip + 1).Getpoint_id()));
                else{
                    dist_next_pt = insert_point.Distance(inst.poi.at((itp + 1)->Getpoint_id()));

                }
                curr_insert_cost =  (arriving_time + dist_next_pt) - itp->Getfree_time();
                //If POI can be inserted
                if(itp->Getfree_time() >= arriving_time + insert_point.Getservice_time() + dist_next_pt ||
                this->Can_insert(inst.poi, this->tp_list.at(num_trip), tp_pos, (arriving_time + insert_point.Getservice_time() + dist_next_pt) - itp->Getfree_time() )){
                    if(best_insert_cost >= curr_insert_cost){
                        best_trip = num_trip;
                        best_position = tp_pos + 1;
                        best_insert_cost = curr_insert_cost;
                    }
                }
            }
        }
    }
    if(best_insert_cost != inst.tour_length){
        std::cout << "Best" << std::endl;
        if(best_position == 0) previous_tp = &(this->hotel_list.at(best_trip));
        else previous_tp = &(this->tp_list.at(best_trip).at(best_position - 1));
        if(best_position == this->tp_list.at(best_trip).size()) next_tp = &(this->hotel_list.at(best_trip + 1));
        else next_tp = &(this->tp_list.at(best_trip).at(best_position));

        if(best_position == 0) arriving_time = previous_tp->Getarriving_time() + inst.hotels.at(previous_tp->Getpoint_id()).Getservice_time() + insert_point.Distance(inst.hotels.at(previous_tp->Getpoint_id()));
        else arriving_time = previous_tp->Getarriving_time() + inst.poi.at(best_position - 1).Getservice_time() + insert_point.Distance(inst.poi.at(previous_tp->Getpoint_id()));
        if(arriving_time < insert_point.Getopening_time()) arriving_time = insert_point.Getopening_time();
        this->tp_list.at(best_trip).insert(tp_list.at(best_trip).begin() + best_position,
                Trip_point(point_id, arriving_time, previous_tp->Getfree_time() - (arriving_time + insert_point.Getservice_time())));
        if(best_position == 0) previous_tp->Setfree_time(arriving_time - (previous_tp->Getarriving_time() + inst.hotels.at(previous_tp->Getpoint_id()).Getservice_time()));
        else this->tp_list.at(best_trip).at(best_position - 1).Setfree_time(arriving_time - (previous_tp->Getarriving_time() + inst.poi.at(previous_tp->Getpoint_id()).Getservice_time()));
        this->trip_score.at(best_trip) += inst.poi.at(point_id).Getscore();
        return true;
    }
    return false;
 }

bool Tour::Can_insert(std::vector<Point> inst_poi, std::vector<Trip_point> &curr_trip, int tp_pos, float remaining_time){
    float curr_freetime = 0;
    if(tp_pos >= curr_trip.size()) return false;
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

        //Print start hotel
        std::cout << "Start hotel: " << inst.hotels[this->hotel_list.at(i).Getpoint_id()].Getname() << std::endl;
        std::cout << "\t Start time: " << this->hotel_list.at(i).Getarriving_time() << std::endl;
        std::cout << "\t Free time: " << this->hotel_list.at(i).Getfree_time() << std::endl;

        //Print POI
        int j = 0;
        for(std::vector<Trip_point>::iterator it = this->tp_list.at(i).begin(); it != this->tp_list.at(i).end(); it++, j++) {
            std::cout << "POI " << j + 1 << std::endl;
            std::cout << "\t Name: " << inst.poi[it->Getpoint_id()].Getname() << std::endl;
            std::cout << "\t Start time: " << it->Getarriving_time() << std::endl;
            std::cout << "\t Free time: " << it->Getfree_time() << std::endl;
        }

        //Print end hotel
        std::cout << "End hotel: " << inst.hotels[this->hotel_list.at(i+1).Getpoint_id()].Getname() << std::endl;
    }
}

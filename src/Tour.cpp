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
    float curr_insert_cost, best_insert_cost = 99999;

    // for every trip
    for(std::vector<std::vector<Trip_point> >::iterator it = this->trip.begin(); it != this->trip.end(); it++, num_trip++){
        if(insert_point.Getname() == "C17" && num_trip == 1){
            std::cout << "COmeco insert" << std::endl;
        }
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
                curr_insert_cost = arriving_time + dist_next_pt;
                //If POI can be inserted
                if(itp->Getidle_time() >= arriving_time + insert_point.Getservice_time() + dist_next_pt ||
                this->Insert_with_delay(inst, this->trip.at(num_trip), tp_pos + 1, (arriving_time + insert_point.Getservice_time() + dist_next_pt) - itp->Getidle_time(), false)){
//                    if(this->Is_best_insert_position(inst, num_trip, tp_pos, point_id, 1, &best_insert_cost)){
//                        best_trip = num_trip;
//                        best_position = tp_pos;
//                    }
                    if(curr_insert_cost <= best_insert_cost){
                        best_insert_cost = curr_insert_cost;
                        best_trip = num_trip;
                        best_position = tp_pos;
                    }
                }
            }
        }
    }
    if(best_insert_cost != 99999){
        previous_tp = &(this->trip.at(best_trip).at(best_position));
        next_tp = &(this->trip.at(best_trip).at(best_position + 1));
        if(best_position == 0) prev = inst.hotels.at(previous_tp->Getpoint_id());
        else prev = inst.poi.at(previous_tp->Getpoint_id());

        //If inserting on penultimate position
        if(best_position == this->trip.at(best_trip).size() - 2) next = inst.hotels.at(next_tp->Getpoint_id());
        else next = inst.poi.at(next_tp->Getpoint_id());

        arriving_time = previous_tp->Getarriving_time() + prev.Getservice_time() + insert_point.Distance(prev);
        if(arriving_time < insert_point.Getopening_time()) arriving_time = insert_point.Getopening_time();
        this->Insert_with_delay(inst, this->trip.at(best_trip), best_position + 1, arriving_time + insert_point.Getservice_time() + insert_point.Distance(next)  - previous_tp->Getidle_time(), true);
        previous_tp->Setidle_time(arriving_time - (previous_tp->Getarriving_time() + prev.Getservice_time()));
        this->trip.at(best_trip).insert(trip.at(best_trip).begin() + best_position + 1,
                Trip_point(point_id, arriving_time, next_tp->Getarriving_time() - (arriving_time + insert_point.Getservice_time())));
        this->trip_score.at(best_trip) += inst.poi.at(point_id).Getscore();
        this->trip_length.at(best_trip) = this->Calculate_trip_length(best_trip, inst);
        return true;
    }
    return false;
 }

bool Tour::Insert_with_delay(Instance &inst, std::vector<Trip_point> &curr_trip, int tp_pos, float remaining_time, bool can_update){
    std::vector<Trip_point> trip_aux = curr_trip;
    Point curr = inst.poi.at(trip_aux.at(tp_pos).Getpoint_id()), next;
    float curr_idletime = 0;
    bool can_delay = false;

    if(tp_pos < trip_aux.size() - 1){
        if(tp_pos + 2 == trip_aux.size()){
            next = inst.hotels.at(trip_aux.at(tp_pos + 1).Getpoint_id());

        }else next = inst.poi.at(trip_aux.at(tp_pos + 1).Getpoint_id());
        Trip_point curr_pt = trip_aux.at(tp_pos);
        //If current POI visit can be delayed
        if(trip_aux.at(tp_pos).Getarriving_time() + remaining_time <= curr.Getmax_delay() ){
            /**Teste que verifica se extrapola horario limite da trip
            if(curr_pt.Getarriving_time() + remaining_time + curr.Getservice_time() + curr.Distance(next) < curr_trip.back().Getarriving_time()){
            */
                curr_idletime = trip_aux.at(tp_pos).Getidle_time() - curr.Distance(next);
                if(curr_idletime > 0 ){
                    if(curr_idletime >= remaining_time){
                        trip_aux.at(tp_pos).Setarriving_time(trip_aux.at(tp_pos).Getarriving_time() + remaining_time);
                        trip_aux.at(tp_pos).Setidle_time(curr.Distance(next) + curr_idletime - remaining_time);
                        can_delay = true;
                    }else{
                        if(this->Insert_with_delay(inst, trip_aux, tp_pos + 1 , remaining_time - curr_idletime, can_update)){
                            trip_aux.at(tp_pos).Setarriving_time(trip_aux.at(tp_pos).Getarriving_time() + remaining_time);
                            trip_aux.at(tp_pos).Setidle_time(curr.Distance(next));
                            can_delay = true;
                        }
                    }
                }else{
                    if(this->Insert_with_delay(inst, trip_aux, tp_pos + 1, remaining_time, can_update)){
                        trip_aux.at(tp_pos).Setarriving_time(trip_aux.at(tp_pos).Getarriving_time() + remaining_time);
                        can_delay = true;
                    }
                }
            //}
        }
    }
    if(can_delay){
            if(can_update) curr_trip = trip_aux;
            return true;
    }else return false;
 }

 float Tour::Calculate_trip_length(int num_trip, Instance &inst){
    Trip_point start_hotel, end_hotel, first_poi, last_poi;
    float length = 0;
    start_hotel = this->trip.at(num_trip).at(0);
    end_hotel = this->trip.at(num_trip).back();
    first_poi = this->trip.at(num_trip).at(1);
    last_poi = this->trip.at(num_trip).at(this->trip.at(num_trip).size() - 2);

    length = inst.hotels.at(start_hotel.Getpoint_id()).Distance(inst.poi.at(first_poi.Getpoint_id()));
    length += (last_poi.Getarriving_time() + inst.poi.at(last_poi.Getpoint_id()).Getservice_time()) - first_poi.Getarriving_time();
    length += inst.poi.at(last_poi.Getpoint_id()).Distance(inst.hotels.at(end_hotel.Getpoint_id()));
    return length;
 }

bool Tour::Is_best_insert_position(Instance &inst, int num_trip, int prev_pos, int insert_id, int type, float *best_cost){
    float insert_cost, arriving_time, dist_next;
    Trip_point  prev_tp = this->trip.at(num_trip).at(prev_pos), next_tp = this->trip.at(num_trip).at(prev_pos + 1);
    Point prev, next;
    if(prev_pos == 0) prev = inst.hotels.at(prev_tp.Getpoint_id());
    else prev = inst.poi.at(prev_tp.Getpoint_id());

    if(prev_pos == this->trip.at(num_trip).size() - 2) next = inst.hotels.at(next_tp.Getpoint_id());
    else next = inst.poi.at(next_tp.Getpoint_id());

    arriving_time = prev_tp.Getarriving_time() + prev.Getservice_time() + prev.Distance(inst.poi.at(insert_id));
    dist_next = inst.poi.at(insert_id).Distance(next);
    switch(type){
        //minimize free time left after insert
        case 1:
            insert_cost = arriving_time + dist_next - prev_tp.Getidle_time();
            if(insert_cost <= *best_cost){
                *best_cost = insert_cost;
                return true;
            }
            break;
         //maximize free time left after insert
        case 2:
            insert_cost = arriving_time + dist_next - prev_tp.Getidle_time();
            if(insert_cost >= *best_cost){
                *best_cost = insert_cost;
                return true;
            }
            break;
        //minimize inserted spent time
        case 3:
            insert_cost = arriving_time + dist_next;
            if(insert_cost <= *best_cost){
                *best_cost = insert_cost;
                return true;
            }
            break;

    }

    return false;
 }

 bool Tour::Validate_tour(Instance &inst){
    int num_trip = 0;
    Point curr, next;
    std::cout << "------------ Validation ---------------- " << std::endl;
    for(std::vector<std::vector<Trip_point> >::iterator it = this->trip.begin(); it != this->trip.end(); it++, num_trip++){
        if(this->Gettrip_length(num_trip) > inst.trip_length.at(num_trip)){
            std::cout << "Excedido tamanho maximo da trip: " << num_trip << std::endl;
        }
        for(std::vector<Trip_point>::iterator itp = it->begin(); itp != it->end() - 1; itp++){
            if(itp == it->begin()) curr = inst.hotels.at(itp->Getpoint_id());
            else curr = inst.poi.at(itp->Getpoint_id());
            if(itp + 2 == it->end()) next = inst.hotels.at( (itp+1)->Getpoint_id());
            else next = inst.poi.at( (itp+1)->Getpoint_id());
            if(itp->Getarriving_time() < curr.Getopening_time() || itp->Getarriving_time() > curr.Getmax_delay()){
                std::cout << "Horario de chegada fora do limite: " << curr.Getname() << std::endl;
                std::cout << "\tChegada: " << itp->Getarriving_time() << std::endl;
                std::cout << "\tAbertura: " << curr.Getopening_time() << std::endl;
                std::cout << "\tHorario maximo: " << curr.Getmax_delay() << std::endl;
                //return false;
            }
            //If not penultimate point
            if(curr.Distance(next) > itp->Getidle_time()){
                std::cout << "Distancia entre pontos maior que Idle time: " << curr.Getname() << " -> " << next.Getname() << std::endl;
                std::cout << "\tDistancia: " << curr.Distance(next) << std::endl;
                std::cout << "\tIdle time: " << itp->Getidle_time() << std::endl;
            }



        }
    }
 }

void Tour::Print_tour(Instance &inst){
    float total_length = 0, total_score = 0, total_idle_time = 0;
    for(int i = 0; i < inst.num_trips; i++){
        total_length += this->Gettrip_length(i);
        total_score += this->Gettrip_score(i);
        std::cout << " ---------- Trip #" << i + 1 << " ---------- " << std::endl;
        std::cout << "Current length: " << this->Gettrip_length(i) << std::endl;
        std::cout << "Current free time: " << inst.trip_length.at(i) - this->Gettrip_length(i) << std::endl;
        std::cout << "Current score: " << this->Gettrip_score(i) << std::endl;

        //Print start hotel
        std::cout << "Start hotel: " << inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Getname() << std::endl;
        std::cout << "\t Start time: " << this->trip.at(i).at(0).Getarriving_time() << std::endl;
        std::cout << "\t Idle time: " << this->trip.at(i).at(0).Getidle_time() << std::endl;
        total_idle_time += this->trip.at(i).at(0).Getidle_time();

        //Print POI
        int j = 0;
        if(this->trip.at(i).size() > 2){
            for(std::vector<Trip_point>::iterator it = this->trip.at(i).begin() + 1; it != this->trip.at(i).end() - 1; it++, j++) {
                std::cout << "POI " << j + 1 << std::endl;
                std::cout << "\t Name: " << inst.poi[it->Getpoint_id()].Getname() << std::endl;
                std::cout << "\t Start time: " << it->Getarriving_time() << std::endl;
                std::cout << "\t Idle time: " << it->Getidle_time() << std::endl;
                total_idle_time += it->Getidle_time();
            }
        }

        //Print end hotel
        std::cout << "End hotel: " << inst.hotels[this->trip.at(i).back().Getpoint_id()].Getname() << std::endl;
    }
    //Print tour data
    std::cout << std::endl << "------- Tour data -------"<< std::endl;
    std::cout << "Tour length: " << total_length << std::endl;
    std::cout << "Tour score: " << total_score << std::endl;
    std::cout << "Tour idle time: " << total_idle_time << std::endl;
    std::cout << "Tour free time: " << inst.tour_length - total_length << std::endl;
}

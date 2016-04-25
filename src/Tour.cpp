#include "Tour.h"
#include "Instance.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>

Tour::Tour(){
    //ctor
}

Tour::~Tour(){
    //dtor
}

float Tour::Gettour_score(){
    float tour_score = 0;

    for(std::vector<float>::iterator it = this->trip_score.begin(); it != this->trip_score.end(); it++){
        tour_score += *it;
    }
    return tour_score;
}

void Tour::Initialize_tour(int num_trips, int exec_time, int seed){
    int prime[10]  = { 16741, 25457, 37309, 44021, 53267, 61729, 74419, 85259, 98473, 103289};
    trip_length.resize(num_trips, 0);
    trip_score.resize(num_trips, 0);
    trip.resize(num_trips);
    if(seed == 0){
        this->seed =  prime[exec_time] % 900000 + 100000;
    }else{
        this->seed = seed;
    }
//    if(seed == 0){
//        int rand_num = pow(prime[std::rand() % 10], 17);
//        seed = (rand_num  % 90000000) + 10000000;
//    }

}

void Tour::Initialize_hotels(std::vector<Point> &hotels, std::vector<float> &length){
    Trip_point tp_aux;
    int i, num_hotel = 0, num_trips = this->trip.size();
    srand(this->seed);
    //Add the start hotel as the start hotel of the first trip
    tp_aux = Trip_point(0, 0, length.at(0) - hotels.at(0).Getservice_time());
    this->trip.at(0).push_back(tp_aux);

    //Hotels random selection
    for(i = 1; i < num_trips; i++){
        num_hotel = rand() % hotels.size();
        //set end hotel from previous trip
        if(hotels.at(this->trip.at(i - 1).at(0).Getpoint_id()).Distance(hotels.at(num_hotel)) < length.at(i - 1)){
            this->trip.at(i - 1).push_back(Trip_point(num_hotel, length.at(i - 1) - hotels.at(num_hotel).Getservice_time(), 0.0));
            //set start hotel from current trip
            this->trip.at(i).push_back(Trip_point(num_hotel, 0.0, length.at(i) - hotels.at(num_hotel).Getservice_time()));
            //Update trip length for previous trip
            this->Settrip_length(i - 1, hotels.at(this->Getstart_hotel(i - 1).Getpoint_id()).Distance(hotels.at(this->Getend_hotel(i - 1).Getpoint_id())));
        }else{
            for(int j = 0; j < hotels.size(); j++){
                if(hotels.at(this->trip.at(i - 1).at(0).Getpoint_id()).Distance(hotels.at(j)) < length.at(i - 1)){
                    this->trip.at(i - 1).push_back(Trip_point(j, length.at(i - 1) - hotels.at(j).Getservice_time(), 0.0));
                    //set start hotel from current trip
                    this->trip.at(i).push_back(Trip_point(j, 0.0, length.at(i) - hotels.at(j).Getservice_time()));
                    //Update trip length for previous trip
                    this->Settrip_length(i - 1, hotels.at(this->Getstart_hotel(i - 1).Getpoint_id()).Distance(hotels.at(this->Getend_hotel(i - 1).Getpoint_id())));
                    break;
                }
            }
        }
    }

    //Add the end hotel as the end hotel of the last trip
    tp_aux = Trip_point(1, length.back() - hotels.at(1).Getservice_time(), 0);
    this->trip.back().push_back(tp_aux);
    this->Settrip_length(num_trips - 1, hotels.at(this->Getstart_hotel(num_trips - 1).Getpoint_id()).Distance(hotels.at(tp_aux.Getpoint_id())));
}

/**
  * Try to insert a given POI into best trip position
 */
 bool Tour::Insert_point(Instance &inst, int point_id, int heuristic_num){
    float arriving_time, dist_next_pt;
    int num_trip = 0, tp_pos, best_trip, best_position;
    Trip_point *next_tp, *previous_tp;
    Point insert_point = inst.poi.at(point_id), prev, next;
    float curr_insert_cost, best_insert_cost = 0;
    bool has_best = false;
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
                if(arriving_time + insert_point.Getservice_time() + dist_next_pt < next.Getopening_time()) dist_next_pt = next.Getopening_time() - (arriving_time + insert_point.Getservice_time());

                //If POI can be inserted
                //std::cout << "Idle: " << itp->Getidle_time() << "\t Spent: " << arriving_time + insert_point.Getservice_time() + dist_next_pt << std::endl;
                if(itp->Getidle_time() >= (arriving_time - itp->Getarriving_time() + prev.Getservice_time()) + insert_point.Getservice_time() + dist_next_pt ||
                this->Insert_with_delay(inst, this->trip.at(num_trip), tp_pos + 1, (arriving_time + insert_point.Getservice_time() + dist_next_pt) - itp->Getidle_time(), false)){
                    if(this->Is_best_insert_position(inst, num_trip, tp_pos, point_id, heuristic_num, best_insert_cost, has_best)){
                        best_trip = num_trip;
                        best_position = tp_pos;
                        has_best = true;
                    }
                }
            }
        }
    }
    if(has_best){
        previous_tp = &(this->trip.at(best_trip).at(best_position));
        next_tp = &(this->trip.at(best_trip).at(best_position + 1));
        if(best_position == 0) prev = inst.hotels.at(previous_tp->Getpoint_id());
        else prev = inst.poi.at(previous_tp->Getpoint_id());

        //If inserting on penultimate position
        if(best_position == this->trip.at(best_trip).size() - 2) next = inst.hotels.at(next_tp->Getpoint_id());
        else next = inst.poi.at(next_tp->Getpoint_id());

        arriving_time = previous_tp->Getarriving_time() + prev.Getservice_time() + insert_point.Distance(prev);
        if(arriving_time < insert_point.Getopening_time()) arriving_time = insert_point.Getopening_time();
        dist_next_pt = insert_point.Distance(next);
        if(arriving_time+ insert_point.Getservice_time() + dist_next_pt < next.Getopening_time()) dist_next_pt = next.Getopening_time() - (arriving_time + insert_point.Getservice_time());
        this->Insert_with_delay(inst, this->trip.at(best_trip), best_position + 1, arriving_time + insert_point.Getservice_time() + dist_next_pt  - previous_tp->Getidle_time(), true);
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
        if(tp_pos + 2 == trip_aux.size()) next = inst.hotels.at(trip_aux.at(tp_pos + 1).Getpoint_id());
        else next = inst.poi.at(trip_aux.at(tp_pos + 1).Getpoint_id());
        Trip_point curr_pt = trip_aux.at(tp_pos);
        //If current POI visit can be delayed
        if(trip_aux.at(tp_pos).Getarriving_time() + remaining_time <= curr.Getmax_delay() ){
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

bool Tour::Is_best_insert_position(Instance &inst, int num_trip, int prev_pos, int insert_id, int type, float &best_cost, bool has_best){
    float insert_cost, arriving_time, dist_next;
    Trip_point  prev_tp = this->trip.at(num_trip).at(prev_pos), next_tp = this->trip.at(num_trip).at(prev_pos + 1);
    Point prev, next;
    if(prev_pos == 0) prev = inst.hotels.at(prev_tp.Getpoint_id());
    else prev = inst.poi.at(prev_tp.Getpoint_id());

    if(prev_pos == this->trip.at(num_trip).size() - 2) next = inst.hotels.at(next_tp.Getpoint_id());
    else next = inst.poi.at(next_tp.Getpoint_id());
    if(has_best){
        switch(type){
            //minimize idle time left after insert
            case 1:
                insert_cost = prev.Distance(inst.poi.at(insert_id)) + inst.poi.at(insert_id).Getservice_time() + inst.poi.at(insert_id).Distance(next) - prev_tp.Getidle_time();
                if(insert_cost <= best_cost){
                    best_cost = insert_cost;
                    return true;
                }
                break;
             //maximize idle time left after insert
            case 2:
                insert_cost = prev.Distance(inst.poi.at(insert_id)) + inst.poi.at(insert_id).Getservice_time() + inst.poi.at(insert_id).Distance(next) - prev_tp.Getidle_time();
                if(insert_cost >= best_cost){
                    best_cost = insert_cost;
                    return true;
                }
                break;
            //minimize inserted spent time
            case 3:
                insert_cost = prev.Distance(inst.poi.at(insert_id)) + inst.poi.at(insert_id).Getservice_time() + inst.poi.at(insert_id).Distance(next);
                if(insert_cost <= best_cost){
                    best_cost = insert_cost;
                    return true;
                }
                break;
        }
    }else{
        best_cost = insert_cost;
        return true;
    }
    return false;
 }

 bool Tour::Validate_tour(Instance &inst){
    int num_trip = 0;
    Point curr, next;
    std::cout << "------------ Validation ---------------- " << std::endl;
    for(std::vector<std::vector<Trip_point> >::iterator it = this->trip.begin(); it != this->trip.end(); it++, num_trip++){
        if(this->Gettrip_length(num_trip) > inst.trip_length.at(num_trip)){
            std::cout << "Excedido tamanho maximo da trip: " << num_trip + 1 << std::endl;
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
            }
            //If not penultimate point
            if(itp->Getidle_time() - curr.Distance(next) < this->epsilon && fabs(itp->Getidle_time() - curr.Distance(next)) > this->epsilon){
                std::cout << "Distancia entre pontos maior que Idle time: " << curr.Getname() << " -> " << next.Getname() << std::endl;
                std::cout << "\tDistancia: " << curr.Distance(next) << std::endl;
                std::cout << "\tIdle time: " << itp->Getidle_time() << std::endl;
            }
        }
    }
 }

void Tour::Print_tour(Instance &inst){
    float total_length = 0, total_score = 0, total_idle_time = 0, dist_next = 0;
    for(int i = 0; i < inst.num_trips; i++){
        total_length += this->Gettrip_length(i);
        total_score += this->Gettrip_score(i);
        std::cout << " ---------- Trip #" << i + 1 << " ---------- " << std::endl;
        std::cout << "Current length: " << this->Gettrip_length(i) << std::endl;
        std::cout << "Current free time: " << inst.trip_length.at(i) - this->Gettrip_length(i) << std::endl;
        std::cout << "Current score: " << this->Gettrip_score(i) << std::endl;

        //Print start hotel
        if(this->trip.at(i).size() > 2)
            dist_next = inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Distance(inst.poi[this->trip.at(i).at(1).Getpoint_id()]);
        else dist_next = inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Distance(inst.hotels[this->trip.at(i).at(1).Getpoint_id()]);
        std::cout << "Start hotel: " << inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Getname() << std::endl;
        std::cout << "\t Start time: " << this->trip.at(i).at(0).Getarriving_time() << std::endl;
        std::cout << "\t Distance next: " << dist_next << std::endl;
        std::cout << "\t Idle time: " << this->trip.at(i).at(0).Getidle_time() - dist_next << std::endl;
        total_idle_time += this->trip.at(i).at(0).Getidle_time() - dist_next;

        //Print POI
        int j = 0;
        if(this->trip.at(i).size() > 2){
            for(std::vector<Trip_point>::iterator it = this->trip.at(i).begin() + 1; it != this->trip.at(i).end() - 1; it++, j++) {
                if(it != this->trip.at(i).end() - 2)
                    dist_next = inst.poi[it->Getpoint_id()].Distance(inst.poi[(it+1)->Getpoint_id()]);
                else dist_next = inst.poi[it->Getpoint_id()].Distance(inst.hotels[(it+1)->Getpoint_id()]);
                std::cout << "POI " << j + 1 << std::endl;
                std::cout << "\t Name: " << inst.poi[it->Getpoint_id()].Getname() << std::endl;
                std::cout << "\t Start time: " << it->Getarriving_time() << std::endl;
                std::cout << "\t Distance next: " << dist_next << std::endl;
                std::cout << "\t Idle time: " << it->Getidle_time() - dist_next << std::endl;
                total_idle_time += it->Getidle_time() - dist_next;
            }
        }

        //Print end hotel
        std::cout << "End hotel: " << inst.hotels[this->trip.at(i).back().Getpoint_id()].Getname() << std::endl;
    }
    //Print tour data
    std::cout << std::endl << "------- Tour data -------"<< std::endl;
    std::cout << "Hotels seed: " << seed << std::endl;
    std::cout << "Tour length: " << total_length << std::endl;
    std::cout << "Tour score: " << total_score << std::endl;
    std::cout << "Tour idle time: " << total_idle_time << std::endl;
    std::cout << "Tour free time: " << inst.tour_length - total_length << std::endl;
}

void Tour::Create_Solution_file(Instance &inst, std::vector<bool> &visited_points, std::vector<Point> &sorted_points, int exec_num, float exec_time, int heuristic_num){
    int point_id = 0;
    std::ofstream solution_file, not_visited_plot, solution_plot;
    std::stringstream filename_aux;
    std::string solution_plot_filename, not_visited_plot_filename, solution_filename;
    float total_length = 0, total_score = 0, total_idle_time = 0, dist_next = 0;

    //Generate the solution file
    filename_aux << ".\\solution\\print-" << inst.name << ".exec" << exec_num << ".h" << heuristic_num << ".txt";
    solution_filename = filename_aux.str();
    solution_file.open(solution_filename.c_str());
    for(int i = 0; i < inst.num_trips; i++){
        total_length += this->Gettrip_length(i);
        total_score += this->Gettrip_score(i);
        solution_file << " ---------- Trip #" << i + 1 << " ---------- " << std::endl;
        solution_file << "Current length: " << this->Gettrip_length(i) << std::endl;
        solution_file << "Current free time: " << inst.trip_length.at(i) - this->Gettrip_length(i) << std::endl;
        solution_file << "Current score: " << this->Gettrip_score(i) << std::endl;

        //Print start hotel
        if(this->trip.at(i).size() > 2)
            dist_next = inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Distance(inst.poi[this->trip.at(i).at(1).Getpoint_id()]);
        else dist_next = inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Distance(inst.hotels[this->trip.at(i).at(1).Getpoint_id()]);
        solution_file << "Start hotel: " << inst.hotels[this->trip.at(i).at(0).Getpoint_id()].Getname() << std::endl;
        solution_file << "\t Start time: " << this->trip.at(i).at(0).Getarriving_time() << std::endl;
        solution_file << "\t Distance next: " << dist_next << std::endl;
        solution_file << "\t Idle time: " << this->trip.at(i).at(0).Getidle_time() - dist_next << std::endl;
        total_idle_time += this->trip.at(i).at(0).Getidle_time() - dist_next;

        //Print POI
        int j = 0;
        if(this->trip.at(i).size() > 2){
            for(std::vector<Trip_point>::iterator it = this->trip.at(i).begin() + 1; it != this->trip.at(i).end() - 1; it++, j++) {
                if(it != this->trip.at(i).end() - 2)
                    dist_next = inst.poi[it->Getpoint_id()].Distance(inst.poi[(it+1)->Getpoint_id()]);
                else dist_next = inst.poi[it->Getpoint_id()].Distance(inst.hotels[(it+1)->Getpoint_id()]);
                solution_file << "POI " << j + 1 << std::endl;
                solution_file << "\t Name: " << inst.poi[it->Getpoint_id()].Getname() << std::endl;
                solution_file << "\t Start time: " << it->Getarriving_time() << std::endl;
                solution_file << "\t Distance next: " << dist_next << std::endl;
                solution_file << "\t Idle time: " << it->Getidle_time() - dist_next << std::endl;
                total_idle_time += it->Getidle_time() - dist_next;
            }
        }
        //Print end hotel
        solution_file << "End hotel: " << inst.hotels[this->trip.at(i).back().Getpoint_id()].Getname() << std::endl;
    }
    //Print tour data
    solution_file << std::endl << "------- Tour data -------"<< std::endl;
    solution_file << "Hotel seed: " << seed << std::endl;
    solution_file << "Tour length: " << total_length << std::endl;
    solution_file << "Tour score: " << total_score << std::endl;
    solution_file << "Tour idle time: " << total_idle_time << std::endl;
    solution_file << "Tour free time: " << inst.tour_length - total_length << std::endl;
    solution_file << "Instance total time: " << exec_time << "ms" << std::endl;

    solution_file << " -------- Sorted Points --------- " << std::endl;
    point_id = 0;
    for(std::vector<Point>::iterator it = sorted_points.begin(); it != sorted_points.end(); it++, point_id++ ){
        solution_file << it->Getname() ;
        if(visited_points.at(point_id)) solution_file << " (V)";
        solution_file << " - ";
    }
    solution_file.close();

    //Generate the files used for plotting graphs
    filename_aux.str("");
    filename_aux << ".\\solution\\plot-" << inst.name << ".h" << heuristic_num << ".exec" << exec_num << ".dat";
    solution_plot_filename = filename_aux.str();
    solution_plot.open(solution_plot_filename.c_str());

    for(int i = 0; i < inst.num_trips; i++){
        solution_plot << inst.hotels.at(this->trip.at(i).at(0).Getpoint_id()).Getname() << "\t" << inst.hotels.at(this->trip.at(i).at(0).Getpoint_id()).Getx() << "\t" << inst.hotels.at(this->trip.at(i).at(0).Getpoint_id()).Gety() << "\t" << 0 << std::endl;
        for(std::vector<Trip_point>::iterator it = this->trip.at(i).begin() + 1; it != this->trip.at(i).end() - 1; it++) {
            solution_plot << inst.poi.at(it->Getpoint_id()).Getname() << "\t" << inst.poi.at(it->Getpoint_id()).Getx() << "\t" << inst.poi.at(it->Getpoint_id()).Gety() << "\t" << inst.poi.at(it->Getpoint_id()).Getscore() << std::endl;
        }
        solution_plot << inst.hotels.at(this->trip.at(i).back().Getpoint_id()).Getname() << "\t" << inst.hotels.at(this->trip.at(i).back().Getpoint_id()).Getx() << "\t" << inst.hotels.at(this->trip.at(i).back().Getpoint_id()).Gety() << "\t" << 0 << std::endl;
        solution_plot << std::endl;
    }
    solution_plot.close();

    filename_aux.str("");
    filename_aux << ".\\solution\\plot-" << inst.name << ".h" << heuristic_num << ".exec" << exec_num << ".notvisited.dat";
    not_visited_plot_filename = filename_aux.str();
    not_visited_plot.open(not_visited_plot_filename.c_str());
    point_id = 0;
    for(std::vector<bool>::iterator it2 = visited_points.begin(); it2 != visited_points.end(); it2++, point_id++){
        if(!(*it2)){
            not_visited_plot << sorted_points.at(point_id).Getname() << "\t" << sorted_points.at(point_id).Getx() << "\t" << sorted_points.at(point_id).Gety()  << "\t" << sorted_points.at(point_id).Getscore() << std::endl;
        }
    }
    not_visited_plot.close();
}

void Tour::Instance_Report(std::string instance_name, bool first, int heuristic, int execution, float time){
    std::ofstream report;
    int heuristic_num = 0;

    if(first){
        report.open(".\\solution\\Instance-report.csv", std::fstream::out);
        report << "Instance; Heuristic; Execution; Score; Time(ms);" << std::endl;
    }else report.open(".\\solution\\Instance-report.csv", std::fstream::app);
    report << instance_name << ";" << heuristic << ";" << execution << ";" << this->Gettour_score() << ";" << time << ";" << std::endl;
    report.close();
}

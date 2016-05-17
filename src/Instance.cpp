#include "Instance.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include "Point.h"
#include "Tour.h"

Instance::Instance(){
    //ctor
}

Instance::~Instance()
{
    //dtor
}

void Instance::Get_data_file(std::string name){
    std::ifstream instance_file;
    std::string instance_fullname, file_line, point_name;
    int file_line_position, id = 0, excluded_poi = 0;
    float lenght_aux, point_x, point_y, point_score, point_service_time, point_opening_time, point_closing_time, max_trip_length = 0;
    Point point_aux, point_aux_next;

    Instance::name = name;
    instance_fullname = filepath + name;
    //Open file
    std::cout << "Instance name: " << instance_fullname << std::endl;
    instance_file.open(instance_fullname.c_str());
    if(!instance_file.is_open()){
        std::cout << "Couldn't open instance file!!!" << std::endl;
        return ;
    }
    file_line_position = 1;
    while (getline(instance_file, file_line)){
        if(!file_line.empty()){
            std::istringstream iss(file_line);
            switch(file_line_position){//get a line from file, convert to an int and put on correct variable
                case 1://get vertices, extra hotels and trips
                    iss >> num_vertices >> num_hotels >> num_trips;
                    num_vertices -= 1;
                    num_hotels += 2;
                    poi.reserve(num_vertices);
                    hotels.reserve(num_hotels + 2);
                    trip_length.reserve(num_trips);
                    break;
                case 2://tour maximum length
                    iss >> tour_length;
                    break;
                case 3://each trip length
                    while (iss >> lenght_aux){
                        trip_length.push_back(lenght_aux);
                        if(lenght_aux > max_trip_length) max_trip_length = lenght_aux;
                    }
                    break;
                default://other values
                    //other hotels
                    if(file_line_position == 3 + num_hotels + 1) id = 0;
                    if(file_line_position <= 3 + num_hotels){
                        iss >> point_name >> point_x >> point_y >> point_score >> point_service_time >> point_opening_time >> point_closing_time;
                        point_aux = Point(point_name, id, point_x, point_y, point_score, point_service_time, point_opening_time, point_closing_time);
                        hotels.push_back(point_aux);
                        id++;
                    }else{
                        //points of interest
                        iss >> point_name >> point_x >> point_y >> point_score >> point_service_time >> point_opening_time >> point_closing_time;
                        point_aux = Point(point_name, id, point_x, point_y, point_score, point_service_time, point_opening_time, point_closing_time);
                        if(point_opening_time == 0 && point_closing_time == 0){
                            if(point_score == 0){
                                excluded_poi++;
                                this->num_vertices--;
                                break;
                            }else{
                                bool found_hotel = false;
                                for(std::vector<Point>::iterator ith = hotels.begin(); ith != hotels.end(); ith++){
                                    if(ith->Distance(point_aux) < point_aux.Getmax_delay()){
                                        if(point_opening_time + point_service_time < max_trip_length){
                                            poi.push_back(point_aux);
                                            found_hotel = true;
                                            id++;
                                            break;
                                        }
                                    }
                                }
                                if(!found_hotel){
                                    excluded_poi++;
                                    this->num_vertices--;
                                }
                            }
                        }else{
                            if(point_opening_time + point_service_time < max_trip_length){
                                poi.push_back(point_aux);
                                id++;
                            }else{
                                excluded_poi++;
                                this->num_vertices--;
                            }
                        }
                    }
                    break;
            }

            file_line_position++;
            file_line.clear();
        }
    }
    instance_file.close();
    std::cout << "POI impossiveis: " << excluded_poi << std::endl;
    //Calculate distance between points
    int tot_vert = this->num_hotels + this->num_vertices;
    Point_distance.resize(tot_vert);
    for(int i = 0; i < tot_vert; i++){
        Point_distance.at(i).resize(tot_vert);
        for(int j = 0; j < tot_vert; j++){
            point_aux = (i < this->num_hotels)? this->hotels.at(i): this->poi.at(i - this->num_hotels);
            point_aux_next = (j < this->num_hotels)? this->hotels.at(j): this->poi.at(j - this->num_hotels);
            Point_distance.at(i).at(j) = point_aux.Distance(point_aux_next);
        }
    }
}

struct trip_length_id{
    int id;
    float length;
    bool operator < (const trip_length_id& tli) const{ return (this->length > tli.length);}
    trip_length_id(){}
    trip_length_id(int i, float l):id(i),length(l){}
};

void Instance::Generate_hotels_pairs(){
    int i = 0, j = 0, k = 0;
    std::vector<short> prev_end_hotel, curr_start_hotel;
    std::vector<float> hotel_aux;
    clock_t start_viable, end_viable, start_cleaning, end_cleaning, start_calculation, end_calculation;

    //creating the viable hotel graph
    start_viable = clock();
    hotel_aux.resize(this->num_hotels, 0);
    prev_end_hotel.resize(this->num_hotels);
    viable_hotel_pair.resize(this->num_trips);
    viable_hotel_pair.at(0).resize(this->num_hotels, hotel_aux);
    //first trip
    //the start hotel will always be the H0
    for(i = 0; i < this->num_hotels; i++){
        if(this->Point_distance.at(0).at(i) <= this->trip_length.at(0)){
            viable_hotel_pair.at(0).at(0).at(i) = 1;
            prev_end_hotel.at(i) = 1;
        }else{
            viable_hotel_pair.at(0).at(0).at(i) = 0;
            prev_end_hotel.at(i) = 0;
        }
    }

    k = 1;
    //For each trip
    //This starts on the second  trip, because the first was already analyzed
    //ends on the next to last, because last is analyzed later
    for(std::vector<std::vector< std::vector<float> > >::iterator it = viable_hotel_pair.begin() + 1; it != viable_hotel_pair.end() - 1; it++, k++){
        i = 0;
        it->resize(this->num_hotels);
        // For each start hotel
        for(std::vector< std::vector<float> >::iterator it2 = it->begin(); it2 != it->end(); it2++, i++){
            it2->resize(this->num_hotels, 0);
            //If it's a viable end hotel for the previous trip
            if(prev_end_hotel.at(i)){
                j = 0;
                for(std::vector<float>::iterator it3 = it2->begin(); it3 != it2->end(); it3++, j++){
                    if(this->Point_distance.at(i).at(j) <= this->trip_length.at(k)){
                        *it3 = 1;
                    }else{
                        *it3 = 0;
                    }
                }
            }
        }
    }

    //Analyzing last trip
    k = this->num_trips - 1;
    viable_hotel_pair.at(k).resize(this->num_hotels);
    for(i = 0; i < this->num_hotels; i++){
        viable_hotel_pair.at(k).at(i).resize(this->num_hotels, 0);
        if(prev_end_hotel.at(i)){
            if(this->Point_distance.at(i).at(1) <= this->trip_length.at(k)){
                viable_hotel_pair.at(k).at(i).at(1) = 1;
            }else{
                viable_hotel_pair.at(k).at(i).at(1) = 0;
            }
        }
    }
    end_viable = clock();
//    std::cout << "end viable" << std::endl;
//    std::cout.precision(1);
//     for(k = 0; k < this->num_trips; k++){
//        std::cout << "Trip #" << k << std::endl;
//        for(i = 0; i < this->num_hotels; i++){
//            for(j = 0; j < this->num_hotels; j++){
//                std::cout << viable_hotel_pair.at(k).at(i).at(j) << "\t";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//    }


    //cleaning the viable hotel graph
    // look on every ending hotel, if it is a starting for the next trip, if not, remove every edge ending on it
    start_cleaning = clock();
    int i2, j2, k2, cleaned = 0;
    bool has_edge = false, next_trip_edge = false;
    for(k = this->num_trips - 2; k >= 0; k--){
        for(j = 0; j < this->num_hotels; j++){
            for(i = 0; i < this->num_hotels; i++){
                if(viable_hotel_pair.at(k).at(i).at(j)){
                    //search for edge on next trip
                    k2 = k + 1;
                    i2 = j;
                    for(j2 = 0; j2 < this->num_hotels; j2++){
                        if(viable_hotel_pair.at(k2).at(i2).at(j2)){
                            next_trip_edge = true;
                            cleaned++;
                            break;
                        }
                    }
                    //if there isn't any path from ending hotel on  next trip, erase all paths arriving at this hotel on current trip
                    if(!next_trip_edge){
                        for(i2 = 0; i2 < this->num_hotels; i2++){
                            viable_hotel_pair.at(k).at(i2).at(j) = 0;
                        }
                    }
                    break;
                }
            }
        }
    }
    end_cleaning = clock();
//    std::cout << "-********************* After cleaning **********************************----" << std::endl;
//    std::cout << "Cleaned: " << cleaned << std::endl;
//     for(k = 0; k < this->num_trips; k++){
//        std::cout << "Trip #" << k << std::endl;
//        for(i = 0; i < this->num_hotels; i++){
//            for(j = 0; j < this->num_hotels; j++){
//                std::cout << viable_hotel_pair.at(k).at(i).at(j) << "\t";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//    }
//    std::cout.precision(5);

    //changing the viable hotel graph for score hotel graph
    start_calculation = clock();
    Tour hotel_pair_trip;
    std::vector<Point> sorted_points(this->poi);
    std::sort(sorted_points.begin(),sorted_points.end());
    std::vector<Point>::iterator itp;
    std::vector<trip_length_id> sorted_trip_length;
    trip_length_id aux_tli;

    i = 0;
    for(std::vector<float>::iterator it = this->trip_length.begin(); it != this->trip_length.end(); it++, i++){
        aux_tli = trip_length_id(i, *it);
        sorted_trip_length.push_back(aux_tli);
    }

    std::sort(sorted_trip_length.begin(), sorted_trip_length.end());

    for(std::vector<trip_length_id>::iterator it = sorted_trip_length.begin(); it != sorted_trip_length.end(); it++, i++){
        std::cout << "ID #" << it->id << "\tLength: " << it->length << std::endl;
    }

    for(k = 0; k < this->num_trips; k++){
        //if first trip
        if(k == 0){
            //just check first hotel
            for(j = 0; j < this->num_hotels; j++){
                //if hotel J is a viable end hotel
                if(viable_hotel_pair.at(0).at(0).at(j)){
                    hotel_pair_trip = Tour();
                    hotel_pair_trip.Initialize_tour(1, 1, 0);
                    hotel_pair_trip.Setstart_hotel(Trip_point(0, 0, this->trip_length.at(0) - this->hotels.at(0).Getservice_time() - this->hotels.at(j).Getservice_time()), 0);
                    hotel_pair_trip.Setend_hotel(Trip_point(j, this->trip_length.at(0) - this->hotels.at(j).Getservice_time(), 0), 0);
                    for(itp = sorted_points.begin(); itp != sorted_points.end(); itp++){
                        hotel_pair_trip.Insert_point(*this, itp->Getid(), 1);
                    }
                    viable_hotel_pair.at(0).at(0).at(j) = hotel_pair_trip.Gettrip_score(0);
                }
            }
        }else{
            //last trip
            if(k == this->num_trips - 1){
                //just check start hotel
                // end hotel will always be H1
                for(i = 0; i < this->num_hotels; i++){
                    if(viable_hotel_pair.at(k).at(i).at(1)){
                        hotel_pair_trip = Tour();
                        hotel_pair_trip.Initialize_tour(1, 1, 0);
                        hotel_pair_trip.Setstart_hotel(Trip_point(i, 0, this->trip_length.at(k) - this->hotels.at(i).Getservice_time() - this->hotels.at(1).Getservice_time()), 0);
                        hotel_pair_trip.Setend_hotel(Trip_point(1, this->trip_length.at(k) - this->hotels.at(1).Getservice_time(), 0), 0);
                        for(itp = sorted_points.begin(); itp != sorted_points.end(); itp++){
                            hotel_pair_trip.Insert_point(*this, itp->Getid(), 1);
                        }
                        viable_hotel_pair.at(k).at(i).at(1) = hotel_pair_trip.Gettrip_score(0);
                    }
                }
            }else{
                // For each start hotel
                for(i = 0; i < this->num_hotels; i++){
                    for(j = 0; j < this->num_hotels; j++){
                        if(viable_hotel_pair.at(k).at(i).at(j)){
                            hotel_pair_trip = Tour();
                            hotel_pair_trip.Initialize_tour(1, 1, 0);
                            hotel_pair_trip.Setstart_hotel(Trip_point(i, 0, this->trip_length.at(k) - this->hotels.at(i).Getservice_time() - this->hotels.at(j).Getservice_time()), 0);
                            hotel_pair_trip.Setend_hotel(Trip_point(j, this->trip_length.at(k) - this->hotels.at(j).Getservice_time(), 0), 0);
                            for(itp = sorted_points.begin(); itp != sorted_points.end(); itp++){
                                hotel_pair_trip.Insert_point(*this, itp->Getid(), 1);
                            }
                            viable_hotel_pair.at(k).at(i).at(j) = hotel_pair_trip.Gettrip_score(0);
                        }
                    }
                }
            }
        }
    }
    end_calculation = clock();
//    //print
//    for(k = 0; k < this->num_trips; k++){
//        std::cout << "Trip #" << k << std::endl;
//        for(i = 0; i < this->num_hotels; i++){
//            for(j = 0; j < this->num_hotels; j++){
//                std::cout << viable_hotel_pair.at(k).at(i).at(j) << "\t";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//    }

    std::cout << "------------------ Hotel Generation Time ------------------" << std::endl;
    std::cout << "Viable: " << (end_viable - start_viable) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
    std::cout << "Cleaning: " << (end_cleaning - start_cleaning) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
    std::cout << "Calculation: " << (end_calculation - start_calculation) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
}

void Instance::Calculate_hotel_zone(){
    int i, j, k, countzone;
    float max_length = 0, sum_zone = 0;

    for(i = 0; i < this->num_trips; i++){
        if(max_length < this->trip_length.at(i)) max_length += this->trip_length.at(i);
    }
    max_length /= this->num_trips;
    hotel_pair_zone.resize(this->num_hotels);
    for(i = 0; i < this->num_hotels; i++){
        hotel_pair_zone.at(i).resize(this->num_hotels, 0);
        for(j = 0; j < this->num_hotels; j++){
            countzone = 0;
            for(k = 0; k < this->num_vertices; k++){
                if(this->Point_distance.at(i).at(this->num_hotels + k) + this->Point_distance.at(this->num_hotels + k).at(j) < max_length){
                    sum_zone += this->poi.at(k).Getscore();
                    countzone++;
                }
            }
            hotel_pair_zone.at(i).at(j) = sum_zone;
        }
    }

    //print
//    for(i = 0; i < this->num_hotels; i++){
//        for(j = 0; j < this->num_hotels; j++){
//            std::cout << hotel_pair_zone.at(i).at(j) << "/" << countzone << "\t";
//        }
//        std::cout << std::endl;
//    }
}

void Instance::Best_hotel_sequence(){
    int i, k, j, best_hotel, best_previous_hotel;
    float best_hotel_score = 0;
    std::vector< std::vector<short> > count_pairs;
    std::vector<int> hotel_sequence(this->num_trips + 1, 0);

    count_pairs.resize(this->num_hotels);
    for(i = 0; i < this->num_hotels; i++){
        count_pairs.at(i).resize(this->num_hotels, 0);
    }

    //Set start and end hotel for this tour
    hotel_sequence.front() = 0;
    hotel_sequence.back() = 1;
    //select best end hotel for trip 1
    for(j = 0; j < this->num_hotels; j++){
        if(best_hotel_score < this->viable_hotel_pair.at(0).at(0).at(j)){
            best_hotel_score = this->viable_hotel_pair.at(0).at(0).at(j);
            best_hotel = j;
        }
    }

    hotel_sequence.at(1) = best_hotel;
    count_pairs.at(0).at(best_hotel)++;

//    for(k = 1; k < this->num_trips; k++){
//        for(j = 0; j < this->num_hotels; j++){
//            if(best_hotel_score < this->viable_hotel_pair.at(k).at(0) ){
//
//            }
//        }
//
//    }

}

void Instance::Print_data(){
    std::cout << "Vertices: " << this->num_vertices << "\t Hotels: " << this->num_hotels <<
            "\t Trips: " << this->num_trips << "\t Tour length: " << this->tour_length << std::endl;
}

void Instance::Print_POI(){
    for(int i = 0; i < this->num_vertices; i++){
        std::cout << "POI " << i + 1 << ": \n";
        std::cout << "\t ID: " << poi.at(i).Getid() << std::endl;
        std::cout << "\t Name: " << poi.at(i).Getname() << std::endl;
        std::cout << "\t X: " << poi.at(i).Getx() << std::endl;
        std::cout << "\t Y: " << poi.at(i).Gety() << std::endl;
        std::cout << "\t Score: " << poi.at(i).Getscore() << std::endl;
        std::cout << "\t Service time: " << poi.at(i).Getservice_time() << std::endl;
         std::cout << "\t Maximum delay: " << poi.at(i).Getmax_delay() << std::endl;
        std::cout << "\t Opening time: " << poi.at(i).Getopening_time() << std::endl;
        std::cout << "\t Closing time: " << poi.at(i).Getclosing_time() << std::endl;
    }
    std::cout << "\n";
}

void Instance::Print_hotels(){
    for(int i = 0; i < this->num_hotels; i++){
        std::cout << "Hotel " << i + 1 << ": \n";
        std::cout << "\t ID: " << hotels.at(i).Getid() << std::endl;
        std::cout << "\t Name: " << hotels.at(i).Getname() << std::endl;
        std::cout << "\t X: " << hotels.at(i).Getx() << std::endl;
        std::cout << "\t Y: " << hotels.at(i).Gety() << std::endl;
        std::cout << "\t Score: " << hotels.at(i).Getscore() << std::endl;
        std::cout << "\t Service time: " << hotels.at(i).Getservice_time() << std::endl;
        std::cout << "\t Opening time: " << hotels.at(i).Getopening_time() << std::endl;
        std::cout << "\t Closing time: " << hotels.at(i).Getclosing_time() << std::endl;
    }
    std::cout << "\n";
}

void Instance::Print_trip_lengths(){
    for(int i = 0; i < Instance::num_trips; i++){
        std::cout << "Trip " << i + 1 << ": " << trip_length.at(i) << " ";
    }
    std::cout << "\n";
}

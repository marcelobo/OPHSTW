#include "Instance.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Point.h"

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
    Point point_aux;

    Instance::name = name;
    instance_fullname = filepath + name;
    //Open file
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
                    num_vertices -= 2;
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
                    }else{
                        //points of interest
                        iss >> point_name >> point_x >> point_y >> point_score >> point_service_time >> point_opening_time >> point_closing_time;
                        point_aux = Point(point_name, id, point_x, point_y, point_score, point_service_time, point_opening_time, point_closing_time);
                        if(point_opening_time == 0 && point_closing_time == 0){
                            if(point_score == 0){
                                excluded_poi++;
                                break;
                            }else{
                                bool found_hotel = false;
                                for(std::vector<Point>::iterator ith = hotels.begin(); ith != hotels.end(); ith++){
                                    if(ith->Distance(point_aux) < point_aux.Getmax_delay()){
                                        if(point_opening_time + point_service_time < max_trip_length){
                                            poi.push_back(point_aux);
                                            found_hotel = true;
                                            break;
                                        }
                                    }
                                }
                                if(!found_hotel)excluded_poi++;
                            }
                        }else{
                            if(point_opening_time + point_service_time < max_trip_length){
                                poi.push_back(point_aux);
                            }
                        }
                    }
                    id++;
                    break;
            }

            file_line_position++;
            file_line.clear();
        }
    }
    instance_file.close();
    std::cout << "POI impossiveis: " << excluded_poi << std::endl;
}

void Instance::Print_data(){
    std::cout << "Vertices: " << Instance::num_vertices << "\t Hotels: " << Instance::num_hotels <<
            "\t Trips: " << Instance::num_trips << "\t Tour length: " << Instance::tour_length << std::endl;
}

void Instance::Print_POI(){
    for(int i = 0; i < Instance::num_vertices; i++){
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
    for(int i = 0; i < Instance::num_hotels; i++){
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

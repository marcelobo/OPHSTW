#include "Trip.h"
#include "Instance.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <vector>

Trip::Trip()
{
    //ctor
}

Trip::~Trip()
{
    //dtor
}

void Trip::Generate_trip(Instance &inst, std::vector<Point> &sorted_points, std::vector<bool> &visited_points, int num_trip){
    float length = 0, dist_previous, dist_end_hotel;

    //Add POI's
    for(int j = 0; j < visited_points.size(); j++){
        if(!visited_points.at(j)){//If POI wasn't already added
            if(this->points.size() == 0){//if it's adding the first POI
                //Calculates the distance from first point to  start hotel
                dist_previous = sqrt(
                    pow( inst.hotels.at(inst.trips.at(num_trip).start_hotel.Getpoint_id()).Getx() - sorted_points.at(j).Getx(),2) +
                    pow( inst.hotels.at(inst.trips.at(num_trip).start_hotel.Getpoint_id()).Gety() - sorted_points.at(j).Gety(),2)
                    );
            }else{
                dist_previous = sqrt(
                    pow( inst.poi.at(inst.trips.at(num_trip).points.back().Getpoint_id()).Getx() - sorted_points.at(j).Getx(),2) +
                    pow( inst.poi.at(inst.trips.at(num_trip).points.back().Getpoint_id()).Gety() - sorted_points.at(j).Gety(),2)
                    );
            }

            dist_end_hotel = sqrt(
                pow(sorted_points.at(j).Getx() -  inst.hotels.at(inst.trips.at(num_trip).end_hotel.Getpoint_id()).Getx(),2) +
                pow(sorted_points.at(j).Gety() -  inst.hotels.at(inst.trips.at(num_trip).end_hotel.Getpoint_id()).Gety(),2)
                );

            if(length + dist_previous + dist_end_hotel <= inst.trip_length[num_trip] &&
               length + dist_previous <= sorted_points.at(j).Getclosing_time() - sorted_points.at(j).Getservice_time()){
                //if arriving earlier
                if(length + dist_previous < sorted_points.at(j).Getopening_time()) length = sorted_points.at(j).Getopening_time();
                else length += dist_previous;

                this->points.push_back(Trip_point(sorted_points.at(j).Getid(), length, inst.trip_length[num_trip] - length));
                length += sorted_points.at(j).Getservice_time();
                if(this->points.size() == 1)//First POI (must update start hotel)
                    this->start_hotel.Setfree_time(this->points.back().Getarriving_time() - (this->start_hotel.Getarriving_time() + inst.hotels.at(this->start_hotel.Getpoint_id()).Getservice_time() ));
                else{
                    int prev_pos = this->points.size() - 2;
                    this->points.at(prev_pos).Setfree_time(this->points.back().Getarriving_time() - (this->points.at(prev_pos).Getarriving_time() + inst.poi.at(this->points.at(prev_pos).Getpoint_id()).Getservice_time() ));
                }


                visited_points.at(j) = true;
            }

            if(length >= inst.trip_length[num_trip]){
                break;
            }
        }


    }

}

void Trip::Print_trip(Instance inst){
    //Print start hotel
    std::cout << "Start hotel: " << inst.hotels[this->start_hotel.Getpoint_id()].Getname() << std::endl;
    std::cout << "\t Start time: " << this->start_hotel.Getarriving_time() << std::endl;
    std::cout << "\t Free time: " << this->start_hotel.Getfree_time() << std::endl;

    //Print POI
    int i = 0;
    for(std::vector<Trip_point>::iterator it = this->points.begin(); it != points.end(); it++, i++) {
        std::cout << "POI " << i << std::endl;
        std::cout << "\t Name: " << inst.poi[it->Getpoint_id()].Getname() << std::endl;
        std::cout << "\t Start time: " << it->Getarriving_time() << std::endl;
        std::cout << "\t Free time: " << it->Getfree_time() << std::endl;
    }

    //Print end hotel
    std::cout << "End hotel: " << inst.hotels[this->end_hotel.Getpoint_id()].Getname() << std::endl;
}

#include "Hotel_Sequences.h"
#include "Instance.h"
#include "Tour.h"

#include <iostream>

Hotel_Sequences::Hotel_Sequences()
{
    //ctor
}

Hotel_Sequences::Hotel_Sequences(int p, float a): population(p), alpha(a){}

Hotel_Sequences::~Hotel_Sequences()
{
    //dtor
}

bool Hotel_Sequences::Inserthotel_sequence(Tour hs){
    if(this->hotels_sequences.size() < this->population){
        this->hotels_sequences.push_back(hs);
        return true;
    }else{
        return false;
    }
}

bool Hotel_Sequences::Generate_hotel_sequence(Instance inst, int n){
    int i, j;
    std::vector<int> hs_aux;
    Tour tour_aux;
    //for each individual
    for(i = 0; i < n; i++){
        if(i == 0) hs_aux = inst.Best_hotel_sequence(1);
        else hs_aux = inst.Best_hotel_sequence(this->alpha);

        tour_aux = Tour();
        tour_aux.Initialize_tour(inst.num_trips, 1);

        for(j = 0; j < inst.num_trips; j++){
            tour_aux.Settrip(j, inst.viable_hotel_points.at(j).at(hs_aux.at(j)).at(hs_aux.at(j + 1)));
        }
    }

}

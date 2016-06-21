#include "Hotel_Sequences.h"
#include "Instance.h"
#include "Tour.h"

#include <iostream>
#include <algorithm>
#include <cstdlib>

bool Sort_sequence(Tour t1, Tour t2){
    return(t1.Gettour_score() > t2.Gettour_score());
}

Hotel_Sequences::Hotel_Sequences()
{
    //ctor
}

Hotel_Sequences::Hotel_Sequences(int pm, float a, float mt): population_max(pm), alpha(a), mutation_tax(mt), population_size(0){}

Hotel_Sequences::~Hotel_Sequences()
{
    //dtor
}

bool Hotel_Sequences::Inserthotel_sequence(Tour hs){
    if(this->hotel_population.size() < this->population_size){
        this->hotel_population.push_back(hs);
        return true;
    }else{
        return false;
    }
}

bool Hotel_Sequences::Generate_hotel_sequence(Instance inst, int n, bool greedy){
    int total_generated = (n == 0)? this->population_max - this->population_size : n;
    int i, j;
    std::vector<int> hs_aux;
    std::vector<Point> sorted_points;
    int ct_greedy = 0, ct_rand = 0;
    Tour tour_aux;
    //for each individual
    for(i = 0; i < total_generated; i++){
        if(i == 0 && greedy) hs_aux = inst.Best_hotel_sequence();
        else hs_aux = inst.Random_hotel_sequence();
//        std::cout << "///////////////////H sequence\n";
//        for(int k  = 0; k < hs_aux.size(); k++){
//            std::cout << inst.hotels.at(hs_aux.at(k)).Getname() << " - ";
//        }
//        std::cout << std::endl;
        tour_aux = Tour();
        tour_aux.Initialize_tour(inst, 1);
        //Create tour with viable trip vector
        for(j = 0; j < inst.num_trips; j++){
            tour_aux.Settrip(j, inst.viable_hotel_points.at(j).at(hs_aux.at(j)).at(hs_aux.at(j + 1)));
        }
        tour_aux.Tour_cleaning(inst);
        tour_aux.Recalculate_time(inst);
        tour_aux.Visiting_poi(inst);
        this->hotel_population.push_back(tour_aux);
        this->population_size++;
    }
    //Sorting population from best to worst fitness
    std::sort(this->hotel_population.begin(), this->hotel_population.end(), Sort_sequence);
}

void Hotel_Sequences::Genetic(Instance &inst, int num_iteration, float n_impr){
    int new_group = (int) (this->population_size * 0.1);
    int selected_elite = (int) (this->population_size * 0.1);
    Hotel_Sequences generation_aux(this->population_max, this->alpha, this->mutation_tax);

    int i, count_no_improvement = 0, j;
    for(i = 0; i < num_iteration; i++){
        std::cout << "Generation #" << i;
        std:: cout << "\tBest Score: " << this->hotel_population.at(0).Gettour_score()<< std::endl;
        generation_aux = Hotel_Sequences(this->population_max, this->alpha, this->mutation_tax);

        //Select selected_elite best individuals for next generation
        for(j = 0; j < selected_elite; j++){
            generation_aux.hotel_population.push_back(this->hotel_population.at(j));
            generation_aux.population_size++;
        }
        //Create new_group individuals for next generation
        generation_aux.Generate_hotel_sequence(inst, new_group);
        this->Recombinations(inst, generation_aux);
        if(this->hotel_population.at(0).Gettour_score() == generation_aux.hotel_population.at(0).Gettour_score()){
            count_no_improvement++;
        }else{
            count_no_improvement = 0;
        }
        std::cout << "\tNo impr: " << count_no_improvement << "\tRazao: " << (float)count_no_improvement/num_iteration << std::endl;
        this->hotel_population = generation_aux.hotel_population;
        if((float)count_no_improvement/num_iteration >= n_impr){
          break;
        }


    }
}

void Hotel_Sequences::Recombinations(Instance &inst, Hotel_Sequences &generation_aux){
    int elite_group = (int)(this->population_size * 0.2);
    int intermediate_group = (this->population_size * 0.7);
    Tour parent1, parent2;
    int num_parent1, num_parent2;
    int i = 0;
    while(generation_aux.population_size < this->population_size){
        num_parent1 = std::rand() % (elite_group + intermediate_group);
        parent1 = this->hotel_population.at(num_parent1);
        //parent from elite group
        if(num_parent1 < elite_group){
            num_parent2 = std::rand() % (this->population_size);
        }else{//parent from intermediate group
            num_parent2 = std::rand() % (elite_group + intermediate_group);
        }
        parent2 = this->hotel_population.at(num_parent2);

        generation_aux.Crossover(inst, parent1, parent2);
    }

    std::sort(generation_aux.hotel_population.begin(), generation_aux.hotel_population.end(), Sort_sequence);
    //remove if theres more than population size in generation_aux
    for(i = this->population_size; i < generation_aux.population_size; i++){
        generation_aux.hotel_population.erase(generation_aux.hotel_population.begin() + i);
    }
    generation_aux.population_size -= (generation_aux.population_size - this->population_size);
}

void Hotel_Sequences::Crossover(Instance &inst, Tour parent1, Tour parent2){
    Tour child_aux;
    std::vector<Trip_point> trip_aux;
    Trip_point tp_aux;
    bool found_child1 = false, found_child2 = false;
    int j;
    int start_crossover = (rand() % (inst.num_trips - 1)) + 1, crossover_pos, roulette_mutation;
    for(int i = 0; i < 2; i++){
        switch(i){
            case 0:
                crossover_pos = start_crossover;
                break;
            case 1:
                if(inst.num_trips > start_crossover + 1)
                    crossover_pos = start_crossover + 1;
                break;
            case 2:
                if(start_crossover > 0)
                    crossover_pos = start_crossover - 1;
                break;
        }
        Point start_hotel1 = inst.hotels.at(parent1.Gettrip(crossover_pos).at(0).Getpoint_id()),
        start_hotel2 = inst.hotels.at(parent2.Gettrip(crossover_pos).at(0).Getpoint_id()),
        end_hotel1 = inst.hotels.at(parent1.Gettrip(crossover_pos).back().Getpoint_id()),
        end_hotel2 = inst.hotels.at(parent2.Gettrip(crossover_pos).back().Getpoint_id());

        //Create child 1
        if(!found_child1 && inst.viable_hotel_pair.at(crossover_pos).at(start_hotel1.Getid()).at(end_hotel2.Getid()) > 0){
            //Create and insert start hotel
            trip_aux.erase(trip_aux.begin(), trip_aux.end());
            tp_aux = Trip_point(start_hotel1.Getid(), 0, inst.trip_length.at(crossover_pos) - start_hotel1.Getservice_time());
            trip_aux.push_back(tp_aux);
            //Create and insert end hotel
            tp_aux = Trip_point(end_hotel2.Getid(), inst.trip_length.at(crossover_pos) - end_hotel2.Getservice_time(), 0);
            trip_aux.push_back(tp_aux);
            child_aux = Tour();
            child_aux.Initialize_tour(inst, 1);

            //copy from first parent
            for(j = 0; j < crossover_pos; j ++){
                child_aux.Settrip(j, parent1.Gettrip(j));
            }
            child_aux.Settrip(crossover_pos, trip_aux);

            //copy from second parent
            for(j = crossover_pos + 1; j < inst.num_trips; j ++){
                child_aux.Settrip(j, parent2.Gettrip(j));
            }
            child_aux.Tour_cleaning(inst);
            child_aux.Recalculate_time(inst);
            child_aux.Visiting_poi(inst);
            //mutation
            roulette_mutation = rand() % 100;
            if(roulette_mutation < mutation_tax * 100) child_aux.Mutation(inst);
            found_child1 = true;
            this->hotel_population.push_back(child_aux);
            this->population_size++;
        }

        //Create child 2
        if(!found_child2 && inst.viable_hotel_pair.at(crossover_pos).at(start_hotel2.Getid()).at(end_hotel1.Getid()) > 0){
            //Create and insert start hotel
            trip_aux.erase(trip_aux.begin(), trip_aux.end());
            tp_aux = Trip_point(start_hotel2.Getid(), 0, inst.trip_length.at(crossover_pos) - start_hotel2.Getservice_time());
            trip_aux.push_back(tp_aux);
            //Create and insert end hotel
            tp_aux = Trip_point(end_hotel1.Getid(), inst.trip_length.at(crossover_pos) - end_hotel1.Getservice_time(), 0);
            trip_aux.push_back(tp_aux);

            child_aux = Tour();
            child_aux.Initialize_tour(inst, 1);
            //copy from second parent
            for(j = 0; j < crossover_pos; j ++){
                child_aux.Settrip(j, parent2.Gettrip(j));
            }
            child_aux.Settrip(crossover_pos, trip_aux);
            //copy from first parent
            for(j = crossover_pos + 1; j < inst.num_trips; j ++){
                child_aux.Settrip(j, parent1.Gettrip(j));
            }
            child_aux.Tour_cleaning(inst);
            child_aux.Recalculate_time(inst);
            child_aux.Visiting_poi(inst);
            roulette_mutation = rand() % 100;
            if(roulette_mutation < mutation_tax * 100) child_aux.Mutation(inst);
            found_child2 = true;
            this->hotel_population.push_back(child_aux);
            this->population_size++;
        }
    }
}

 void Hotel_Sequences::Print_sequences(Instance &inst, bool print_hotel){
    int i = 1;
    for(std::vector<Tour>::iterator it = this->hotel_population.begin(); it != this->hotel_population.end(); it++, i++){
        std::cout << "Individual #" << i << "\tScore: " << it->Gettour_score() << std::endl;
        if(print_hotel){
            std::cout << "\tHotel Sequence: ";
            it->Print_hotel_sequence(inst);
            std::cout << std::endl;
        }

    }
}


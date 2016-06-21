#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <iomanip>

#include <Point.h>
#include <Tour.h>
#include <Instance.h>
#include <Trip_point.h>
#include <Hotel_Sequences.h>

using namespace std;

//Id print_instance = 0 it doesn't print instance values
#define print_instance_header 1
#define print_instance_points 0
#define print_instance_solution 0
#define population 100
#define generation 80
#define no_improvement 0.25

//Path to instances directories
string Instance::filepath = "./instances/";

string inst_names[] = {
  /**0*/  "64-45-1-2.ophstw",
  /**1*/  "102-60-1-2.ophstw",
  /**2*/  "T3-100-1-2.ophstw",
  /**3*/  "T3-105-1-2.ophstw",
  /**4*/  "100-240-15-8.ophstw",
  /**5*/  "100-190-15-8.ophstw",
  /**6*/  "100-210-15-5.ophstw",
  /**7*/  "100-240-15-5.ophstw"};

int main(int argc, char *argv[]) {
    //variables
    int i, instance_number = 4, heuristic = 1, execution = 1;
    Instance instance;
    string instance_filename, file_line;
    clock_t reading_time_start, reading_time_end, time_start, time_end, hotel_start, hotel_end, start_hotelgen, end_hotelgen;
    float reading_time = 0, exec_time, tour_score = 0, hotel_creation_time, hotelgen_time;
    Tour solution;
    unsigned int seed;
    Hotel_Sequences hsequences(population, 0.35, 0.03);

    if(argc == 4){
        instance_number = atoi(argv[1]) % (sizeof(inst_names) / sizeof(*inst_names));
        instance_filename = inst_names[instance_number];
        heuristic = atoi(argv[2]);
        execution = atoi(argv[3]);
    }else{
        instance_filename = inst_names[instance_number];
    }

    cout << "File: " << instance_filename << endl;
    cout << "Heuristic: " << heuristic << endl;
    cout << "execution: " <<  execution << endl;

    cout.setf(ios::fixed, ios::floatfield);
    cout.setf(ios::showpoint);
    cout.precision(3);
    time_start = clock();
    reading_time_start = clock();
    instance = Instance();
    instance.Get_data_file(instance_filename);
    reading_time_end = clock();
    reading_time = reading_time_end - reading_time_start;
    //print instance
    if(print_instance_header){
        instance.Print_data();
        instance.Print_trip_lengths();
        cout << "Instance reading time: " << (reading_time / CLOCKS_PER_SEC * 1000.00) << "ms" << endl;
    }
    if(print_instance_points){
        instance.Print_hotels();
        instance.Print_POI();
    }

    solution = Tour();
    solution.Initialize_tour(instance, execution);
    solution.Initialize_hotels(instance.hotels, instance.trip_length);

    //Start hotel pair generation and selection
    hotel_start = clock();
    instance.Generate_hotels_pairs();
    instance.Calculate_hotel_zone();
    hotel_end = clock();

    start_hotelgen = clock();
    hsequences.Generate_hotel_sequence(instance, population, true);
    hsequences.Genetic(instance, generation, no_improvement);
    end_hotelgen = clock();

    hotel_creation_time = (hotel_end - hotel_start) / double(CLOCKS_PER_SEC) * 1000;
    hotelgen_time = (end_hotelgen - start_hotelgen) / double(CLOCKS_PER_SEC) * 1000;
    cout << "Hotel creation time: " << hotel_creation_time  << "ms" << endl;
    cout << "Hotel generation time: " << hotelgen_time << "ms" << endl;
    cout << "Hotel total time: " <<  hotelgen_time + hotel_creation_time << "ms" << endl;
    //hsequences.Print_sequences(instance, true);
    solution = hsequences.Gethotel_sequence(0);
    solution.Print_simple_tour(instance);
    solution.Validate_tour(instance);
//    std::cout << "-----------------------------------------------------------------------------------------------------------------------\n"<<
//        "\t\t\tWith POI Insertion\n";
//    solution.Erase_all_POI();
//    solution.Recalculate_time(instance);
//    solution.Print_simple_tour(instance);
//    solution.Print_visited_poi(instance);
//    solution.Visiting_poi(instance);
//    solution.Print_simple_tour(instance);
//    solution.Print_visited_poi(instance);
//    std::cout << "-----------------------------------------------------------------------------------------------------------------------\n"<<
//        "\t\t\tWith best trip POI\n";
//    Tour solution2 = Tour();
//    solution2.Initialize_tour(instance, 1);
//    //Create tour with viable trip vector
//    for(int j = 0; j < instance.num_trips; j++){
//        solution2.Settrip(j, instance.viable_hotel_points.at(j).at(hs_aux.at(j)).at(hs_aux.at(j + 1)));
//    }
//    solution2.Tour_cleaning(instance);
//    solution2.Recalculate_time(instance);
//    solution2.Visiting_poi(instance);

    /**
    Isso nao ocorre por enquanto
    //Sorting POI's list
    vector<Point> sorted_points(instance.poi);
    sort(sorted_points.begin(),sorted_points.end());
    vector<bool> visited_points(instance.num_vertices, false);

    i = 0;
    //Try to insert all POI's in solution
    for(vector<Point>::iterator it = sorted_points.begin(); it != sorted_points.end(); it++, i++){
        if(solution.Insert_point(instance, it->Getid(), heuristic)){
            visited_points.at(i) = true;
        }
    }
    */
    time_end = clock();
    exec_time = (time_end - time_start) / double(CLOCKS_PER_SEC) * 1000;
    /**
    if(print_instance_solution){
        solution.Print_tour(instance, exec_time);
        solution.Validate_tour(instance);
        cout << "------------ POI not visited ---------------- " << endl;
        i = 0;
        for(vector<bool>::iterator itv = visited_points.begin(); itv != visited_points.end(); itv++, i++){
            if(!(*itv)){
                cout << sorted_points.at(i).Getname() << endl;
            }
        }
    }else{
        solution.Create_Solution_file(instance,visited_points, sorted_points, execution, exec_time, heuristic);
    }

    bool first = (instance_number == 0 && heuristic == 1 && execution == 1)? true : false;

    solution.Instance_Report(instance_filename, first, heuristic, execution, exec_time);
    */
    cout << "Instance Execution Time: " << exec_time << " ms\n";

    return EXIT_SUCCESS;
}

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

using namespace std;

//Id print_instance = 0 it doesn't print instance values
#define print_instance_header 1
#define print_instance_points 0
#define print_instance_solution 0

//Path to instances directories
string Instance::filepath = "./instances/";

string inst_names[] = {"64-45-1-2.ophstw","102-60-1-2.ophstw", "T3-100-1-2.ophstw", "T3-105-1-2.ophstw", "100-240-15-8.ophstw", "100-190-15-8.ophstw",
    "100-210-15-5.ophstw", "100-240-15-5.ophstw"};

int main(int argc, char *argv[]) {
    //variables
    int i, instance_number = 0, heuristic = 1, execution = 1;
    Instance instance;
    string instance_filename, file_line;
    clock_t reading_time_start, reading_time_end, time_start, time_end;
    float reading_time = 0, exec_time, tour_score = 0;
    Tour solution;
    unsigned int seed;

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
    cout.precision(5);
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
    solution.Initialize_tour(instance.num_trips, execution);
    solution.Initialize_hotels(instance.hotels, instance.trip_length);

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
    time_end = clock();
    exec_time = (time_end - time_start) / double(CLOCKS_PER_SEC) * 1000;
    if(print_instance_solution){
        cout << "Instance total time: " << exec_time << "ms" << endl;
        solution.Print_tour(instance);

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

    solution.Validate_tour(instance);
    bool first = (instance_number == 0 && heuristic == 1 && execution == 1)? true : false;

    solution.Instance_Report(instance_filename, first, heuristic, execution, exec_time);

    return EXIT_SUCCESS;
}

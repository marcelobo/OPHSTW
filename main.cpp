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

//Path to instances directories
string Instance::filepath = "./instances/";

int main(int argc, char *argv[]) {
    //variables
    int i, exec_num = 1, execution_times = 10, heuristic_num = 1;
    Instance instance;
    string instance_filename = "100-240-15-10.ophstw", file_line;
    clock_t reading_time_start, reading_time_end, time_start, time_end;
    float reading_time = 0, exec_time, tour_score = 0;
    Tour trip_aux;
    Trip_point tp_aux;
    Tour solution;
    unsigned int seed;

    //innstace report variables
    float best = 0;
    vector<float> average_heuristic, best_heuristic, average_time;
    best_heuristic.resize(3, 0);
    average_heuristic.resize(3,0);
    average_time.resize(3,0);


    cout.setf( ios::fixed, ios::floatfield);
    cout.setf(ios::showpoint);
    cout.precision(5);
    for(heuristic_num = 1; heuristic_num <= 3; heuristic_num++){
         cout << "---------------------------------------- Heuristic #" << heuristic_num << " ----------------------------------------" << endl;
        for(exec_num = 1; exec_num <= execution_times; exec_num++){
            time_start = clock();
            cout << "Execution #" << exec_num << endl;
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
            solution.Initialize_tour(instance.num_trips, ((heuristic_num - 1) * 3) + exec_num);
            solution.Initialize_hotels(instance.hotels, instance.trip_length);

            //Sorting POI's list
            vector<Point> sorted_points(instance.poi);
            sort(sorted_points.begin(),sorted_points.end());
            vector<bool> visited_points(instance.num_vertices, false);

            i = 0;
            //Try to insert all POI's in solution
            for(vector<Point>::iterator it = sorted_points.begin(); it != sorted_points.end(); it++, i++){
                if(solution.Insert_point(instance, it->Getid(), heuristic_num)){
                    visited_points.at(i) = true;
                    //solution.Print_tour(instance);
                }
            }
            time_end = clock();
            exec_time = (time_end - time_start) / double(CLOCKS_PER_SEC) * 1000;
            cout << "Instance total time: " << exec_time << "ms" << endl;
            solution.Print_tour(instance);

            cout << "------------ POI not visited ---------------- " << endl;
            i = 0;
            for(vector<bool>::iterator itv = visited_points.begin(); itv != visited_points.end(); itv++, i++){
                if(!(*itv)){
                    cout << sorted_points.at(i).Getname() << endl;
                }
            }
            solution.Validate_tour(instance);
            solution.Create_Solution_file(instance,visited_points, sorted_points, exec_num, exec_time, heuristic_num);
            cout << " --------------------------------------------------------------------------" << endl << endl;

            tour_score = solution.Gettour_score();
            if(tour_score > best) best = tour_score;
            if(tour_score > best_heuristic.at(heuristic_num - 1)) best_heuristic.at(heuristic_num - 1) = tour_score;
            average_heuristic.at(heuristic_num - 1) +=tour_score;
            average_time[heuristic_num - 1] += exec_time;
        }
    }

    cout << "Report:" << endl;
    cout << "Best: " << best << endl;
    for(heuristic_num = 1; heuristic_num <= 3; heuristic_num++){
        best_heuristic[heuristic_num - 1] = (best_heuristic.at(heuristic_num - 1)/best) - 1;
        average_heuristic.at(heuristic_num - 1) = (average_heuristic.at(heuristic_num - 1) /( execution_times * best)) - 1;
        average_time.at(heuristic_num - 1) /= execution_times;
        cout << "H" << heuristic_num << "\t best:" << best_heuristic.at(heuristic_num - 1) << "\t average: " << average_heuristic.at(heuristic_num - 1) << "\t average time (ms): " << average_time.at(heuristic_num - 1) << endl;
    }
    solution.Create_Report_file(instance.name, best, best_heuristic, average_heuristic, average_time);
	return EXIT_SUCCESS;
}

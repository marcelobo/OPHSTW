#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <dirent.h>
#include <algorithm>

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
    //directory reading
    DIR *dir;
    struct dirent *ent;
    int i;
    Instance instance;
    string instance_filename, file_line;
    clock_t time_start, time_end;
    Tour trip_aux;
    Trip_point tp_aux;
    Tour solution;


    dir = opendir( Instance::filepath.c_str());
	if (dir != NULL) {//if directory exists
        ent = readdir (dir);
        while (ent != NULL) {//while exists files into directory
            if(strcmp(ent->d_name,".") && strcmp(ent->d_name, "..")){
                time_start = clock();
                cout << "Instance " << ent->d_name << endl;
                instance.Get_data_file(ent->d_name);
                time_end = clock();
                //print instance
                if(print_instance_header){
                    instance.Print_data();
                    instance.Print_trip_lengths();
                    cout << "Tempo Leitura instancia: " << time_end - time_start /CLOCKS_PER_SEC << "ms" << endl;
                }
                if(print_instance_points){
                    instance.Print_hotels();
                    instance.Print_POI();
                }

                solution.Initialize_tour(instance.num_trips);
                //Add the start hotel as the start hotel of the first trip
                tp_aux = Trip_point(0, 0, instance.trip_length[0] - instance.hotels[0].Getservice_time());
                solution.Setstart_hotel(tp_aux, 0);

                //Hotels random selection
                int num_hotel = 0;
                for(i = 1; i < instance.num_trips; i++){
                    num_hotel = rand() % instance.num_hotels;
                    //set end hotel from previous trip
                    solution.Setend_hotel(Trip_point(num_hotel, instance.trip_length[i] - instance.hotels[num_hotel].Getservice_time(), 0.0), i - 1);
                    //set start hotel from current trip
                    solution.Setstart_hotel(Trip_point(num_hotel, 0.0, instance.trip_length[i] - instance.hotels[num_hotel].Getservice_time()), i);
                    //Update trip length for previous trip
                    solution.Settrip_length(i - 1, instance.hotels.at(solution.Getstart_hotel(i - 1).Getpoint_id()).Distance(instance.hotels.at(solution.Getend_hotel(i - 1).Getpoint_id())));
                }

                //Add the end hotel as the end hotel of the last trip
                tp_aux = Trip_point(1, instance.trip_length.back() - instance.hotels[1].Getservice_time(), 0);
                solution.Setend_hotel(tp_aux, instance.num_trips - 1);
                solution.Settrip_length(instance.num_trips - 1, instance.hotels.at(solution.Getstart_hotel(instance.num_trips - 1).Getpoint_id()).Distance(instance.hotels.at(tp_aux.Getpoint_id())));

                //Sorting POI's list
                vector<Point> sorted_points(instance.poi);
                sort(sorted_points.begin(),sorted_points.end());
                std::vector<bool> visited_points(instance.num_vertices, false);

                i = 0;
                //Try to insert all POI's in solution
                for(vector<Point>::iterator it = sorted_points.begin(); it != sorted_points.end(); it++, i++){
                    if(solution.Insert_point(instance, it->Getid())){
                        visited_points.at(i) = true;
                        solution.Print_tour(instance);
                    }
                }

                cout << "Insert Finished" << endl;
                solution.Print_tour(instance);
                cout << " ------------------------------ " << endl << endl;
            }
            ent = readdir(dir);
        }
        closedir(dir);
    } else {
        // could not open directory
        perror ("");
        return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <dirent.h>
#include <algorithm>

#include <Point.h>
#include <Instance.h>
#include <Trip.h>
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
    Trip trip_aux;
    Trip_point tp_aux;


    dir = opendir( Instance::filepath.c_str());
	if (dir != NULL) {//if directory exists
        ent = readdir (dir);
        while (ent != NULL) {//while exists files into directory
            if(strcmp(ent->d_name,".") && strcmp(ent->d_name, "..")){
                time_start = clock();
                cout << "Instance " << ent->d_name << endl;
                instance.Get_data_file(ent->d_name);
                time_end = clock();
                cout << "Tempo Leitura instancia: " << time_end - time_start /CLOCKS_PER_SEC << endl;
                //print instance
                if(print_instance_header){
                    instance.Print_data();
                }
                if(print_instance_points){
                    instance.Print_trip_lengths();
                    instance.Print_hotels();
                    instance.Print_POI();
                }

                Trip_point tp_aux;
                //Add the start hotel as the start hotel of the first trip
                tp_aux = Trip_point(0, 0, instance.trip_length[0] - instance.hotels[0].Getservice_time());
                instance.trips[0].Setstart_hotel(tp_aux);

                 //Add the end hotel as the end hotel of the last trip
                tp_aux = Trip_point(1, instance.trip_length.back() - instance.hotels[1].Getservice_time(), 0);
                instance.trips.back().Setend_hotel(tp_aux);

                //Hotels random selection
                int num_hotel = 0;
                for(i = 1; i <= instance.num_trips; i++){
                    num_hotel = rand() % instance.num_hotels;
                    //set end hotel from previous trip
                    instance.trips[i - 1].Setend_hotel(Trip_point(num_hotel, instance.trip_length[i - 1] - instance.hotels[num_hotel].Getservice_time(), 0));
                    //set start hotel from current trip
                    //this must be the same where finishes the previous trip
                    instance.trips[i].Setstart_hotel(Trip_point(num_hotel, 0.0, instance.trip_length[i] - instance.hotels[num_hotel].Getservice_time()));
                }

                //Sorting POI's list
                vector<Point> sorted_points(instance.poi);
                sort(sorted_points.begin(),sorted_points.end());
                /**
                for(int i = 0; i < sorted_points.size(); i++){
                    std::cout << "POI #" << i + 1 << std::endl;
                    std::cout << "POI ID: " << sorted_points.at(i).Getid() << std::endl;
                    std::cout << "Name: " << sorted_points.at(i).Getname()<< std::endl;
                    std::cout << "Opening time: " << sorted_points.at(i).Getopening_time() << std::endl;
                }*/
                std::vector<bool> visited_points(instance.num_vertices, false);

                i = 0;
                for(vector<Trip>::iterator it = instance.trips.begin(); it != instance.trips.end(); it++, i++)
                    it->Generate_trip(instance, sorted_points, visited_points, i);


                for(i = 0; i < instance.num_trips; i++){
                    cout << "--------- Trip #" << i << " ---------" << endl;
                    instance.trips[i].Print_trip(instance);
                    cout << endl << endl;
                }

                //Print not visited points
                cout << " -------------- Not visited POI -------------- " << endl;
                for(i = 0; i < visited_points.size(); i++){
                    if(!visited_points.at(i)){
                        cout << sorted_points.at(i).Getname() << endl;
                    }
                }

                for(i = 0; i < visited_points.size(); i++){
                    if(!visited_points.at(i)){
                        if(sorted_points.at(i).InsertPoint(instance)){
                            visited_points.at(i) = true;
                            cout << "Inserted " << instance.poi.at(sorted_points.at(i).Getid()).Getname() << endl;
                        }
                    }
                }

                 for(i = 0; i < instance.num_trips; i++){
                    cout << "--------- Trip #" << i << " ---------" << endl;
                    instance.trips[i].Print_trip(instance);
                    cout << endl << endl;
                }
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

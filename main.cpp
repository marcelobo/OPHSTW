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

                //print instance
                if(print_instance_header){
                    instance.Print_data();
                    instance.Print_trip_lengths();
                    //cout << "Tempo Leitura instancia: " << time_end - time_start /CLOCKS_PER_SEC << "ms" << endl;
                }
                if(print_instance_points){
                    instance.Print_hotels();
                    instance.Print_POI();
                }

                solution.Initialize_tour(instance.num_trips);
                solution.Initialize_hotels(instance.hotels, instance.trip_length);

                //Sorting POI's list
                vector<Point> sorted_points(instance.poi);
                sort(sorted_points.begin(),sorted_points.end());
                vector<bool> visited_points(instance.num_vertices, false);

                i = 0;
                //Try to insert all POI's in solution
                for(vector<Point>::iterator it = sorted_points.begin(); it != sorted_points.end(); it++, i++){
                    if(solution.Insert_point(instance, it->Getid())){
                            visited_points.at(i) = true;
                            solution.Print_tour(instance);
                    }

                }

                solution.Print_tour(instance);
                time_end = clock();
                cout << "Tempo total instancia: " << time_end - time_start /CLOCKS_PER_SEC << "ms" << endl;
                cout << "------------ POI not visited ---------------- " << endl;
                i = 0;
                for(vector<bool>::iterator itv = visited_points.begin(); itv != visited_points.end(); itv++, i++){
                    if(!(*itv)){
                        cout << sorted_points.at(i).Getname() << endl;
                    }
                }

                solution.Validate_tour(instance);
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

#ifndef INSTANCE_H
#define INSTANCE_H
#include <string>
#include <vector>
#include <Trip.h>

class Instance
{
    public:
        //variables
        static std::string filepath;
        int num_vertices;
        int num_hotels;
        int num_trips;
        float tour_length;
        std::string name;
        std::vector<Point> hotels;
        std::vector<Point> poi;
        std::vector<float> trip_length;
        std::vector<Trip> trips;

        //function
        Instance();
        virtual ~Instance();
        void Get_data_file(std::string name);
        void Print_data();
        void Print_POI();
        void Print_hotels();
        void Print_trip_lengths();

        //function declaration
//        int Getnum_vertices() { return num_vertices; }
//        void Setnum_vertices(int val) { num_vertices = val; }
//        int Getnum_hotels() { return num_hotels; }
//        void Setnum_hotels(int val) { num_hotels = val; }
//        int Getnum_trips() { return num_trips; }
//        void Setnum_trips(int val) { num_trips = val; }
//        float Gettour_length() { return tour_length; }
//        void Settour_length(float val) { tour_length = val; }
//        std::string Getname() { return name; }
//        void Setname(std::string val) { name = val; }
    protected:

    private:
};

#endif // INSTANCE_H

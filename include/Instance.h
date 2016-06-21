#ifndef INSTANCE_H
#define INSTANCE_H
#include <string>
#include <vector>

class Point;
class Trip_point;

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
        std::vector<std::pair<int,int> > sorted_poi; //Store ID and Score from sorted POI. Biggest to smallest
        std::vector<float> trip_length;
        std::vector< std::vector< std::vector<float> > > viable_hotel_pair;//Store trip score for each hotel pair / Vector sequence: trip, start hotel, end hotel
        std::vector< std::vector< std::vector< std::vector<Trip_point> > > > viable_hotel_points;//Vector that stores POI's for each hotel pair / Vector sequence: trip, start hotel, end_hotel, poi visited
        std::vector< std::vector<float> > Point_distance;
        std::vector< std::vector<float> > hotel_pair_zone;


        //function
        Instance();
        virtual ~Instance();
        void Get_data_file(std::string name);
        void Print_data();
        void Print_POI();
        void Print_hotels();
        void Print_trip_lengths();
        void Generate_hotels_pairs();
        void Calculate_hotel_zone();
        std::vector<int> Best_hotel_sequence();
        std::vector<int> Random_hotel_sequence();

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

#ifndef TRIP_H
#define TRIP_H
#include <Trip_point.h>
#include <vector>

class Instance;

class Tour
{
    public:
        Tour();
        virtual ~Tour();
        float Gettrip_length(int num_trip) { return trip_length.at(num_trip); }
        void Settrip_length(int num_trip, float cl) {trip_length.at(num_trip) = cl;}
        float Gettrip_score(int num_trip) { return trip_score.at(num_trip); }
        void Settrip_score(int num_trip, float ts) {trip_score.at(num_trip) = ts;}
        Trip_point Getstart_hotel(int num_trip) { return start_hotel.at(num_trip); }
        void Setstart_hotel(const Trip_point &sh, int num_trip) {start_hotel.at(num_trip) = sh;}
        Trip_point Getend_hotel(int num_trip) { return end_hotel.at(num_trip); }
        void Setend_hotel(const Trip_point &sh, int num_trip) {end_hotel.at(num_trip) = sh;}
        void Generate_tour(Instance &inst, std::vector<Point> &sorted_points, std::vector<bool> &visited_points, int num_trip);
        void Initialize_tour(int num_trips);
        bool Insert_point(Instance &inst, int point_id);
        bool Can_insert(std::vector<Point> inst_poi, std::vector<Trip_point> &curr_trip, int tp_pos, float remaining_time);
        void Print_tour(Instance inst);
    protected:

    private:
        std::vector<float> trip_length;
        std::vector<float> trip_score;
        std::vector<std::vector<Trip_point> > tp_list;
        std::vector<Trip_point> start_hotel;
        std::vector<Trip_point> end_hotel;
};

#endif // TRIP_H

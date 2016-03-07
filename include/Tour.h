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
        Trip_point Getstart_hotel(int num_trip) { return trip.at(num_trip).at(0); }
        void Setstart_hotel(const Trip_point &sh, int num_trip) {trip.at(num_trip).front() = sh;}
        Trip_point Getend_hotel(int num_trip) { return trip.at(num_trip).back(); }
        void Setend_hotel(const Trip_point &eh, int num_trip) {trip.at(num_trip).back() = eh;}
        void Generate_tour(Instance &inst, std::vector<Point> &sorted_points, std::vector<bool> &visited_points, int num_trip);
        void Initialize_tour(int num_trips);
        void Initialize_hotels(std::vector<Point> &hotels, std::vector<float> &length);
        bool Insert_point(Instance &inst, int point_id);
        bool Insert_with_delay(Instance &inst, std::vector<Trip_point> &curr_trip, int tp_pos, float remaining_time, bool can_update);
        float Calculate_trip_length(int num_trip, Instance &inst);
        bool Is_best_insert_position(Instance &inst, int num_trip, int prev_pos, int insert_id, int type, float *best_cost);
        void Print_tour(Instance inst);
    protected:

    private:
        std::vector<float> trip_length;
        std::vector<float> trip_score;
        std::vector<std::vector<Trip_point> > trip;
};

#endif // TRIP_H

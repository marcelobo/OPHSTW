#ifndef TRIP_H
#define TRIP_H
#include <Trip_point.h>
#include <vector>

class Instance;

class Tour
{
    public:
        static const float epsilon = 0.0001;
        Tour();
        virtual ~Tour();
        float Gettrip_length(int num_trip) { return trip_length.at(num_trip); }
        void Settrip_length(int num_trip, float cl) {trip_length.at(num_trip) = cl;}
        float Gettrip_score(int num_trip) { return trip_score.at(num_trip); }
        void Settrip_score(int num_trip, float ts) {trip_score.at(num_trip) = ts;}
        float Gettour_score();
        Trip_point Getstart_hotel(int num_trip) { return trip.at(num_trip).at(0); }
        void Setstart_hotel(const Trip_point &sh, int num_trip);
        Trip_point Getend_hotel(int num_trip) { return trip.at(num_trip).back(); }
        void Setend_hotel(const Trip_point &eh, int num_trip);
        void Generate_tour(Instance &inst, std::vector<Point> &sorted_points, std::vector<bool> &visited_points, int num_trip);
        void Initialize_tour(int num_trips, int exec_time, int seed = 0);
        void Initialize_hotels(std::vector<Point> &hotels, std::vector<float> &length);
        bool Insert_point(Instance &inst, int point_id, int heuristic_num = 1);
        bool Insert_with_delay(Instance &inst, std::vector<Trip_point> &curr_trip, int tp_pos, float remaining_time, bool can_update);
        float Calculate_trip_length(int num_trip, Instance &inst);
        bool Is_best_insert_position(Instance &inst, int num_trip, int prev_pos, int insert_id, int type, float &best_cost, bool has_best);
        void Print_tour(Instance &inst);
        bool Validate_tour(Instance &inst);
        void Create_Solution_file(Instance &inst, std::vector<bool> &visited_points, std::vector<Point> &sorted_points, int exec_num, float exec_time, int heuristic_num = 1);
        void Instance_Report(std::string instance_name, bool first, int heuristic, int execution, float time);
        void Create_Report_file(std::string instance_name, float best, std::vector<float> &best_by_heuristic, std::vector<float> &average_by_heuristic, std::vector<float> &average_time);
    protected:

    private:
        std::vector<float> trip_length;
        std::vector<float> trip_score;
        std::vector<std::vector<Trip_point> > trip;
        unsigned int seed;
};

#endif // TRIP_H

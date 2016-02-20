#ifndef TRIP_H
#define TRIP_H
#include <Trip_point.h>
#include <vector>

class Instance;

class Trip
{
    public:
        Trip();
        virtual ~Trip();
        Trip_point Getstart_hotel() { return start_hotel; }
        void Setstart_hotel(const Trip_point &sh) {start_hotel = sh;}
        Trip_point Getend_hotel() { return end_hotel; }
        void Setend_hotel(const Trip_point &eh) { end_hotel = eh; }
        void Generate_trip(Instance &inst, std::vector<Point> &sorted_points, std::vector<bool> &visited_points, int num_trip);
        bool Add_point(Instance &inst, int point_id);
        void Print_trip(Instance inst);
    protected:

    private:
        float curr_length;
        std::vector<Trip_point> points;
        Trip_point start_hotel;
        Trip_point end_hotel;

};

#endif // TRIP_H

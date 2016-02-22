#ifndef TRIP_POINT_H
#define TRIP_POINT_H

#include <point.h>


class Trip_point
{
    public:
        Trip_point();
        Trip_point(int id, float arvt, float ft);
        virtual ~Trip_point();

        int Getpoint_id() { return point_id; }
        void Setpoint_id(int val) { point_id = val; }
        float Getarriving_time() { return arriving_time; }
        void Setarriving_time(float val) { arriving_time = val; }
        float Getfree_time() { return free_time; }
        void Setfree_time(float val) { free_time = val; }

    protected:

    private:
        int point_id;
        float arriving_time;
        float free_time;
};

#endif // TRIP_POINT_H

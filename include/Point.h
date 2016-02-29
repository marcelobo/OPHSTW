#ifndef POINT_H
#define POINT_H
#include <string>
#include <vector>


class Trip_point;

class Point
{
    public:
        Point();
        Point(const Point &p);
        Point(std::string n, int id, float x, float y, float s, float st, float ot, float ct);
        virtual ~Point();
        std::string Getname() { return name; }
        void Setname(std::string val) { name = val; }
        int Getid() { return id; }
        void Setid(int val) { id = val; }
        float Getx() { return x; }
        void Setx(float val) { x = val; }
        float Gety() { return y; }
        void Sety(float val) { y = val; }
        float Getscore() { return score; }
        void Setscore(float val) { score = val; }
        float Getservice_time() const { return service_time; }
        void Setservice_time(float val) { service_time = val; }
        float Getopening_time() { return opening_time; }
        void Setopening_time(float val) { opening_time = val; }
        float Getclosing_time() { return closing_time; }
        void Setclosing_time(float val) { closing_time = val; }
        float Getmax_delay() { return max_delay; }
        void Setmax_delay(float val) { max_delay = val; }
        bool operator<(const Point& poi) const;
        float Distance(Point &p);
    protected:

    private:
        std::string name;
        int id;
        float x;
        float y;
        float score;
        float service_time;
        float opening_time;
        float closing_time;
        float max_delay;
};

#endif // POINT_H

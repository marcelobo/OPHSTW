#ifndef HOTEL_SEQUENCES_H
#define HOTEL_SEQUENCES_H

#include <vector>

class Point;
class Instance;
class Tour;

class Hotel_Sequences
{
    public:
        Hotel_Sequences();
        Hotel_Sequences(int population, float a);
        virtual ~Hotel_Sequences();

        float Getalpha() { return alpha; }
        void Setalpha(float val) { alpha = val; }
        bool Inserthotel_sequence(Tour hs);
        bool Generate_hotel_sequence(Instance inst, int n = 0);

    protected:

    private:
        float alpha;
        int population;
        std::vector<Tour> hotels_sequences;
};

#endif // HOTEL_SEQUENCES_H

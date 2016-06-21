#ifndef HOTEL_SEQUENCES_H
#define HOTEL_SEQUENCES_H

#include <vector>
#include <Tour.h>

class Point;
class Instance;

class Hotel_Sequences
{
    public:
        Hotel_Sequences();
        Hotel_Sequences(int pm, float a, float mt);
        virtual ~Hotel_Sequences();

        float Getalpha() { return alpha; }
        void Setalpha(float val) { alpha = val; }
        Tour Gethotel_sequence(int pos){ return hotel_population.at(pos);}
        bool Inserthotel_sequence(Tour hs);
        bool Generate_hotel_sequence(Instance inst, int n = 0, bool greedy = false);
        void Genetic(Instance &inst, int num_iteration, float n_impr);
        void Recombinations(Instance &inst, Hotel_Sequences &generation_aux);
        void Crossover(Instance &inst, Tour parent1, Tour parent2);
        void Print_sequences(Instance &inst, bool print_hotel);

    protected:

    private:
        float alpha;
        float mutation_tax;
        int population_size;
        int population_max;
        std::vector<Tour> hotel_population;

};

#endif // HOTEL_SEQUENCES_H

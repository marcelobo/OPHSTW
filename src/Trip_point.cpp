#include "Trip_point.h"

Trip_point::Trip_point(){
    //ctor
}

Trip_point::Trip_point(int pi, float arvt, float ft)
    :point_id(pi),arriving_time(arvt),idle_time(ft){

}

Trip_point::~Trip_point(){
    //dtor
}

#include "Trip_point.h"

Trip_point::Trip_point(){
    //ctor
}

Trip_point::Trip_point(int pi, float arvt, float md, float ft)
    :point_id(pi),arriving_time(arvt),max_delay(md),free_time(ft){

}

Trip_point::~Trip_point(){
    //dtor
}

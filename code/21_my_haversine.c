#include "20_my_trig_functions.c"


float64 my_haversine(float64 x0, float64 y0, float64 x1, float64 y1, float64 radius)
{
    PROFILE_FUNCTION_BEGIN();
    float64 dlon = deg_to_rad(x1 - x0);
    float64 dlat = deg_to_rad(y1 - y0);

    float64 lat0 = deg_to_rad(y0);
    float64 lat1 = deg_to_rad(y1);

    float64 a = square(my_sine(dlat/2.0)) + my_cosine(lat0)*my_cosine(lat1)*square(my_sine(dlon/2.0));
    float64 c = 2.0*my_arcsine(my_sqrt(a));

    float64 result = radius * c;
    PROFILE_FUNCTION_END();
    return result;
}

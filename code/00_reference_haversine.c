#include <math.h>


float64 reference_haversine(float64 x0, float64 y0, float64 x1, float64 y1, float64 radius)
{
    PROFILE_FUNCTION_BEGIN();
    float64 dlon = deg_to_rad(x1 - x0);
    float64 dlat = deg_to_rad(y1 - y0);

    float64 lat0 = deg_to_rad(y0);
    float64 lat1 = deg_to_rad(y1);

    float64 a = square(sin(dlat/2.0)) + cos(lat0)*cos(lat1)*square(sin(dlon/2.0));
    float64 c = 2.0*asin(sqrt(a));

    float64 result = radius * c;
    PROFILE_FUNCTION_END();
    return result;
}

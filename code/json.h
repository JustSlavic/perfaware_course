#ifndef JSON_H
#define JSON_H

#include "base.h"


typedef struct json json;

json *json__parse(void *data, uint64 size);
void json__free(json *);

int32 json__get_length(json *v);
float64 json__get_float(json *v);
json *json__list_lookup(json *lst, int32 n);
json *json__object_lookup(json *obj, char const *s);


#endif

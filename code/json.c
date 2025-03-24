#include "base.h"
#include "json.h"

#include <string.h>


typedef enum
{
    JSON_NONE,
    JSON_BOOL,
    JSON_INT,
    JSON_FLOAT,
    JSON_STRING,
    JSON_LIST,
    JSON_OBJECT,
} json_tag;

typedef struct json_list json_list;
typedef struct json_object json_object;

struct json
{
    json_tag tag;
    union
    {
        bool b;
        int64 i;
        float64 f;
        struct
        {
            char *s;
            int32 n;
        };
        struct
        {
            json_list *l;
            int32 n1;
        };
        struct
        {
            json_object *o;
            int32 n2;
        };
    };
};

struct json_list
{
    json values[1 << 10];
    uint32 value_count;
    json_list *next;
};

struct json_object
{
    json key;
    json value;
    json_object *next;
};

typedef enum
{
    JSON_TOKEN_EOF,
    JSON_TOKEN_INT = 300,
    JSON_TOKEN_FLOAT,
    JSON_TOKEN_STRING,
} json_token_tag;

typedef struct
{
    json_token_tag tag;
    union
    {
        int64 i;
        float64 f;
        struct
        {
            char *s;
            int32 n;
        };
    };
} json_token;

typedef struct
{
    uint8 *data;
    uint64 size;

    uint64 index;

    bool saved_token_valid;
    json_token saved_token;
} json_lexer;


char get_char(json_lexer *lexer)
{
    char c = 0;
    if (lexer->index < lexer->size)
    {
        c = lexer->data[lexer->index];
    }
    return c;
}

char eat_char(json_lexer *lexer)
{
    char c = get_char(lexer);
    lexer->index += 1;
    return c;
}

typedef bool (*predicate)(char);

bool is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

bool is_ascii_alpha(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool is_ascii_digit(char c)
{
    return ('0' <= c && c <= '9');
}

bool is_not_double_quote_and_not_new_line(char c)
{
    return (c != '\"') && (c != '\n');
}

int32 consume_while(json_lexer *lexer, predicate p)
{
    int32 count = 0;
    char c = get_char(lexer);
    while (c > 0 && p(c))
    {
        eat_char(lexer);
        c = get_char(lexer);
        count += 1;
    }
    return count;
}

void skip_whitespaces(json_lexer *lexer)
{
    consume_while(lexer, is_whitespace);
}

json_token get_token(json_lexer *lexer)
{
    if (lexer->saved_token_valid)
    {
        return lexer->saved_token;
    }

    skip_whitespaces(lexer);

    json_token result = {};

    char c = get_char(lexer);
    if (c == 0)
    {
    }
    else if (c == '\"')
    {
        eat_char(lexer); // consume first double quote
        result.tag = JSON_TOKEN_STRING;
        result.s = (char *)(lexer->data + lexer->index);
        result.n = consume_while(lexer, is_not_double_quote_and_not_new_line);
        eat_char(lexer); // consume final double quote
    }
    else if (is_ascii_digit(c) || c == '-')
    {
        int64 sign = 1;
        int64 integer = 0;
        float64 fractional = 0;
        float64 frac_coeff = 0.1;
        if (c == '-')
        {
            sign = -1;
            eat_char(lexer);
            c = get_char(lexer);
        }

        while (is_ascii_digit(c))
        {
            integer *= 10;
            integer += (c - '0');
            eat_char(lexer);
            c = get_char(lexer);
        }

        if (c == '.')
        {
            eat_char(lexer); // eat dot
            c = get_char(lexer);
            while (is_ascii_digit(c))
            {
                fractional += (c - '0') * frac_coeff;
                frac_coeff *= 0.1;
                eat_char(lexer);
                c = get_char(lexer);
            }
            result.tag = JSON_TOKEN_FLOAT;
            result.f = sign * ((float64) integer + fractional);
        }
        else
        {
            result.tag = JSON_TOKEN_INT;
            result.i = sign * integer;
        }
    }
    else
    {
        eat_char(lexer);
        result.tag = c;
    }

    lexer->saved_token = result;
    lexer->saved_token_valid = true;
    return result;
}

json_token eat_token(json_lexer *lexer)
{
    json_token t = get_token(lexer);
    lexer->saved_token_valid = false;
    return t;
}

json parse_json_impl(json_lexer *lexer)
{
    PROFILE_FUNCTION_BEGIN();
    json result = {};
    json_token t = get_token(lexer);
    if (t.tag == JSON_TOKEN_INT)
    {
        result.tag = JSON_INT;
        result.i = t.i;

        eat_token(lexer);
    }
    if (t.tag == JSON_TOKEN_FLOAT)
    {
        result.tag = JSON_FLOAT;
        result.f = t.f;

        eat_token(lexer);
    }
    if (t.tag == '[')
    {
        eat_token(lexer);

        result.tag = JSON_LIST;
        result.l = malloc(sizeof(json_list));
        memset(result.l, 0, sizeof(*result.l));
        json_list *current = result.l;

        do
        {
            t = get_token(lexer);
            if (t.tag == ']') break;

            if (current->value_count == ARRAY_COUNT(current->values))
            {
                json_list *next = malloc(sizeof(json_list));
                memset(next, 0, sizeof(*next));
                current->next = next;
                current = next;
            }

            current->values[current->value_count++] = parse_json_impl(lexer);
            result.n += 1;

            t = get_token(lexer);
            if (t.tag == ',') eat_token(lexer);
        } while (true);
        if (t.tag == ']') eat_token(lexer);
    }
    if (t.tag == '{')
    {
        eat_token(lexer);

        result.tag = JSON_OBJECT;
        result.o = NULL;
        json_object *current = NULL;

        do
        {
            t = get_token(lexer);
            if (t.tag == '}') break;

            if (t.tag == JSON_TOKEN_STRING)
            {
                json key =
                {
                    .tag = JSON_STRING,
                    .s = t.s,
                    .n = t.n,
                };
                eat_token(lexer);

                t = eat_token(lexer);
                if (t.tag == ':')
                {
                    json_object *object_element = malloc(sizeof(json_object));

                    object_element->key = key;
                    object_element->value = parse_json_impl(lexer);
                    object_element->next = NULL;
                    result.n += 1;

                    if (current == NULL)
                    {
                        result.o = object_element;
                        current = object_element;
                    }
                    else
                    {
                        current->next = object_element;
                        current = object_element;
                    }
                }
            }

            t = get_token(lexer);
            if (t.tag == ',') eat_token(lexer);
        } while (true);
        if (t.tag == '}') eat_token(lexer);
    }
    PROFILE_FUNCTION_END();
    return result;
}

json *json__parse(void *data, uint64 size)
{
    PROFILE_FUNCTION_BANDWIDTH_BEGIN(size);
    json_lexer lexer =
    {
        .data = (uint8 *) data,
        .size = size,
    };

    json *result = malloc(sizeof(json));

    *result = parse_json_impl(&lexer);
    PROFILE_FUNCTION_END();
    return result;
}

void free_json_list(json_list *l);
void free_json_object(json_object *o);

void free_json_list(json_list *l)
{
    json_list *curr = l;
    json_list *next = curr->next;

    while (curr)
    {
        for (uint32 i = 0; i < curr->value_count; i++)
        {
            if (curr->values[i].tag == JSON_LIST) free_json_list(curr->values[i].l);
            if (curr->values[i].tag == JSON_OBJECT) free_json_object(curr->values[i].o);
        }

        free(curr);

        curr = next;
        if (next) next = next->next;
    }
}

void free_json_object(json_object *o)
{
    json_object *curr = o;
    json_object *next = curr->next;

    while (curr)
    {
        if (curr->value.tag == JSON_LIST) free_json_list(curr->value.l);
        if (curr->value.tag == JSON_OBJECT) free_json_object(curr->value.o);

        free(curr);

        curr = next;
        if (next) next = next->next;
    }
}

void json__free(json *object)
{
    PROFILE_FUNCTION_BEGIN();
    switch (object->tag)
    {
        case JSON_NONE:
        case JSON_BOOL:
        case JSON_INT:
        case JSON_FLOAT:
        case JSON_STRING:
            break;

        case JSON_LIST: free_json_list(object->l); break;
        case JSON_OBJECT: free_json_object(object->o); break;
    }
    free(object);
    PROFILE_FUNCTION_END();
}

int32 json__get_length(json *v)
{
    int32 result = 0;
    if (v->tag == JSON_LIST ||
        v->tag == JSON_OBJECT)
    {
        result = v->n;
    }
    return result;
}

float64 json__get_float(json *v)
{
    float64 result = 0;
    if (v->tag == JSON_FLOAT)
    {
        result = v->f;
    }
    return result;
}

json *json__list_lookup(json *v, int32 n)
{
    PROFILE_FUNCTION_BEGIN();
    json *result = NULL;
    if (v->tag == JSON_LIST)
    {
        json_list *l = v->l;
        for (int i = 0; i < (n / ARRAY_COUNT(((json_list *)0)->values)); i++)
            l = l->next;
        uint32 index = n % ARRAY_COUNT(((json_list *)0)->values);
        if (index < l->value_count)
        {
            result = &l->values[index];
        }
    }
    PROFILE_FUNCTION_END();
    return result;
}

json *json__object_lookup(json *v, char const *s)
{
    PROFILE_FUNCTION_BEGIN();
    json *result = NULL;
    if (v->tag == JSON_OBJECT)
    {
        for (json_object *o = v->o; o; o = o->next)
        {
            if (strncmp(o->key.s, s, o->key.n) == 0)
            {
                result = &o->value;
                break;
            }
        }
    }
    PROFILE_FUNCTION_END();
    return result;
}

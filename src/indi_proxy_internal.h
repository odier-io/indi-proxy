/*--------------------------------------------------------------------------------------------------------------------*/

#ifndef INDI_PROXY_INDI_PROXY_INTERNAL_H
#define INDI_PROXY_INDI_PROXY_INTERNAL_H

/*--------------------------------------------------------------------------------------------------------------------*/

#include "indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/

#define __USED__ __attribute__ ((unused))

/*--------------------------------------------------------------------------------------------------------------------*/
/* MEMORY                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_boolean_dup(
    bool b
);

str_t indi_double_dup(
    double d
);

str_t indi_string_dup(
    STR_t s
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* NULL                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    indi_object_t base;

} indi_null_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_null_t *indi_null_new();

void indi_null_free(
    indi_null_t *obj
);

str_t indi_null_to_string(
    indi_null_t *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* NUMBER                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    indi_object_t base;

    double data;

} indi_number_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_number_t *indi_number_new();

void indi_number_free(
    indi_number_t *obj
);

double indi_number_get(
    indi_number_t *obj
);

void indi_number_set(
    indi_number_t *obj,
    double data
);

str_t indi_number_to_string(
    indi_number_t *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/

__USED__
static inline indi_number_t *indi_number_from(double data)
{
    indi_number_t *result = indi_number_new();

    indi_number_set(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* BOOLEAN                                                                                                            */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    indi_object_t base;

    bool data;

} indi_boolean_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_boolean_t *indi_boolean_new();

void indi_boolean_free(
    indi_boolean_t *obj
);

bool indi_boolean_get(
    indi_boolean_t *obj
);

void indi_boolean_set(
    indi_boolean_t *obj,
    bool data
);

str_t indi_boolean_to_string(
    indi_boolean_t *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/

__USED__
static inline indi_boolean_t *indi_boolean_from(bool data)
{
    indi_boolean_t *result = indi_boolean_new();

    indi_boolean_set(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* STRING                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    indi_object_t base;

    struct indi_string_node_s *head;
    struct indi_string_node_s *tail;

} indi_string_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_string_t *indi_string_new();

void indi_string_free(
    indi_string_t *obj
);

void indi_string_append(
    indi_string_t *obj,
    STR_t data
);

size_t indi_string_length(
    indi_string_t *obj
);

str_t indi_string_to_string(
    indi_string_t *obj
);

str_t indi_string_to_cstring(
    indi_string_t *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/

__USED__
static inline indi_string_t *indi_string_from(STR_t data)
{
    indi_string_t *result = indi_string_new();

    indi_string_append(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* DICT                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    indi_object_t base;

    struct indi_dict_node_s *head;
    struct indi_dict_node_s *tail;

} indi_dict_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    int idx;

    indi_type_t type;

    struct indi_dict_node_s *head;

} indi_dict_iter_t;

/*--------------------------------------------------------------------------------------------------------------------*/

#define INDI_DICT_ITER(obj) \
                ((indi_dict_iter_t) {0, ((indi_dict_t *) (obj))->base.type, ((indi_dict_t *) (obj))->head})

/*--------------------------------------------------------------------------------------------------------------------*/

indi_dict_t *indi_dict_new();

void indi_dict_free(
    indi_dict_t *obj
);

void indi_dict_del(
    indi_dict_t *obj,
    STR_t key
);

bool indi_dict_iterate(
    indi_dict_iter_t *iter,
    STR_t *key,
    indi_object_t **obj
);

indi_object_t *indi_dict_get(
    indi_dict_t *obj,
    STR_t key
);

void indi_dict_put(
    indi_dict_t *obj,
    STR_t key,
    buff_t val
);

str_t indi_dict_to_string(
    indi_dict_t *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* LIST                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    indi_object_t base;

    struct indi_list_node_s *head;
    struct indi_list_node_s *tail;

} indi_list_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    int idx;

    indi_type_t type;

    struct indi_list_node_s *head;

} indi_list_iter_t;

/*--------------------------------------------------------------------------------------------------------------------*/

#define INDI_LIST_ITER(obj) \
                ((indi_list_iter_t) {0, ((indi_list_t *) (obj))->base.type, ((indi_list_t *) (obj))->head})

/*--------------------------------------------------------------------------------------------------------------------*/

indi_list_t *indi_list_new();

void indi_list_free(
    indi_list_t *obj
);

void indi_list_del(
    indi_list_t *obj,
    int idx
);

bool indi_list_iterate(
    indi_list_iter_t *iter,
    int *idx,
    indi_object_t **obj
);

indi_object_t *indi_list_get(
    indi_list_t *obj,
    int idx
);

indi_list_t *indi_list_push(
    indi_list_t *obj,
    buff_t val
);

str_t indi_list_to_string(
    indi_list_t *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* INDI_PROXY_INDI_PROXY_INTERNAL_H */

/*--------------------------------------------------------------------------------------------------------------------*/

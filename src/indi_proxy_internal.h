/*--------------------------------------------------------------------------------------------------------------------*/

#ifndef INDI_PROXY_INDI_PROXY_INTERNAL_H
#define INDI_PROXY_INDI_PROXY_INTERNAL_H

/*--------------------------------------------------------------------------------------------------------------------*/

#include "indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/* ALLOC                                                                                                              */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef enum indi_type_e
{
    INDI_TYPE_NULL      = 100,
    INDI_TYPE_BOOLEAN   = 101,
    INDI_TYPE_NUMBER    = 102,
    INDI_TYPE_STRING    = 103,
    INDI_TYPE_DICT      = 104,
    INDI_TYPE_LIST      = 105,

} indi_type_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_object_s
{
    uint32_t magic;

    enum indi_type_e type;

} indi_object_t;

/*--------------------------------------------------------------------------------------------------------------------*/

size_t indi_free(
    buff_t buff
);

buff_t indi_alloc(
    size_t size
);

buff_t indi_realloc(
    buff_t buff,
    size_t size
);

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

void indi_object_free(
    buff_t buff
);

str_t indi_object_to_string(
    BUFF_t buff
);

str_t indi_object_to_cstring(
    BUFF_t buff
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* NULL                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_null_s
{
    struct indi_object_s base;

} indi_null_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_null_s *indi_null_new();

void indi_null_free(
    struct indi_null_s *obj
);

str_t indi_null_to_string(
    struct indi_null_s *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* NUMBER                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_number_s
{
    struct indi_object_s base;

    double data;

} indi_number_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_number_s *indi_number_new();

void indi_number_free(
    struct indi_number_s *obj
);

double indi_number_get(
    struct indi_number_s *obj
);

void indi_number_set(
    struct indi_number_s *obj,
    double data
);

str_t indi_number_to_string(
    struct indi_number_s *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/

static inline struct indi_number_s *indi_number_from(double data)
{
    struct indi_number_s *result = indi_number_new();

    indi_number_set(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* BOOLEAN                                                                                                            */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_boolean_s
{
    struct indi_object_s base;

    bool data;

} indi_boolean_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_boolean_s *indi_boolean_new();

void indi_boolean_free(
    struct indi_boolean_s *obj
);

bool indi_boolean_get(
    struct indi_boolean_s *obj
);

void indi_boolean_set(
    struct indi_boolean_s *obj,
    bool data
);

str_t indi_boolean_to_string(
    struct indi_boolean_s *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/

static inline struct indi_boolean_s *indi_boolean_from(bool data)
{
    struct indi_boolean_s *result = indi_boolean_new();

    indi_boolean_set(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* STRING                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_string_s
{
    struct indi_object_s base;

    struct indi_string_node_s *head;
    struct indi_string_node_s *tail;

} indi_string_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_string_s *indi_string_new();

void indi_string_free(
    struct indi_string_s *obj
);

void indi_string_append(
    struct indi_string_s *obj,
    STR_t data
);

size_t indi_string_length(
    struct indi_string_s *obj
);

str_t indi_string_to_string(
    struct indi_string_s *obj
);

str_t indi_string_to_cstring(
    struct indi_string_s *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/

static inline struct indi_string_s *indi_string_from(STR_t data)
{
    struct indi_string_s *result = indi_string_new();

    indi_string_append(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* DICT                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_dict_s
{
    struct indi_object_s base;

    struct indi_dict_node_s *head;
    struct indi_dict_node_s *tail;

} indi_dict_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_dict_iter_s
{
    int idx;

    enum indi_type_e type;

    struct indi_dict_node_s *head;

} indi_dict_iter_t;

/*--------------------------------------------------------------------------------------------------------------------*/

#define INDI_DICT_ITER(obj) \
                ((struct indi_dict_iter_s) {0, ((struct indi_dict_s *) (obj))->base.type, ((struct indi_dict_s *) (obj))->head})

/*--------------------------------------------------------------------------------------------------------------------*/

indi_dict_t *indi_dict_new();

void indi_dict_free(
    struct indi_dict_s *obj
);

void indi_dict_del(
    struct indi_dict_s *obj,
    STR_t key
);

bool indi_dict_iterate(
    struct indi_dict_iter_s *iter,
    STR_t *key,
    indi_object_t **obj
);

struct indi_object_s *indi_dict_get(
    struct indi_dict_s *obj,
    STR_t key
);

void indi_dict_put(
    struct indi_dict_s *obj,
    STR_t key,
    buff_t val
);

str_t indi_dict_to_string(
    struct indi_dict_s *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* LIST                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_list_s
{
    struct indi_object_s base;

    struct indi_list_node_s *head;
    struct indi_list_node_s *tail;

} indi_list_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_list_iter_s
{
    int idx;

    enum indi_type_e type;

    struct indi_list_node_s *head;

} indi_list_iter_t;

/*--------------------------------------------------------------------------------------------------------------------*/

#define INDI_LIST_ITER(obj) \
                ((struct indi_list_iter_s) {0, ((struct indi_list_s *) (obj))->base.type, ((struct indi_list_s *) (obj))->head})

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_list_s *indi_list_new();

void indi_list_free(
    struct indi_list_s *obj
);

void indi_list_del(
    struct indi_list_s *obj,
    int idx
);

bool indi_list_iterate(
    struct indi_list_iter_s *iter,
    int *idx,
    indi_object_t **obj
);

struct indi_object_s *indi_list_get(
    struct indi_list_s *obj,
    int idx
);

struct indi_list_s *indi_list_push(
    struct indi_list_s *obj,
    buff_t val
);

str_t indi_list_to_string(
    struct indi_list_s *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* JSON                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_object_s *indi_json_parse(
    STR_t json
);

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* INDI_PROXY_INDI_PROXY_INTERNAL_H */

/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

indi_number_t *indi_number_new()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_number_t *obj = indi_memory_alloc(sizeof(indi_number_t));

    /*----------------------------------------------------------------------------------------------------------------*/
    
    obj->base.magic = INDI_OBJECT_MAGIC;
    obj->base.type = INDI_TYPE_NUMBER;

    /*----------------------------------------------------------------------------------------------------------------*/

    obj->data = 0.0;

    /*----------------------------------------------------------------------------------------------------------------*/

    return obj;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_number_free(indi_number_t *obj)
{
    indi_memory_free(obj);
}

/*--------------------------------------------------------------------------------------------------------------------*/

double indi_number_get(indi_number_t *obj)
{
    return obj->data;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_number_set(indi_number_t *obj, double data)
{
    obj->data = data;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_number_to_string(indi_number_t *obj)
{
    return indi_double_dup(obj->data);
}

/*--------------------------------------------------------------------------------------------------------------------*/

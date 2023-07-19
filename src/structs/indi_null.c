/*--------------------------------------------------------------------------------------------------------------------*/

#include "../indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

indi_null_t *indi_null_new()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_null_t *obj = indi_alloc(sizeof(indi_null_t));

    /*----------------------------------------------------------------------------------------------------------------*/

    obj->object.magic = 0x65656565;
    obj->object.type = INDI_TYPE_NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    return obj;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_null_free(indi_null_t *obj)
{
    indi_free(obj);
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_null_to_string(indi_null_t *obj)
{
    return indi_string_dup("null");
}

/*--------------------------------------------------------------------------------------------------------------------*/

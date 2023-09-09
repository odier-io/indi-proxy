/*--------------------------------------------------------------------------------------------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef HAVE_MALLOC_USABLE_SIZE
size_t malloc_usable_size(buff_t);
#endif

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef HAVE_MALLOC_USABLE_SIZE
static size_t used_mem = 0;
#endif

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void indi_memory_initialize()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    xmlMemSetup(
        (buff_t) indi_memory_free,
        (buff_t) indi_memory_alloc,
        (buff_t) indi_memory_realloc,
        (buff_t) indi_string_dup
    );

    /*----------------------------------------------------------------------------------------------------------------*/

    #ifdef HAVE_MALLOC_USABLE_SIZE
    used_mem = 0;
    #endif

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_memory_finalize()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    xmlCleanupParser();

    /*----------------------------------------------------------------------------------------------------------------*/

    #ifdef HAVE_MALLOC_USABLE_SIZE
    if(used_mem > 0) fprintf(stderr, "Memory leak: %ld bytes!\n", used_mem);
    #endif

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

size_t indi_memory_free(buff_t buff)
{
    if(buff == NULL)
    {
        return 0x00;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    #ifdef HAVE_MALLOC_USABLE_SIZE
    size_t result = malloc_usable_size(buff);

    used_mem -= result;
    #endif

    /*----------------------------------------------------------------------------------------------------------------*/

    free(buff);

    /*----------------------------------------------------------------------------------------------------------------*/

    #ifdef HAVE_MALLOC_USABLE_SIZE
    return result;
    #else
    return 0x0000;
    #endif
}

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t indi_memory_alloc(size_t size)
{
    if(size == 0x00)
    {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    buff_t result = malloc(size);

    if(result == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        fflush(stderr);
        exit(1);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    #ifdef HAVE_MALLOC_USABLE_SIZE
    used_mem += malloc_usable_size(result);
    #endif

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t indi_memory_realloc(buff_t buff, size_t size)
{
    if(buff == NULL) {
        return indi_memory_alloc(size);
    }

    if(size == 0x00) {
        return /*-------------*/(NULL);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    #ifdef HAVE_MALLOC_USABLE_SIZE
    used_mem -= malloc_usable_size(buff);
    #endif

    /*----------------------------------------------------------------------------------------------------------------*/

    buff_t result = realloc(buff, size);

    if(result == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        fflush(stderr);
        exit(1);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    #ifdef HAVE_MALLOC_USABLE_SIZE
    used_mem += malloc_usable_size(result);
    #endif

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_boolean_dup(bool b)
{
    str_t str;

    if(b) {
        str = strcpy(indi_memory_alloc(4 + 1), "true");
    }
    else {
        str = strcpy(indi_memory_alloc(5 + 1), "false");
    }

    return str;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_double_dup(double d)
{
    if(!isnan(d))
    {
        str_t str = indi_memory_alloc(32 + 1);

        snprintf(str, 32, "%lf", d);

        return str;
    }

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_string_dup(STR_t s)
{
    if(s != NULL)
    {
        str_t str = indi_memory_alloc(strlen(s));

        strcpy(str, s);

        return str;
    }

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void indi_object_free(indi_object_t *obj)
{
    if(obj == NULL)
    {
        return;
    }

    if(obj->magic != INDI_OBJECT_MAGIC)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(obj->type)
    {
        case INDI_TYPE_NULL:
            indi_null_free((indi_null_t *) obj);
            break;

        case INDI_TYPE_NUMBER:
            indi_number_free((indi_number_t *) obj);
            break;

        case INDI_TYPE_BOOLEAN:
            indi_boolean_free((indi_boolean_t *) obj);
            break;

        case INDI_TYPE_STRING:
            indi_string_free((indi_string_t *) obj);
            break;

        case INDI_TYPE_LIST:
            indi_list_free((indi_list_t *) obj);
            break;

        case INDI_TYPE_DICT:
            indi_dict_free((indi_dict_t *) obj);
            break;

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_object_to_string(const indi_object_t *obj)
{
    if(obj == NULL)
    {
        return NULL;
    }

    if(obj->magic != INDI_OBJECT_MAGIC)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(obj->type)
    {
        case INDI_TYPE_NULL:
            return indi_null_to_string((indi_null_t *) obj);

        case INDI_TYPE_NUMBER:
            return indi_number_to_string((indi_number_t *) obj);

        case INDI_TYPE_BOOLEAN:
            return indi_boolean_to_string((indi_boolean_t *) obj);

        case INDI_TYPE_STRING:
            return indi_string_to_string((indi_string_t *) obj);

        case INDI_TYPE_LIST:
            return indi_list_to_string((indi_list_t *) obj);

        case INDI_TYPE_DICT:
            return indi_dict_to_string((indi_dict_t *) obj);

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_object_to_cstring(const indi_object_t *obj)
{
    if(obj == NULL)
    {
        return NULL;
    }

    if(obj->magic != INDI_OBJECT_MAGIC)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(obj->type)
    {
        case INDI_TYPE_NULL:
            return indi_null_to_string((indi_null_t *) obj);

        case INDI_TYPE_NUMBER:
            return indi_number_to_string((indi_number_t *) obj);

        case INDI_TYPE_BOOLEAN:
            return indi_boolean_to_string((indi_boolean_t *) obj);

        case INDI_TYPE_STRING:
            return indi_string_to_cstring((indi_string_t *) obj);

        case INDI_TYPE_LIST:
            return indi_list_to_string((indi_list_t *) obj);

        case INDI_TYPE_DICT:
            return indi_dict_to_string((indi_dict_t *) obj);

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

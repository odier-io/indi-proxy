/*--------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

#include "../indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_dict_node_s
{
    STR_t key;

    struct indi_object_s *val;

    struct indi_dict_node_s *next;

} node_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_dict_t *indi_dict_new()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_t *obj = indi_alloc(sizeof(indi_dict_t));

    /*----------------------------------------------------------------------------------------------------------------*/

    obj->object.magic = 0x65656565;
    obj->object.type = INDI_TYPE_DICT;

    /*----------------------------------------------------------------------------------------------------------------*/

    obj->head = NULL;
    obj->tail = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    return obj;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_dict_free(indi_dict_t *obj)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *node = obj->head; node != NULL;)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        node_t *temp = node;

        node = node->next;

        /*------------------------------------------------------------------------------------------------------------*/

        indi_object_free(temp->val);

        indi_free(temp);

        /*------------------------------------------------------------------------------------------------------------*/
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_free(obj);

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_dict_del(indi_dict_t *obj, STR_t key)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *prev_node = NULL, *curr_node = obj->head; curr_node != NULL; prev_node = curr_node, curr_node = curr_node->next)
    {
        if(strcmp(curr_node->key, key) == 0)
        {
            /*--------------------------------------------------------------------------------------------------------*/

            if(prev_node == NULL)
            {
                obj->head = curr_node->next;
            }
            else
            {
                prev_node->next = curr_node->next;
            }

            /*--------------------------------------------------------------------------------------------------------*/

            indi_object_free(curr_node->val);

            indi_free(curr_node);

            /*--------------------------------------------------------------------------------------------------------*/

            break;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

bool indi_dict_iterate(indi_dict_iter_t *i, STR_t *key, indi_object_t **obj)
{
    if(i->type == INDI_TYPE_DICT && i->head != NULL)
    {
        if(key != NULL) {
            *key = i->head->key;
        }

        if(obj != NULL) {
            *obj = i->head->val;
        }

        i->idx += 0x0000000001;
        i->head = i->head->next;

        return true;
    }

    return false;
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_object_t *indi_dict_get(indi_dict_t *obj, STR_t key)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *curr_node = obj->head; curr_node != NULL; curr_node = curr_node->next)
    {
        if(strcmp(curr_node->key, key) == 0)
        {
            return curr_node->val;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_dict_put(indi_dict_t *obj, STR_t key, buff_t val)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *curr_node = obj->head; curr_node != NULL; curr_node = curr_node->next)
    {
        if(strcmp(curr_node->key, key) == 0)
        {
            indi_object_free(curr_node->val);

            curr_node->val = val;

            return;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    node_t *node = indi_alloc(sizeof(node_t) + strlen(key) + 1);

    node->key = strcpy((str_t) (node + 1), key);

    node->val = val;
    node->next = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    if(obj->head == NULL)
    {
        obj->head = node;
        obj->tail = node;
    }
    else
    {
        obj->tail->next = node;
        obj->tail /*-*/ = node;
    }

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_dict_to_string(indi_dict_t *obj)
{
    indi_string_t *string = indi_string_new();

    /**/    indi_string_append(string, "{");
    /**/
    /**/    for(node_t *curr_node = obj->head; curr_node != NULL; curr_node = curr_node->next)
    /**/    {
    /**/        str_t curr_node_val = indi_object_to_string(curr_node->val);
    /**/
    /**/        /**/    indi_string_append(string, "\"");
    /**/        /**/    indi_string_append(string, curr_node->key);
    /**/        /**/    indi_string_append(string, "\"");
    /**/        /**/    indi_string_append(string, ":");
    /**/        /**/    indi_string_append(string, curr_node_val);
    /**/
    /**/        indi_free(curr_node_val);
    /**/
    /**/        if(curr_node->next != NULL)
    /**/        {
    /**/            indi_string_append(string, ",");
    /**/        }
    /**/    }
    /**/
    /**/    indi_string_append(string, "}");

    str_t result = indi_string_to_cstring(string);

    indi_string_free(string);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

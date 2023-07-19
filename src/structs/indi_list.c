/*--------------------------------------------------------------------------------------------------------------------*/

#include "../indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_list_node_s
{
    struct indi_object_s *val;

    struct indi_list_node_s *next;

} node_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_list_t *indi_list_new()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_list_t *obj = indi_alloc(sizeof(indi_list_t));

    /*----------------------------------------------------------------------------------------------------------------*/

    obj->object.magic = 0x65656565;
    obj->object.type = INDI_TYPE_LIST;

    /*----------------------------------------------------------------------------------------------------------------*/

    obj->head = NULL;
    obj->tail = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    return obj;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_list_free(indi_list_t *obj)
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

void indi_list_del(indi_list_t *obj, int idx)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    int i = 0;

    for(node_t *prev_node = NULL, *curr_node = obj->head; curr_node != NULL; prev_node = curr_node, curr_node = curr_node->next)
    {
        if(idx == i++)
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

bool indi_list_iterate(indi_list_iter_t *i, int *idx, indi_object_t **obj)
{
    if(i->type == INDI_TYPE_LIST && i->head != NULL)
    {
        if(idx != NULL) {
            *idx = i->idx;
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

indi_object_t *indi_list_get(indi_list_t *obj, int idx)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    int i = 0;

    for(node_t *curr_node = obj->head; curr_node != NULL; curr_node = curr_node->next)
    {
        if(idx == i++)
        {
            return curr_node->val;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_list_t *indi_list_push(struct indi_list_s *obj, buff_t val)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    node_t *node = indi_alloc(sizeof(node_t));

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

    return obj;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_list_to_string(indi_list_t *obj)
{
    indi_string_t *sb = indi_string_new();

    /**/    indi_string_append(sb, "[");
    /**/
    /**/    for(node_t *curr_node = obj->head; curr_node != NULL; curr_node = curr_node->next)
    /**/    {
    /**/        str_t curr_node_val = indi_object_to_string(curr_node->val);
    /**/
    /**/        /**/    indi_string_append(sb, curr_node_val);
    /**/
    /**/        indi_free(curr_node_val);
    /**/
    /**/        if(curr_node->next != NULL)
    /**/        {
    /**/            indi_string_append(sb, ",");
    /**/        }
    /**/    }
    /**/
    /**/    indi_string_append(sb, "]");

    str_t result = indi_string_to_cstring(sb);

    indi_string_free(sb);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

#include <libxml/tree.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static xmlNode *json_to_xml(xmlNode *node, indi_dict_t *dict) // NOLINT(misc-no-recursion)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    STR_t key;

    indi_object_t *obj1;

    for(indi_dict_iter_t iter1 = INDI_DICT_ITER(dict); indi_dict_iterate(&iter1, &key, &obj1);)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        /**/ if(key[0] == '$')
        {
            str_t val = indi_object_to_cstring(obj1);

            xmlNodeSetContent(node, /*--------*/ BAD_CAST val);

            indi_free(val);
        }

        /*------------------------------------------------------------------------------------------------------------*/

        else if(key[0] == '@')
        {
            str_t val = indi_object_to_cstring(obj1);

            xmlNewProp(node, BAD_CAST (key + 1), BAD_CAST val);

            indi_free(val);
        }

        /*------------------------------------------------------------------------------------------------------------*/

        else
        {
            int idx;

            indi_object_t *obj2;

            for(indi_list_iter_t iter2 = INDI_LIST_ITER(obj1); indi_list_iterate(&iter2, &idx, &obj2);)
            {
                json_to_xml(xmlNewChild(node, NULL, BAD_CAST key, NULL), (indi_dict_t *) obj2);
            }
        }

        /*------------------------------------------------------------------------------------------------------------*/
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return node;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static str_t xmlDump(xmlDoc *doc)
{
    int len;

    str_t result;

    xmlDocDumpMemoryEnc(doc, (xmlChar **) &result, &len, "iso-8859-1");

    memmove(result, result + 44, len - 44 + 1);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_json_to_xml(STR_t json, bool validate)
{
    str_t result;

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_t *dict = (indi_dict_t *) indi_json_parse(json);

    if(dict == NULL)
    {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    xmlDoc *doc = xmlNewDoc(BAD_CAST "1.0"); xmlDocSetRootElement(doc, json_to_xml(xmlNewNode(NULL, BAD_CAST "defSwitchVector"), dict));

    result = validate == false || indi_validation_check(doc) == true ? xmlDump(doc) : NULL;

    xmlFreeDoc(doc);

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_object_free(dict);

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

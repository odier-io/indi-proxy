/*--------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

#include <libxml/tree.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static xmlNode *json_to_xml(xmlNode *node, indi_dict_t *dict) // NOLINT(misc-no-recursion)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    STR_t key;

    indi_object_t *object;

    for(indi_dict_iter_t iter = INDI_DICT_ITER(dict); indi_dict_iterate(&iter, &key, &object);)
    {
        /**/ if(key[0] == "$")
        {
            str_t val = indi_object_to_cstring(object);

            xmlNodeSetContent(node, BAD_CAST val);

            indi_free(val);
        }
        else if(key[0] == '@')
        {
            str_t val = indi_object_to_cstring(object);

            xmlNewProp(node, BAD_CAST (key + 1), BAD_CAST val);

            indi_free(val);
        }
        else
        {

        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return node;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_json_to_xml(STR_t json, bool validate)
{
    str_t result = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_t *dict = (indi_dict_t *) indi_json_parse(json);

    if(dict == NULL)
    {
        fprintf(stderr, "Invalid JSON document\n");

        goto _err0;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    xmlNode *root = xmlNewNode(NULL, BAD_CAST "root");

    json_to_xml(root, dict);

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlDocSetRootElement(doc, root);

    xmlChar *xml_string;
    xmlDocDumpFormatMemory(doc, &xml_string, NULL, 1);

    printf("%s\n", xml_string);

    /*----------------------------------------------------------------------------------------------------------------*/

//_err1:
    indi_object_free(dict);

_err0:
    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

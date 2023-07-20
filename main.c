#include <stdio.h>

#include <libxml/parser.h>

#include "src/indi_proxy_internal.h"

void indi_free2(buff_t buff)
{
    indi_free(buff);
}

int main()
{
    xmlMemSetup(indi_free2, indi_alloc, indi_realloc, indi_string_dup);

    if(indi_validation_init() == false)
    {
        printf("Error initializing validation\n");
    }

    STR_t xml1 = "/<defSwitchVector device=\"CCD Simulator\" name=\"CONNECTION\" label=\"Connection\" group=\"Main Control\" state=\"Ok\" perm=\"rw\" rule=\"OneOfMany\" timeout=\"60\" timestamp=\"2023-07-17T06:46:07\"><defSwitch name=\"CONNECT\" label=\"Connect\">On</defSwitch><defSwitch name=\"DISCONNECT\" label=\"Disconnect\">Off</defSwitch></defSwitchVector>";

    printf("xml1: %s\n\n", xml1);

    indi_xmldoc_t *xmldoc = indi_xml_parse(xml1);
    str_t json1 = indi_object_to_string(indi_xml_to_object(xmldoc, true));
    indi_xml_free(xmldoc);

    printf("json1: %s\n\n", json1);

    indi_object_t *test = indi_json_parse(json1);
    str_t json2 = indi_object_to_string(test);
    indi_object_free(test);

    printf("json2: %s\n\n", json2);

    indi_object_t *obj = indi_json_parse(json2);
    str_t xml2 = indi_xml_to_string(indi_object_to_xml(obj, false));
    indi_object_free(obj);

    printf("xml2: %s\n\n", xml2);

    indi_free(xml2);
    indi_free(json2);
    indi_free(json1);

    if(indi_validation_free() == false)
    {
        printf("Error finalizing validation\n");
    }

    xmlCleanupParser();

    indi_memory_report();

    return 0;
}
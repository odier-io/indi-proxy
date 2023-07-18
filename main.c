#include <stdio.h>

#include <libxml/parser.h>

#include "indi_proxy_internal.h"

int main()
{
    if(indi_validation_init() != 0)
    {
        printf("Error initializing validation\n");
    }

    str_t json = indi_xml_to_json("<defSwitchVector device=\"CCD Simulator\" name=\"CONNECTION\" label=\"Connection\" group=\"Main Control\" state=\"Ok\" perm=\"rw\" rule=\"OneOfMany\" timeout=\"60\" timestamp=\"2023-07-17T06:46:07\"><defSwitch name=\"CONNECT\" label=\"Connect\">On</defSwitch><defSwitch name=\"DISCONNECT\" label=\"Disconnect\">Off</defSwitch></defSwitchVector>", true);

    printf("%s\n", json);

    indi_free(json);

    if(indi_validation_free() != 0)
    {
        printf("Error finalizing validation\n");
    }

    indi_memory_report();

    xmlCleanupParser();
    xmlMemoryDump();

    return 0;
}

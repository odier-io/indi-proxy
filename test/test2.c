/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "../src/indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/

int main()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_memory_initialize();

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_proxy_t proxy;

    indi_proxy_initialize(&proxy, 400, 400, NULL);

    for(int i = 0; i < 50; i++)
    {
        printf("\nStep %d]:\n", i);

        indi_proxy_consume(
            &proxy,
            strlen("....................................................<defSwitchVector>foo"),
            "....................................................<defSwitchVector>foo"
        );

        indi_proxy_consume(
            &proxy,
            strlen("</defSwitchVector>........"),
            "</defSwitchVector>........"
        );

        indi_proxy_consume(
            &proxy,
            strlen("...........................................<defNumberVector>foo"),
            "...........................................<defNumberVector>foo"
        );

        indi_proxy_consume(
            &proxy,
            strlen("</defNumberVector>................................"),
            "</defNumberVector>................................"
        );

        indi_proxy_consume(
            &proxy,
            strlen("<message device=\"Filter Simulator\" timestamp=\"2023-07-29T17:12:35\" message=\"[INFO] Setting current filter to slot 5\"/><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Busy\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">4</oneNumber></setNumberVector><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Ok\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">5</oneNumber></setNumberVector>"),
            "<message device=\"Filter Simulator\" timestamp=\"2023-07-29T17:12:35\" message=\"[INFO] Setting current filter to slot 5\"/><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Busy\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">4</oneNumber></setNumberVector><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Ok\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">5</oneNumber></setNumberVector>"
        );
/*
        indi_proxy_consume(
            &proxy,
            strlen("<message device=\"Filter Simulator\" timestamp=\"2023-07-29T17:12:35\" message=\"[INFO] Setting current filter to slot 5\"/><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Busy\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">4</oneNumber></setNumberVector><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Ok\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">5</oneNumber></setNumberVector>"),
            "<message device=\"Filter Simulator\" timestamp=\"2023-07-29T17:12:35\" message=\"[INFO] Setting current filter to slot 5\"/><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Busy\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">4</oneNumber></setNumberVector><setNumberVector device=\"Filter Simulator\" name=\"FILTER_SLOT\" state=\"Ok\" timeout=\"60\" timestamp=\"2023-07-29T17:12:35\"><oneNumber name=\"FILTER_SLOT_VALUE\">5</oneNumber></setNumberVector>"
        );
*/
    }

    indi_proxy_finalize(&proxy);

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_memory_finalize();

    /*----------------------------------------------------------------------------------------------------------------*/

    printf("[SUCCESS]\n");

    return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

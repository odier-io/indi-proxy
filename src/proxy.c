/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

#define STATE_OUTSIDE 100
#define STATE_INSIDE1 200
#define STATE_INSIDE2 300
#define STATE_INSIDE3 400
#define STATE_INSIDE4 500
#define STATE_INSIDE5 600

/*--------------------------------------------------------------------------------------------------------------------*/

static void fake_emit(__USED__ indi_proxy_t *proxy, __USED__ size_t size, str_t buff)
{
    printf("|%s|\n", buff);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_proxy_initialize(indi_proxy_t *proxy, size_t xml_stream_size, size_t xml_residual_size, indi_emit_func_t emit_func)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->emit_func = (emit_func != NULL) ? emit_func
                                           : fake_emit
    ;

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->xml_stream_size = /*-------------*/(xml_stream_size + 0);
    proxy->xml_stream_buff = indi_memory_alloc(xml_stream_size + 1);

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->xml_residual_size = /*-------------*/(0x000000000000000 + 0);
    proxy->xml_residual_buff = indi_memory_alloc(xml_residual_size + 1);

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->etag_size = 0x00;
    proxy->etag_buff = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->state = STATE_OUTSIDE;
    proxy->pos = 0x000000;
    proxy->py = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_proxy_finalize(indi_proxy_t *proxy)
{
    indi_memory_free(proxy->xml_stream_buff);

    indi_memory_free(proxy->xml_residual_buff);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static STR_t S_TAGS[] = {
    /* Getters */
    "<defTextVector",
    "<defNumberVector",
    "<defSwitchVector",
    "<defLightVector",
    "<defBLOBVector",
    /* Setters */
    "<setTextVector",
    "<setNumberVector",
    "<setSwitchVector",
    "<setLightVector",
    "<setBLOBVector",
    /* Others */
    "<message",
    "<delProperty",
};

/*--------------------------------------------------------------------------------------------------------------------*/

static STR_t E_TAGS[] = {
    /* Getters */
    "</defTextVector>",
    "</defNumberVector>",
    "</defSwitchVector>",
    "</defLightVector>",
    "</defBLOBVector>",
    /* Setters */
    "</setTextVector>",
    "</setNumberVector>",
    "</setSwitchVector>",
    "</setLightVector>",
    "</setBLOBVector>",
    /* Others */
    "/>",
    "/>",
};

/*--------------------------------------------------------------------------------------------------------------------*/

#define TAG_NB (sizeof(S_TAGS) / sizeof(STR_t))

/*--------------------------------------------------------------------------------------------------------------------*/

#define PREVENT_OVERFLOW(size) \
                                                    \
            if(proxy->pos + (size) > STREAM_SIZE)    \
            {                                       \
                fprintf(stderr, "Overflow!\n");     \
                                                    \
                proxy->state = STATE_OUTSIDE;       \
                proxy->pos = 0x00000000000;         \
                                                    \
                return;                             \
            }

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_proxy_consume(indi_proxy_t *proxy, size_t size, STR_t buff)
{
    bool all_tested;

    size_t input_pos = 0;

    size_t INPUT_SIZE = size;
    size_t STREAM_SIZE = proxy->xml_stream_size;

    STR_t input_ptr = buff + 0x0000000;
    str_t stream_ptr = proxy->xml_stream_buff + proxy->pos;

    /*----------------------------------------------------------------------------------------------------------------*/

    if(proxy->xml_residual_size > 0)
    {
        INPUT_SIZE += proxy->xml_residual_size;
        input_ptr = proxy->xml_residual_buff;

        memcpy(proxy->xml_residual_buff + proxy->xml_residual_size, buff, size + 1);

        proxy->xml_residual_size = 0;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    for(;;)
    {
        switch(proxy->state)
        {
            /*--------------------------------------------------------------------------------------------------------*/

            _label_outside:
            case STATE_OUTSIDE:

                while(input_pos < INPUT_SIZE)
                {
                    if(*input_ptr != '<')
                    {
                        /* skip character */

                        input_ptr++;
                        input_pos++;
                    }
                    else
                    {
                        proxy->state = STATE_INSIDE1;
                        goto _label_inside1;
                    }
                }

                return;

            /*--------------------------------------------------------------------------------------------------------*/

            _label_inside1:
            case STATE_INSIDE1:

                all_tested = true;

                for(size_t k = 0; k < TAG_NB; k++)
                {
                    size_t tag_size = strlen(S_TAGS[k]);
                    BUFF_t tag_buff = /*--*/(S_TAGS[k]);

                    if(input_pos + tag_size <= INPUT_SIZE)
                    {
                        if(memcmp(input_ptr, tag_buff, tag_size) == 0)
                        {
                            PREVENT_OVERFLOW(tag_size) memcpy(stream_ptr, tag_buff, tag_size);

                            input_ptr += tag_size;
                            input_pos += tag_size;

                            stream_ptr += tag_size;
                            proxy->pos += tag_size;

                            proxy->etag_size = strlen(E_TAGS[k]);
                            proxy->etag_buff = /*--*/(E_TAGS[k]);

                            if(k < 10)
                            {
                                proxy->state = STATE_INSIDE2;
                                goto _label_inside2;
                            }
                            else
                            {
                                proxy->state = STATE_INSIDE3;
                                goto _label_inside3;
                            }
                        }
                    }
                    else
                    {
                        all_tested = false;
                    }
                }

                if(all_tested)
                {
                    /* skip character */

                    input_ptr++;
                    input_pos++;

                    proxy->state = STATE_OUTSIDE;
                    goto _label_outside;
                }

                memmove(proxy->xml_residual_buff, input_ptr, proxy->xml_residual_size = INPUT_SIZE - input_pos);

                return;

            /*--------------------------------------------------------------------------------------------------------*/

            _label_inside2:
            case STATE_INSIDE2:

                while(input_pos < INPUT_SIZE)
                {
                    if(*input_ptr != '<')
                    {
                        PREVENT_OVERFLOW(1) *stream_ptr++ = *input_ptr++;

                        input_pos++;
                        proxy->pos++;
                    }
                    else
                    {
                        proxy->state = STATE_INSIDE4;
                        goto _label_inside4;
                    }
                }

                return;

            /*--------------------------------------------------------------------------------------------------------*/

            _label_inside3:
            case STATE_INSIDE3:

                while(input_pos < INPUT_SIZE)
                {
                    if(*input_ptr != '/')
                    {
                        PREVENT_OVERFLOW(1) *stream_ptr++ = *input_ptr++;

                        input_pos++;
                        proxy->pos++;
                    }
                    else
                    {
                        proxy->state = STATE_INSIDE4;
                        goto _label_inside4;
                    }
                }

                return;

            /*--------------------------------------------------------------------------------------------------------*/

            _label_inside4:
            case STATE_INSIDE4:

                if(input_pos + proxy->etag_size <= INPUT_SIZE)
                {
                    if(memcmp(input_ptr, proxy->etag_buff, proxy->etag_size) == 0)
                    {
                        PREVENT_OVERFLOW(proxy->etag_size) memcpy(stream_ptr, proxy->etag_buff, proxy->etag_size);

                        input_ptr += proxy->etag_size;
                        input_pos += proxy->etag_size;

                        stream_ptr += proxy->etag_size;
                        proxy->pos += proxy->etag_size;

                        proxy->state = STATE_INSIDE5;
                        goto _label_inside5;
                    }
                    else
                    {
                        PREVENT_OVERFLOW(1) *stream_ptr++ = *input_ptr++;

                        input_pos++;
                        proxy->pos++;

                        proxy->state = STATE_INSIDE2;
                        goto _label_inside2;
                    }
                }

                memmove(proxy->xml_residual_buff, input_ptr, proxy->xml_residual_size = INPUT_SIZE - input_pos);

                return;

            /*--------------------------------------------------------------------------------------------------------*/

            _label_inside5:
            case STATE_INSIDE5:

                *stream_ptr = '\0';

                proxy->emit_func(proxy, proxy->pos, proxy->xml_stream_buff);

                stream_ptr = proxy->xml_stream_buff;
                proxy->pos = 0x00000000000000000;

                proxy->state = STATE_OUTSIDE;
                goto _label_outside;

            /*--------------------------------------------------------------------------------------------------------*/
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

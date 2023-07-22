/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static void fake_emit(str_t message)
{
    printf("|%s|\n", message);
}

/*--------------------------------------------------------------------------------------------------------------------*/

bool indi_proxy_initialize(indi_proxy_t *proxy, indi_emit_func_t emit_func, size_t size)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->emit_func = (emit_func != NULL) ? emit_func
                                           : fake_emit
    ;

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->size = /*------*/(size + 0);
    proxy->buff = indi_alloc(size + 1);

    memset(proxy->buff, 0x00, size + 1);

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->read_size = 0;
    proxy->write_size = 0;

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->read_idx = 0;
    proxy->write_idx = 0;
    proxy->stag_idx = 0;

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->etag = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    return true;
}

/*--------------------------------------------------------------------------------------------------------------------*/

bool indi_proxy_finalize(indi_proxy_t *proxy)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_free(proxy->buff);

    /*----------------------------------------------------------------------------------------------------------------*/

    return true;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static __inline__ size_t _min(size_t x, size_t y)
{
    return x < y ? x : y;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static size_t _ring_write(indi_proxy_t *proxy, size_t raw_size, STR_t raw_buff)
{
    const size_t ring_size = proxy->size;
    /*-*/ str_t ring_buff = proxy->buff;

    /*----------------------------------------------------------------------------------------------------------------*/

    /*-*/ size_t data_size;
    const str_t data_buff;

    if(raw_size > ring_size)
    {
        data_size = ring_size;
        data_buff = raw_buff + (raw_size - ring_size);
    }
    else
    {
        data_size = raw_size;
        data_buff = raw_buff + 0x00000000000000000000;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    const size_t write_idx = proxy->write_idx;

    /*----------------------------------------------------------------------------------------------------------------*/

    const size_t size_right = _min(data_size, ring_size - write_idx);
    const size_t size_left = _min(data_size - size_right, write_idx);

    /*----------------------------------------------------------------------------------------------------------------*/

    if(size_right > 0)
    {
        memcpy(ring_buff + write_idx, data_buff, size_right);

        proxy->write_idx = write_idx + size_right;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(size_left > 0)
    {
        memcpy(ring_buff, data_buff + size_right, size_left);

        proxy->write_idx = 0x0000000 + size_left;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    proxy->write_size += data_size;

    /*----------------------------------------------------------------------------------------------------------------*/

    return data_size;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static bool ring_find_symbol(indi_proxy_t *proxy, const int symbol)
{
    size_t l = 0;

    STR_t buffer_ptr;
    STR_t buffer_end;

    const size_t ring_size = proxy->size;
    const str_t ring_buff = proxy->buff;

    const size_t read_idx = proxy->read_idx;
    const size_t write_idx = proxy->write_idx;

    /*----------------------------------------------------------------------------------------------------------------*/

    const size_t size_right = write_idx > read_idx ? write_idx - read_idx : ring_size - read_idx;
    const size_t size_left = write_idx < read_idx ? write_idx : 0;

    /*----------------------------------------------------------------------------------------------------------------*/

    buffer_ptr = (STR_t) ring_buff + read_idx + 0x00000000;
    buffer_end = (STR_t) ring_buff + read_idx + size_right;

    for(; buffer_ptr != buffer_end; buffer_ptr++, l++)
    {
        if(*buffer_ptr == symbol)
        {
            proxy->read_size += l;

            proxy->read_idx = (size_t) buffer_ptr - (size_t) ring_buff;

            while(proxy->read_idx > ring_size) proxy->read_idx -= ring_size;

            return true;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    buffer_ptr = (STR_t) ring_buff + 0x0000000;
    buffer_end = (STR_t) ring_buff + size_left;

    for(; buffer_ptr < buffer_end; buffer_ptr++, l++)
    {
        if(*buffer_ptr == symbol)
        {
            proxy->read_size += l;

            proxy->read_idx = (size_t) buffer_ptr - (size_t) ring_buff;

            while(proxy->read_idx > ring_size) proxy->read_idx -= ring_size;

            return true;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return false;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int ring_find_string(indi_proxy_t *proxy, STR_t s)
{
    size_t l = 0;

    STR_t buffer_ptr;
    STR_t buffer_end;

    const size_t length = strlen(s);

    const size_t ring_size = proxy->size;
    const str_t ring_buff = proxy->buff;

    const size_t read_idx = proxy->read_idx;
    const size_t write_idx = proxy->write_idx;

    /*----------------------------------------------------------------------------------------------------------------*/

    const size_t size_right = write_idx > read_idx ? write_idx - read_idx : ring_size - read_idx;
    const size_t size_left = write_idx < read_idx ? write_idx : 0;

    /*----------------------------------------------------------------------------------------------------------------*/

    buffer_ptr = (STR_t) ring_buff + read_idx + 0x00000000;
    buffer_end = (STR_t) ring_buff + read_idx + size_right;

    for(; buffer_ptr != buffer_end && l < length; buffer_ptr++, l++)
    {
        if(*buffer_ptr != s[l])
        {
            break;
        }
    }

    if(l == length)
    {
        proxy->read_size += length;

        proxy->read_idx = (size_t) buffer_ptr - (size_t) ring_buff;

        while(proxy->read_idx > ring_size) proxy->read_idx -= ring_size;

        return true;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    buffer_ptr = (STR_t) ring_buff + 0x0000000;
    buffer_end = (STR_t) ring_buff + size_left;

    for(; buffer_ptr != buffer_end && l < length; buffer_ptr++, l++)
    {
        if(*buffer_ptr != s[l])
        {
            break;
        }
    }

    if(l == length)
    {
        proxy->read_size += length;

        proxy->read_idx = (size_t) buffer_ptr - (size_t) ring_buff;

        while(proxy->read_idx > ring_size) proxy->read_idx -= ring_size;

        return true;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return false;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static STR_t STAG_NAMES[] = {
    "<defTextVector",
    "<defNumberVector",
    "<defSwitchVector",
    "<defLightVector",
    "<defBLOBVector",
};

static STR_t ETAG_NAMES[] = {
    "</defTextVector>",
    "</defNumberVector>",
    "</defSwitchVector>",
    "</defLightVector>",
    "</defBLOBVector>",
};

/*--------------------------------------------------------------------------------------------------------------------*/

static STR_t ring_find_xml_tags(indi_proxy_t *proxy)
{
    for(size_t i = 0; i < sizeof(STAG_NAMES) / sizeof(STAG_NAMES[0]); i++)
    {
        if(ring_find_string(proxy, STAG_NAMES[i]))
        {
            return ETAG_NAMES[i];
        }
    }

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_proxy_consume(indi_proxy_t *proxy, size_t size, STR_t buff)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    _ring_write(proxy, size, buff);

    /*----------------------------------------------------------------------------------------------------------------*/

    for(;;)
    {
        if(proxy->etag == NULL)
        {
            /*--------------------------------------------------------------------------------------------------------*/

            if(ring_find_symbol(proxy, '<') == false) {
                break;
            }

            proxy->stag_idx = proxy->read_idx;

            if((proxy->etag = ring_find_xml_tags(proxy)) == NULL) {
                break;
            }

            /*--------------------------------------------------------------------------------------------------------*/
        }
        else
        {
            /*--------------------------------------------------------------------------------------------------------*/

            if(ring_find_symbol(proxy, '<') == false) {
                break;
            }

            /*------------------------------*/

            if(ring_find_string(proxy, proxy->etag) == false) {
                break;
            }

            /*--------------------------------------------------------------------------------------------------------*/

            const size_t size_right = proxy->read_idx > proxy->stag_idx ? proxy->read_idx - proxy->stag_idx : proxy->size - proxy->stag_idx;
            const size_t size_left = proxy->read_idx < proxy->stag_idx ? proxy->read_idx : 0;

            str_t xml = indi_alloc(size_right + size_left + 1);

            memcpy(xml, proxy->buff + proxy->stag_idx, size_right);
            memcpy(xml + size_right, proxy->buff, size_left);

            xml[size_right + size_left + 1] = '\0';

            proxy->emit_func(xml);

            indi_free(xml);

            /*--------------------------------------------------------------------------------------------------------*/

            proxy->etag = NULL;

            /*--------------------------------------------------------------------------------------------------------*/
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

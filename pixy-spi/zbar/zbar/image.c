/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#include <string.h>
#include "image.h"

zbar_image_t *zbar_image_create ()
{
    zbar_image_t *img = __calloc(1, sizeof(zbar_image_t));
    _zbar_refcnt_init();
    _zbar_image_refcnt(img, 1);
    img->srcidx = -1;
    return(img);
}

void _zbar_image_free (zbar_image_t *img)
{
    if(img->syms) {
        zbar_symbol_set_ref(img->syms, -1);
        img->syms = NULL;
    }
    __free(img);
}

void zbar_image_destroy (zbar_image_t *img)
{
    _zbar_image_refcnt(img, -1);
}

void zbar_image_ref (zbar_image_t *img,
                     int refs)
{
    _zbar_image_refcnt(img, refs);
}

unsigned long zbar_image_get_format (const zbar_image_t *img)
{
    return(img->format);
}

unsigned zbar_image_get_sequence (const zbar_image_t *img)
{
    return(img->seq);
}

unsigned zbar_image_get_width (const zbar_image_t *img)
{
    return(img->width);
}

unsigned zbar_image_get_height (const zbar_image_t *img)
{
    return(img->height);
}

const void *zbar_image_get_data (const zbar_image_t *img)
{
    return(img->data);
}

unsigned long zbar_image_get_data_length (const zbar_image_t *img)
{
    return(img->datalen);
}

void zbar_image_set_format (zbar_image_t *img,
                            unsigned long fmt)
{
    img->format = fmt;
}

void zbar_image_set_sequence (zbar_image_t *img,
                              unsigned seq)
{
    img->seq = seq;
}

void zbar_image_set_size (zbar_image_t *img,
                          unsigned w,
                          unsigned h)
{
    img->width = w;
    img->height = h;
}

inline void zbar_image_free_data (zbar_image_t *img)
{
    if(!img)
        return;
    else if(img->cleanup && img->data) {
        if(img->cleanup != zbar_image_free_data) {
            /* using function address to detect this case is a bad idea;
             * windows link libraries add an extra layer of indirection...
             * this works around that problem (bug #2796277)
             */
            zbar_image_cleanup_handler_t *cleanup = img->cleanup;
            img->cleanup = zbar_image_free_data;
            cleanup(img);
        }
        else
            __free((void*)img->data);
    }
    img->data = NULL;
}

void zbar_image_set_data (zbar_image_t *img,
                          const void *data,
                          unsigned long len,
                          zbar_image_cleanup_handler_t *cleanup)
{
    zbar_image_free_data(img);
    img->data = data;
    img->datalen = len;
    img->cleanup = cleanup;
}

void zbar_image_set_userdata (zbar_image_t *img,
                              void *userdata)
{
    img->userdata = userdata;
}

void *zbar_image_get_userdata (const zbar_image_t *img)
{
    return(img->userdata);
}

zbar_image_t *zbar_image_copy (const zbar_image_t *src)
{
    zbar_image_t *dst = zbar_image_create();
    dst->format = src->format;
    dst->width = src->width;
    dst->height = src->height;
    dst->datalen = src->datalen;
    dst->data = __malloc(src->datalen);
    assert(dst->data);
    memcpy((void*)dst->data, src->data, src->datalen);
    dst->cleanup = zbar_image_free_data;
    return(dst);
}

const zbar_symbol_set_t *zbar_image_get_symbols (const zbar_image_t *img)
{
    return(img->syms);
}

void zbar_image_set_symbols (zbar_image_t *img,
                             const zbar_symbol_set_t *syms)
{
    if(img->syms)
        zbar_symbol_set_ref(img->syms, -1);
    img->syms = (zbar_symbol_set_t*)syms;
    if(syms)
        zbar_symbol_set_ref(img->syms, 1);
}

const zbar_symbol_t *zbar_image_first_symbol (const zbar_image_t *img)
{
    return((img->syms) ? img->syms->head : NULL);
}

typedef struct zimg_hdr_s {
    uint32_t magic, format;
    uint16_t width, height;
    uint32_t size;
} zimg_hdr_t;

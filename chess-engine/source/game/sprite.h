#ifndef SPRITE_H
#define SPRITE_H

#include "gmath.h"

typedef struct
{

    int width;
    int height;

    int frames;

    uint32_t* pixels;

} spritesheet_t;

bool R_create_spritesheet(uint8_t* bitmap,
                          spritesheet_t* spritesheet)
{

    uint32_t file_size;
    uint32_t data_offset;
    uint32_t compression;

    if ((bitmap[0] != 'B') || (bitmap[1] != 'M'))
        return false;

    file_size = *(uint32_t*) (bitmap + 2);
    data_offset = *(uint32_t*) (bitmap + 10);

    if (file_size < 60)
        return false;

    compression = *(uint32_t*) (bitmap + 30);

    if (compression != 0)
        return false; // ERROR: compression is not supported
        
    spritesheet->width = *(int*) (bitmap + 18);
    spritesheet->height = *(int*) (bitmap + 22);
    
    if (((file_size - data_offset) / (spritesheet->width * spritesheet->height)) != 4)
        return false;

    if ((spritesheet->width % spritesheet->height) != 0)
        return false; // ERROR: invalid sprite

    spritesheet->frames = spritesheet->width / spritesheet->height;
    spritesheet->pixels = (uint32_t*) (bitmap + data_offset);

    return true;
}

void
R_draw_sprite(p_framebuffer_t* framebuffer, // Framebuffer to draw in
              spritesheet_t* spritesheet, // Spritesheet to draw with
              m_rectangle_t* destination, // Region in framebuffer to draw in
              uint32_t color_filter, // Color to apply to the texture
              uint32_t frame) // Which frame of the sprite to draw
{

    if (--frame >= spritesheet->frames)
        return;

    float dw = destination->right - destination->left;
    float dh = destination->top - destination->bottom;

    float xstep = spritesheet->height / dw;
    float ystep = spritesheet->height / dh;

    int xs = M_max(destination->left, 0);
    int ys = M_max(destination->bottom, 0);

    int xf = M_min(destination->right, framebuffer->width);
    int yf = M_min(destination->top, framebuffer->height);

    float xbase = frame * spritesheet->height;
    float xi = xstep * (xs - destination->left) + xbase;
    float yi = ystep * (ys - destination->bottom);

    float xt = xi;
    float yt = yi;
    
    uint32_t* src = spritesheet->pixels;
    uint32_t* dest = framebuffer->pixels;

    for (int y = ys; y < yf; ++y, yt += ystep)
    {
        
        uint32_t* src_row = src + spritesheet->width * ((int) yt);
        uint32_t* dest_row = dest + framebuffer->width * y + xs;
        
        for (int x = xs; x < xf; ++x, xt += xstep)
        {
            
            uint32_t src_color = src_row[(int) xt];             
            uint32_t alpha = src_color >> 24;
                        
            *dest_row++ = M_lerp_color(*dest_row, src_color & color_filter, alpha);
                
        } xt = xi;
    }

}

#endif
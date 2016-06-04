#ifndef RENDERER_H
#define RENDERER_H

#include "platform.h"
#include "gmath.h"

void
R_fill_rectangle(p_framebuffer_t* framebuffer,
                 m_rectangle_t* rectangle,
                 uint32_t color)
{
    
    uint32_t* pixels = framebuffer->pixels;
    
    int xs = M_max(rectangle->left, 0);
    int xf = M_min(rectangle->right, framebuffer->width);
    
    int ys = M_max(rectangle->bottom, 0);
    int yf = M_min(rectangle->top, framebuffer->height);
    
    for (int y = ys; y < yf; ++y)
    {        
        uint32_t* pixels_row = pixels + framebuffer->width * y + xs;        
        for (int x = xs; x < xf; ++x)
            *pixels_row++ = color;
    }
}

void
R_blend_rectangle(p_framebuffer_t* framebuffer,
                  m_rectangle_t* rectangle,
                  uint32_t color)
{
    uint32_t* pixels = framebuffer->pixels;
    
    int xs = M_max(rectangle->left, 0);
    int xf = M_min(rectangle->right, framebuffer->width);
    
    int ys = M_max(rectangle->bottom, 0);
    int yf = M_min(rectangle->top, framebuffer->height);
    
    uint32_t alpha = color >> 24;
    
    for (int y = ys; y < yf; ++y)
    {        
        uint32_t* pixels_row = pixels + framebuffer->width * y + xs;        
        for (int x = xs; x < xf; ++x)
            *pixels_row++ = M_lerp_color(*pixels_row, color, alpha);
    }
}

void
R_draw_highlight(p_framebuffer_t* framebuffer,
                 m_rectangle_t* draw_region,
                 uint32_t fill_color,
                 uint32_t outline_color)
{

    R_blend_rectangle(framebuffer, draw_region, fill_color);

    if (draw_region->left >= 0 && draw_region->left < framebuffer->width)
        for (int y = M_max(draw_region->bottom, 0); y < M_min(draw_region->top, framebuffer->height); ++y)
            framebuffer->pixels[framebuffer->width * y + draw_region->left] = outline_color;

    if (draw_region->right >= 0 && draw_region->right < framebuffer->width)
        for (int y = M_max(draw_region->bottom, 0); y < M_min(draw_region->top, framebuffer->height); ++y)
            framebuffer->pixels[framebuffer->width * y + (draw_region->right - 1)] = outline_color;

    if (draw_region->bottom >= 0 && draw_region->bottom < framebuffer->height)
        for (int x = M_max(draw_region->left, 0); x < M_min(draw_region->right, framebuffer->width); ++x)
            framebuffer->pixels[framebuffer->width * draw_region->bottom + x] = outline_color;

    if (draw_region->top >= 0 && draw_region->top < framebuffer->height)
        for (int x = M_max(draw_region->left, 0); x < M_min(draw_region->right, framebuffer->width); ++x)
            framebuffer->pixels[framebuffer->width * (draw_region->top - 1) + x] = outline_color;

}     

#endif
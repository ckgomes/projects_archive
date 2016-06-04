#ifndef GMATH_H
#define GMATH_H 

typedef struct
{
    
    int top;
    int right;
    int bottom;
    int left;    
    
} m_rectangle_t;

#define M_max(a, b) (a >= b ? a : b)
#define M_min(a, b) (a <  b ? a : b)

#define M_sign(x) ((x > 0) - (x < 0))
#define M_clamp(value, min, max) (value < min ? min : value > max ? max : value)
#define M_between(value, min, max) ((value >= min) && (value <= max))

inline uint32_t
M_lerp_color(uint32_t c1, uint32_t c2, uint32_t lerp)
{
    
    const uint32_t MASK_RED   = 0xFF0000;
    const uint32_t MASK_BLUE  = 0x0000FF;
    const uint32_t MASK_GREEN = 0x00FF00;
    
    uint32_t lerp_red = (((0xFF - lerp) * (c1 & MASK_RED)) + lerp * (c2 & MASK_RED)) >> 8;
    uint32_t lerp_blue = (((0xFF - lerp) * (c1 & MASK_BLUE)) + lerp * (c2 & MASK_BLUE)) >> 8;
    uint32_t lerp_green = (((0xFF - lerp) * (c1 & MASK_GREEN)) + lerp * (c2 & MASK_GREEN)) >> 8;
    
    return (lerp_red & MASK_RED) | (lerp_blue & MASK_BLUE) | (lerp_green & MASK_GREEN);
    
}

#endif
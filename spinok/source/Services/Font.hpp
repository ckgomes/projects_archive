#ifndef FONT_HPP
#define FONT_HPP

#include "../Common/Vector2.hpp"

#include <vector>

class Font final
{

public:

    Font(const char* path);
    ~Font();

    const std::vector<vec2>& positions(char ch) const;
    const std::vector<u16>& indices(char ch) const;
    f32 advancement(char ch) const;

private:

    std::vector<f32> m_advancement;
    std::vector<std::vector<vec2>> m_positions;
    std::vector<std::vector<u16>> m_indices;

};

#endif // FONT_HPP


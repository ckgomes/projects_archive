#include "Font.hpp"

#include <TTF/TTF.hpp>

Font::Font(const char* path)
{

    constexpr size_t numCharacters = 256;

    TTF::Font font(path);
    TTF::Triangulator2DI triangulator;

    // This will make the font 1 unit in height, now it must be scaled
    f32 i_scale = 1.0F / f32(font.UnitsPerEM());    

    m_positions.reserve(numCharacters);
    m_indices.reserve(numCharacters);
    m_advancement.reserve(numCharacters);

    for (u32 ch = 0; ch < numCharacters; ch++)
    {

        auto codePoint = TTF::CodePoint(ch);
        auto glyphMetrics = font.GetGlyphMetrics(codePoint);
        
        m_positions.emplace_back();
        m_indices.emplace_back();

        font.TriangulateGlyph(codePoint, triangulator);

        m_advancement.emplace_back(f32(glyphMetrics.advance_width) * i_scale);

        m_indices.back().reserve(triangulator.GetTriangleCount() * 3);
        m_positions.back().reserve(triangulator.GetVertexCount());

        for (const auto& triangle : triangulator)
        {
            m_indices.back().emplace_back(u16(triangle.i0));
            m_indices.back().emplace_back(u16(triangle.i1));
            m_indices.back().emplace_back(u16(triangle.i2));
        }

        for (size_t i = 0; i < triangulator.GetVertexCount(); ++i)
        {
            auto position = triangulator.GetVertex(i);
            m_positions.back().emplace_back(vec2 {f32(position.x), f32(position.y)} * i_scale);
        }
    }
}

Font::~Font()
{}

const std::vector<vec2>& Font::positions(char ch) const
{
    return m_positions[ch];
}

const std::vector<u16>& Font::indices(char ch) const
{
    return m_indices[ch];
}

f32 Font::advancement(char ch) const
{
    return m_advancement[ch];
}

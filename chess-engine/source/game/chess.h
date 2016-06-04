#ifndef CHESS_H
#define CHESS_H

#include "renderer.h"
#include "sprite.h"

#define BOARD_SIZE 120

#define PC_COLOR_MASK 0x08
#define PC_PIECE_MASK 0x07

typedef enum
{

    PC_WHITE = 0x0,
    PC_BLACK = 0x8

} piece_color_t;

typedef enum
{

    PC_EMPTY   = 0,
    PC_PAWN    = 1,
    PC_ROOK    = 2,
    PC_KNIGHT  = 3,
    PC_BISHOP  = 4,
    PC_QUEEN   = 5,
    PC_KING    = 6,
    PC_INVALID = 7,

    PC_WHITE_PAWN    = PC_WHITE | PC_PAWN,
    PC_WHITE_ROOK    = PC_WHITE | PC_ROOK,
    PC_WHITE_KNIGHT  = PC_WHITE | PC_KNIGHT,
    PC_WHITE_BISHOP  = PC_WHITE | PC_BISHOP,
    PC_WHITE_QUEEN   = PC_WHITE | PC_QUEEN,
    PC_WHITE_KING    = PC_WHITE | PC_KING,

    PC_BLACK_PAWN    = PC_BLACK | PC_PAWN,
    PC_BLACK_ROOK    = PC_BLACK | PC_ROOK,
    PC_BLACK_KNIGHT  = PC_BLACK | PC_KNIGHT,
    PC_BLACK_BISHOP  = PC_BLACK | PC_BISHOP,
    PC_BLACK_QUEEN   = PC_BLACK | PC_QUEEN,
    PC_BLACK_KING    = PC_BLACK | PC_KING,

} board_piece_t;

typedef enum
{

    PC_WHITE_KING_ELIGIBLE  = (1 << 0),
    PC_WHITE_LROOK_ELIGIBLE = (1 << 1),
    PC_WHITE_RROOK_ELIGIBLE = (1 << 2),

    PC_BLACK_KING_ELIGIBLE  = (1 << 3),
    PC_BLACK_LROOK_ELIGIBLE = (1 << 4),
    PC_BLACK_RROOK_ELIGIBLE = (1 << 5),

    PC_ALL_ELIGIBLE = 0xFF

} castling_flags_t;

typedef enum
{

    PC_OK, // Move accepted
    PC_PROMOTION, // Move accepted, prompt for pawn promotion
    PC_CHECK, // Move would result in a direct checkmate
    PC_ILEGAL // Move is not allowed

} move_result_t;

typedef enum
{

    PC_OUTSIDE = 0,

    PC_A8 = 21, PC_B8, PC_C8, PC_D8, PC_E8, PC_F8, PC_G8, PC_H8,
    PC_A7 = 31, PC_B7, PC_C7, PC_D7, PC_E7, PC_F7, PC_G7, PC_H7,
    PC_A6 = 41, PC_B6, PC_C6, PC_D6, PC_E6, PC_F6, PC_G6, PC_H6,
    PC_A5 = 51, PC_B5, PC_C5, PC_D5, PC_E5, PC_F5, PC_G5, PC_H5,
    PC_A4 = 61, PC_B4, PC_C4, PC_D4, PC_E4, PC_F4, PC_G4, PC_H4,
    PC_A3 = 71, PC_B3, PC_C3, PC_D3, PC_E3, PC_F3, PC_G3, PC_H3,
    PC_A2 = 81, PC_B2, PC_C2, PC_D2, PC_E2, PC_F2, PC_G2, PC_H2,
    PC_A1 = 91, PC_B1, PC_C1, PC_D1, PC_E1, PC_F1, PC_G1, PC_H1,

} board_place_t;

typedef struct
{

    board_piece_t pieces[BOARD_SIZE];
    castling_flags_t castling_flags;
    board_place_t enpassant_place;
    bool king_checked;
    uint32_t turn;

} board_state_t;

typedef struct
{
    // One piece should never have more than 40 legal moves in one given turn
    // After the last legal place to move, PC_OUTSIDE is added to indicate so
    board_place_t places[BOARD_SIZE / 3];

} valid_moves_t;

///////////////////////////////////////////////////////////////////////////////

bool
C_is_white(board_piece_t piece)
{
    return M_between(piece, PC_WHITE_PAWN, PC_WHITE_KING);
}

bool
C_is_black(board_piece_t piece)
{
    return M_between(piece, PC_BLACK_PAWN, PC_BLACK_KING);
}

valid_moves_t
C_possible_moves(const board_state_t* board_state,
                 board_place_t placeid)
{

    const board_place_t pawn_offsets[] = { 9, 11 }; // Signed depending on color
    const board_place_t knight_offsets[] = { -21, -19, -12, -8, 8, 12, 19, 21 };
    const board_place_t royal_offsets[] = { -11, -10, -9, -1, 1, 9, 10, 11 };
    const board_place_t bishop_offsets[] = { -11, -9, 9, 11 };
    const board_place_t rook_offsets[] = { -10, -1, 1, 10 };

    const int pawn_offsets_count = sizeof(pawn_offsets) / sizeof(board_place_t);
    const int knight_offsets_count = sizeof(knight_offsets) / sizeof(board_place_t);
    const int royal_offsets_count = sizeof(royal_offsets) / sizeof(board_place_t);
    const int bishop_offsets_count = sizeof(bishop_offsets) / sizeof(board_place_t);
    const int rook_offsets_count = sizeof(rook_offsets) / sizeof(board_place_t);

    valid_moves_t valid_moves;

    const board_piece_t* pieces = board_state->pieces;
    board_place_t* places = valid_moves.places;

    board_piece_t pieceid = board_state->pieces[placeid];
    int move_index = 0;

    switch (pieceid)
    {

        case PC_WHITE_PAWN:
        {
            if (pieces[placeid - 10] == PC_EMPTY)
            {
                places[move_index++] = placeid - 10;
                if (M_between(placeid, PC_A2, PC_H2) && (pieces[placeid - 20] == PC_EMPTY))
                    places[move_index++] = placeid - 20;
            }
            for (int i = 0; i < pawn_offsets_count; ++i)
            {
                if (C_is_black(pieces[placeid - pawn_offsets[i]]) ||
                   ((board_state->enpassant_place == (placeid - pawn_offsets[i])) &&
                    (pieces[placeid - pawn_offsets[i]] == PC_EMPTY)))
                    places[move_index++] = placeid - pawn_offsets[i];
            }
        } break;

        case PC_WHITE_ROOK:
        {
            for (int i = 0; i < rook_offsets_count; ++i)
            {
                int offset;
                for (offset = rook_offsets[i];
                     pieces[placeid + offset] == PC_EMPTY;
                     offset += rook_offsets[i])
                    places[move_index++] = placeid + offset;
                if (C_is_black(pieces[placeid + offset]))
                    places[move_index++] = placeid + offset;
            }
        } break;

        case PC_WHITE_KNIGHT:
        {
            for (int i = 0; i < knight_offsets_count; ++i)
                if (!C_is_white(pieces[placeid + knight_offsets[i]]))
                    places[move_index++] = placeid + knight_offsets[i];
        } break;

        case PC_WHITE_BISHOP:
        {
            for (int i = 0; i < bishop_offsets_count; ++i)
            {
                int offset;
                for (offset = bishop_offsets[i];
                     pieces[placeid + offset] == PC_EMPTY;
                     offset += bishop_offsets[i])
                    places[move_index++] = placeid + offset;
                if (C_is_black(pieces[placeid + offset]))
                    places[move_index++] = placeid + offset;
            }
        } break;

        case PC_WHITE_QUEEN:
        {
            for (int i = 0; i < royal_offsets_count; ++i)
            {
                int offset;
                for (offset = royal_offsets[i];
                     pieces[placeid + offset] == PC_EMPTY;
                     offset += royal_offsets[i])
                    places[move_index++] = placeid + offset;
                if (C_is_black(pieces[placeid + offset]))
                    places[move_index++] = placeid + offset;
            }
        } break;

        case PC_WHITE_KING:
        {
            for (int i = 0; i < royal_offsets_count; ++i)
                if (!C_is_white(pieces[placeid + royal_offsets[i]]))
                    places[move_index++] = placeid + royal_offsets[i];
        } break;

        case PC_BLACK_PAWN:
        {
            if (pieces[placeid + 10] == PC_EMPTY)
            {
                places[move_index++] = placeid + 10;
                if (M_between(placeid, PC_A7, PC_H7) && (pieces[placeid + 20] == PC_EMPTY))
                    places[move_index++] = placeid + 20;
            }
            for (int i = 0; i < pawn_offsets_count; ++i)
            {
                if (C_is_white(pieces[placeid + pawn_offsets[i]]) ||
                   ((board_state->enpassant_place == (placeid + pawn_offsets[i])) &&
                    (pieces[placeid + pawn_offsets[i]] == PC_EMPTY)))
                    places[move_index++] = placeid + pawn_offsets[i];
            }
        } break;

        case PC_BLACK_ROOK:
        {
            for (int i = 0; i < rook_offsets_count; ++i)
            {
                int offset;
                for (offset = rook_offsets[i];
                     pieces[placeid + offset] == PC_EMPTY;
                     offset += rook_offsets[i])
                    places[move_index++] = placeid + offset;
                if (C_is_white(pieces[placeid + offset]))
                    places[move_index++] = placeid + offset;
            }
        } break;

        case PC_BLACK_KNIGHT:
        {
            for (int i = 0; i < knight_offsets_count; ++i)
                if (!C_is_black(pieces[placeid + knight_offsets[i]]))
                    places[move_index++] = placeid + knight_offsets[i];
        } break;

        case PC_BLACK_BISHOP:
        {
            for (int i = 0; i < bishop_offsets_count; ++i)
            {
                int offset;
                for (offset = bishop_offsets[i];
                     pieces[placeid + offset] == PC_EMPTY;
                     offset += bishop_offsets[i])
                    places[move_index++] = placeid + offset;
                if (C_is_white(pieces[placeid + offset]))
                    places[move_index++] = placeid + offset;
            }
        } break;

        case PC_BLACK_QUEEN:
        {
            for (int i = 0; i < royal_offsets_count; ++i)
            {
                int offset;
                for (offset = royal_offsets[i];
                     pieces[placeid + offset] == PC_EMPTY;
                     offset += royal_offsets[i])
                    places[move_index++] = placeid + offset;
                if (C_is_white(pieces[placeid + offset]))
                    places[move_index++] = placeid + offset;
            }
        } break;

        case PC_BLACK_KING:
        {
            for (int i = 0; i < royal_offsets_count; ++i)
                if (!C_is_black(pieces[placeid + royal_offsets[i]]))
                    places[move_index++] = placeid + royal_offsets[i];
        } break;

    }

    valid_moves.places[move_index] = PC_OUTSIDE;
    return valid_moves;

}

void
C_initialize_board(board_state_t* board_state)
{

    for (int placeid = 0; placeid < BOARD_SIZE; ++placeid)
    {

        int rem = placeid % 10;
        int div = placeid / 10;

        if ((div >= 2) && (div <= 9) && (rem >= 1) && (rem <= 8))
            board_state->pieces[placeid] = PC_EMPTY;
        else board_state->pieces[placeid] = PC_INVALID;

    }

    for (int i = PC_A7; i <= PC_H7; ++i)
        board_state->pieces[i] = PC_BLACK_PAWN;
    for (int i = PC_A2; i <= PC_H2; ++i)
        board_state->pieces[i] = PC_WHITE_PAWN;

    board_state->pieces[PC_A8] = board_state->pieces[PC_H8] = PC_BLACK_ROOK;
    board_state->pieces[PC_A1] = board_state->pieces[PC_H1] = PC_WHITE_ROOK;

    board_state->pieces[PC_B8] = board_state->pieces[PC_G8] = PC_BLACK_KNIGHT;
    board_state->pieces[PC_B1] = board_state->pieces[PC_G1] = PC_WHITE_KNIGHT;

    board_state->pieces[PC_C8] = board_state->pieces[PC_F8] = PC_BLACK_BISHOP;
    board_state->pieces[PC_C1] = board_state->pieces[PC_F1] = PC_WHITE_BISHOP;

    board_state->pieces[PC_D8] = PC_BLACK_QUEEN;
    board_state->pieces[PC_D1] = PC_WHITE_QUEEN;

    board_state->pieces[PC_E8] = PC_BLACK_KING;
    board_state->pieces[PC_E1] = PC_WHITE_KING;

    board_state->castling_flags = PC_ALL_ELIGIBLE;
    board_state->enpassant_place = PC_OUTSIDE;
    board_state->king_checked = false;
    board_state->turn = 0;

}

move_result_t
C_process_move(board_state_t* board_state,
               board_place_t src_place,
               board_place_t dest_place)
{

    board_piece_t* src_piece = board_state->pieces + src_place;
    board_piece_t* dest_piece = board_state->pieces + dest_place;

    // If the king is checked, does this move resolve it
    if (board_state->king_checked)
    {
        board_state_t board_copy = *board_state;
        for (int i = PC_A8; i <= PC_H1; ++i)
        {
            // valid_moves_t valid_moves = C_possible_moves(&board_copy,
            //                                               )
        }
    }

    // Process En passant capture
    if (dest_place == board_state->enpassant_place)
    {
        if (board_state->turn % 2)
            board_state->pieces[dest_place - 10] = PC_EMPTY;
        else board_state->pieces[dest_place + 10] = PC_EMPTY;
    }
    board_state->enpassant_place = PC_OUTSIDE;

    // Process En passant opportunity
    if ((*src_piece == PC_WHITE_PAWN) && ((src_place - dest_place) == 20))
        board_state->enpassant_place = src_place - 10;
    if ((*src_piece == PC_BLACK_PAWN) && ((dest_place - src_place) == 20))
        board_state->enpassant_place = src_place + 10;

    // Move the pieces
    *dest_piece = *src_piece;
    *src_piece = PC_EMPTY;

    board_state->turn++;

    return PC_OK;

}

board_place_t
C_place_of_point(m_rectangle_t* board_region,
                 int point_x,
                 int point_y)
{

    if (point_x < board_region->left || point_x >= board_region->right)
        return PC_OUTSIDE;

    if (point_y < board_region->bottom || point_y >= board_region->top)
        return PC_OUTSIDE;

    int dx = board_region->right - board_region->left;
    int dy = board_region->top - board_region->bottom;

    int sx = dx / 8;
    int sy = dy / 8;

    int px = point_x - board_region->left;
    int py = point_y - board_region->bottom;

    int unit = 1 + px / sx;
    int tens = 9 - py / sy;

    return (board_place_t) (unit + tens * 10);

}

m_rectangle_t
C_rectangle_of_place(m_rectangle_t* board_region,
                     board_place_t placeid)
{

    m_rectangle_t place = {0};

    int dx = board_region->right - board_region->left;
    int dy = board_region->top - board_region->bottom;

    int sx = dx / 8;
    int sy = dy / 8;

    int rem = placeid % 10;
    int div = placeid / 10;

    if ((div >= 2) && (div <= 9) && (rem >= 1) && (rem <= 8))
    {
        place.left = board_region->left + (rem - 1) * sx;
        place.bottom = board_region->bottom + (9 - div) * sy;
        place.right = place.left + sx;
        place.top = place.bottom + sy;
    }

    return place;

}

void
C_draw_board(p_framebuffer_t* framebuffer,
             m_rectangle_t* region)
{

    const uint32_t colors[] = { 0xFFFFFBC9, 0xFF121212 };

    int pw = (region->right - region->left) / 8;
    int ph = (region->top - region->bottom) / 8;

    m_rectangle_t place_rectangle;
    place_rectangle.left   = region->left;
    place_rectangle.right  = region->left + pw;
    place_rectangle.top    = region->top;
    place_rectangle.bottom = region->top - ph;

    int colorid = 0;

    for (int place = PC_A8; place <= PC_H1; ++place)
    {

        if ((place % 10) == 9)
        {
            place_rectangle.left   = region->left;
            place_rectangle.right  = region->left + pw;
            place_rectangle.top    -= ph;
            place_rectangle.bottom -= ph;
            place++;
        }
        else
        {
            R_fill_rectangle(framebuffer, &place_rectangle, colors[colorid]);
            place_rectangle.left  += pw;
            place_rectangle.right += pw;
        }

        colorid = !colorid;

    }

}

void
C_draw_piece(p_framebuffer_t* framebuffer,
             spritesheet_t* spritesheet,
             m_rectangle_t* region,
             board_piece_t piece)
{

    const uint32_t colors[] = { 0xFFB0B0B0, 0xFF606060 };

    R_draw_sprite(framebuffer,
                  spritesheet,
                  region,
                  colors[(piece & PC_BLACK) != 0],
                  piece & PC_PIECE_MASK);

}

void
C_draw_board_state(p_framebuffer_t* framebuffer,
                   spritesheet_t* spritesheet,
                   m_rectangle_t* board_region,
                   board_state_t* board_state)
{
    for (int placeid = 0; placeid < BOARD_SIZE; ++placeid)
    {
        board_piece_t pieceid = board_state->pieces[placeid];
        if (pieceid != PC_EMPTY)
        {
            m_rectangle_t piece_region = C_rectangle_of_place(board_region, placeid);
            C_draw_piece(framebuffer, spritesheet, &piece_region, pieceid);
        }
    }
}

#endif
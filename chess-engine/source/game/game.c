#include "platform.h"
#include "chess.h"

typedef struct
{

    // NOTE: keep this field on top for live editing stability
    uint32_t struct_size;

    bool initialized;
    bool clear;
    int frame;

    double time_acc;

    board_place_t mouse_place;
    board_piece_t selected_piece;
    m_rectangle_t selected_piece_region;

    board_piece_t grabbed_place;
    valid_moves_t grabbed_moves;

    uint8_t* pieces_bitmap;
    spritesheet_t pieces_spritesheet;

    m_rectangle_t turn_color_region;
    m_rectangle_t board_region;
    board_state_t board_state;

} gamestate_t;

void
G_initialize(p_game_t* game)
{

    gamestate_t* gamestate = (gamestate_t*) game->memory;

    if (!gamestate->initialized || (gamestate->struct_size != sizeof(gamestate_t)))
    {

        gamestate->struct_size = sizeof(gamestate_t);

        gamestate->initialized = true;
        gamestate->frame = 0;

        gamestate->selected_piece = 0;
        gamestate->grabbed_place = 0;

        C_initialize_board(&gamestate->board_state);
    }

    if (game->memory_size < sizeof(gamestate_t))
        printf("Not enough memory to run the game\n");

    if (!game->load_asset("asset/pieces.bmp", &gamestate->pieces_bitmap, NULL))
        printf("Failed to load asset\n");

    if (!R_create_spritesheet(gamestate->pieces_bitmap, &gamestate->pieces_spritesheet))
        printf("Failed to create spritesheet\n");

    gamestate->board_region.left = gamestate->board_region.bottom = 0;
    gamestate->board_region.right = gamestate->board_region.top = 600;
    
    gamestate->turn_color_region.left = 700;
    gamestate->turn_color_region.right = 764;
    gamestate->turn_color_region.bottom = 140;
    gamestate->turn_color_region.top = 204;

    gamestate->selected_piece_region.left = 700;
    gamestate->selected_piece_region.right = 764;
    gamestate->selected_piece_region.bottom = 40;
    gamestate->selected_piece_region.top = 104;

    gamestate->clear = true;

}

void
G_update(float dt,
         p_timer_t* timer,
         p_input_t* input,
         p_game_t* game)
{

    gamestate_t* gamestate = (gamestate_t*) game->memory;

    gamestate->time_acc += dt;

    gamestate->mouse_place = C_place_of_point(&gamestate->board_region,
                                               input->mouse_x,
                                               input->mouse_y);

    // Replace pieces handler
    if (P_mouse_isdown(input, P_MOUSE_RIGHT) && (gamestate->mouse_place != PC_OUTSIDE))
        gamestate->board_state.pieces[gamestate->mouse_place] = gamestate->selected_piece;

    // Move pieces handler
    if (P_mouse_isdown(input, P_MOUSE_LEFT) &&
       (gamestate->mouse_place != PC_OUTSIDE))
    {
        board_piece_t pieceid = gamestate->board_state.pieces[gamestate->mouse_place];
        if (gamestate->grabbed_place == gamestate->mouse_place)
            gamestate->grabbed_place = PC_OUTSIDE;
        else if ((gamestate->grabbed_place != PC_OUTSIDE))
        {
            valid_moves_t* moves = &gamestate->grabbed_moves;
            for (int i = 0; moves->places[i] != PC_OUTSIDE; ++i)
            {
                if (moves->places[i] == gamestate->mouse_place)
                {
                    C_process_move(&gamestate->board_state,
                                    gamestate->grabbed_place,
                                    gamestate->mouse_place);
                    gamestate->grabbed_place = PC_OUTSIDE;
                    printf("En passant = %u\n", gamestate->board_state.enpassant_place);
                    fflush(stdout);
                    break;
                }
            }
        }
        else if (pieceid != PC_EMPTY)
        {
            bool black_turn = gamestate->board_state.turn % 2;
            if ((black_turn && C_is_black(pieceid)) ||
               (!black_turn && C_is_white(pieceid)))
            {
                gamestate->grabbed_place = gamestate->mouse_place;
                gamestate->grabbed_moves = C_possible_moves(&gamestate->board_state,
                                                             gamestate->grabbed_place);                
            }
        }
    }

    // Piece selection handler
    gamestate->selected_piece += M_sign(input->mouse_wheel);
    gamestate->selected_piece = M_clamp(gamestate->selected_piece, PC_EMPTY, PC_BLACK_KING);
    if (gamestate->selected_piece == PC_INVALID)
        gamestate->selected_piece = PC_BLACK_PAWN;
    if (gamestate->selected_piece == PC_BLACK)
        gamestate->selected_piece = PC_WHITE_KING;

}

void
G_render(p_framebuffer_t* backbuffer,
         p_input_t* input,
         p_game_t* game)
{

    gamestate_t* gamestate = (gamestate_t*) game->memory;
    gamestate->frame++;

    if (gamestate->clear)
    {
        memset(backbuffer->pixels, 0x00, backbuffer->width * backbuffer->height * 4);
        gamestate->clear = false;
    }

    C_draw_board(backbuffer, &gamestate->board_region);

    if (gamestate->mouse_place != PC_OUTSIDE)
    {
        m_rectangle_t highlight_region = C_rectangle_of_place(&gamestate->board_region,
                                                               gamestate->mouse_place);
        R_blend_rectangle(backbuffer, &highlight_region, 0x7F00FFFF);
    }

    if (gamestate->grabbed_place != PC_OUTSIDE)
    {
        valid_moves_t* moves = &gamestate->grabbed_moves;
        for (int i = 0; i < moves->places[i] != PC_OUTSIDE; ++i)
        {
            board_piece_t piece = gamestate->board_state.pieces[moves->places[i]];
            m_rectangle_t piece_region = C_rectangle_of_place(&gamestate->board_region,
                                                               moves->places[i]);
            bool iscapture = (piece == PC_EMPTY) &&
                             (moves->places[i] != gamestate->board_state.enpassant_place);
            R_blend_rectangle(backbuffer,
                             &piece_region,
                              iscapture ? 0x5F7F7FFF : 0x4FFF0000);
        }
        m_rectangle_t grab_region = C_rectangle_of_place(&gamestate->board_region,
                                                          gamestate->grabbed_place);
        R_fill_rectangle(backbuffer, &grab_region, 0xFFF0F07F);
    }

    C_draw_board_state(backbuffer,
                      &gamestate->pieces_spritesheet,
                      &gamestate->board_region,
                      &gamestate->board_state);
                      
    R_fill_rectangle(backbuffer,
                    &gamestate->turn_color_region,
                     gamestate->board_state.turn % 2 ? 0xFF606060 : 0xFFB0B0B0);                  

    R_fill_rectangle(backbuffer,
                    &gamestate->selected_piece_region,
                     0xFF000000);

    R_draw_sprite(backbuffer,
                 &gamestate->pieces_spritesheet,
                 &gamestate->selected_piece_region,
                  gamestate->selected_piece & PC_BLACK ? 0xFF606060 : 0xFFB0B0B0,
                  gamestate->selected_piece & PC_PIECE_MASK);

}

void
G_quit(p_game_t* game)
{
    gamestate_t* gamestate = (gamestate_t*) game->memory;
    if (gamestate->pieces_bitmap)
    {
        game->free_asset(gamestate->pieces_bitmap);
        gamestate->pieces_bitmap = 0;
    }
}

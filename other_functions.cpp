
#include "other_functions.hpp"

using namespace game::game_bricks;

const unsigned int* get_stage_map(unsigned int stage)
{
    if(stage < 0 
            || stage >= game::game_bricks::TOTAL_STAGES) return NULL;
    
    return &(game::game_bricks::brick_map[stage][0]);
}
/*
std::vector<sf::IntRect> get_brick_rects(game::game_bricks::bricks id)
{
    unsigned int w = BRICK_BOUND.x;
    unsigned int h = BRICK_BOUND.y;
    
    typedef std::vector<sf::IntRect> brick_list;
    typedef std::map<game::game_bricks::bricks, brick_list > brick_rects;
    
    brick_rects brick_rects_per_type = brick_rects();
    
    //Bloque tipo RED
    brick_list red = brick_list();
    red.push_back(sf::IntRect(64, 0, w, h));
    
    brick_rects_per_type.insert
    (
        std::make_pair(bricks::RED, std::move(red))
    );
    
    
    return brick_rects_per_type.find(id);
}*/
/* 
 * File:   CBonus.hpp
 * Author: sheldor
 *
 * Created on 10 de abril de 2014, 17:06
 */

#ifndef CBONUS_HPP
#define	CBONUS_HPP


#include "CEntity.hpp"
#include "CBrick.hpp"
#include "CPaddle.hpp"

class CBonus : public CEntity
{
    public:

        CBonus(CState *state, game::game_bonus::bonus t);
        CBonus(const CBonus& orig);
        virtual ~CBonus();

        void init();
        void update(const float dt);
        void draw(sf::RenderTarget &target, sf::RenderStates states) const;
        void reset();
        
        void set_position(sf::Vector2f pos);
        
        bool collision_bonus_paddle(CPaddle *p);
        bool is_removable();
        
    private:
        
        void settings();

        game::game_bonus::bonus type;
        
        std::vector<sf::IntRect> rects;
        
        bool removable{false};
};

#endif	/* CBONUS_HPP */


/* 
 * File:   CPlayState.cpp
 * Author: sheldor
 * 
 * Created on 17 de marzo de 2014, 21:11
 */

#include "CPlayState.hpp"

using namespace game::game_states;
using namespace game::game_textures;
using namespace game::game_bricks;

/* Declaracion de variables static */
unsigned int CPlayState::current_stage;

CPlayState::CPlayState(CGameContainer *gc)
{
    this->gc = gc;
    this->rh = new CResourceHolder();
}

CPlayState::CPlayState(const CPlayState& orig)
{
}

CPlayState::~CPlayState()
{
    delete this->balls;
    delete this->balls_iterator;
    delete this->bricks;
    delete this->bricks_iterator;
    delete this->bonus;
    delete this->bonus_iterator;
    delete this->paddle;
    delete this->rh;
    delete this->gc;
}

void CPlayState::init()
{
    this->load_textures();
    
    current_stage = 0;
    //Cargamos el paddle
    this->paddle = new CPaddle(this);
    //Cargamos los  ladrillos
    this->bricks = new brick_list();
    this->load_bricks();
    //Cargamos la bola
    this->balls = new ball_list();
    CBall *b = new CBall(this, this->paddle);
    b->set_in_paddle();
    this->balls->push_back(b);
    //Cargamos los bonus
    this->bonus = new bonus_list();
    /*
    for (int i = 0; i < static_cast<int>(game::game_bonus::bonus::COUNT); ++i)
    {
        CBonus *bb = new CBonus(this, static_cast<game::game_bonus::bonus>(i));
        bb->setPosition(sf::Vector2f(100 + 50 * i, 100));
        this->bonus->push_back(bb);
    }
     * */   
}
float tt=2;
float ctt= 0;
bool btt=false;
void CPlayState::events()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))// && btt)
    {
        ball_iterator I = this->balls->begin();
        ball_iterator E = this->balls->end();

        while(I != E)
        {
            CBall *ball = (*I);        
            
            if (ball->is_in_paddle())
            {
                ball->set_in_paddle(false);
                break;
            }
            
            ++I;
        }
        btt=false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))// && btt)
    {
        CBall *b = new CBall(this, this->paddle);
        b->set_in_paddle();
        this->balls->push_back(b);
        btt=false;
    }
}

int CPlayState::update(float dt)
{
    ctt += dt;
    if (ctt>tt)
    {
        btt = true;
        ctt = 0;
    }
    this->update_paddle(dt);
    this->update_balls(dt);
    this->update_bonus(dt);
    
    if (this->update_bricks(dt))
    {
        this->next_stage();
    }
    
    return states::NULLSTATE;
}

void CPlayState::render()
{
    this->render_paddle();
    this->render_bricks();
    this->render_balls();
    this->render_bonus();
}

void CPlayState::clear()
{
    this->clear_balls();
    this->clear_bonus();
    this->clear_bricks();
}

void CPlayState::update_balls(const float dt)
{
    ball_iterator I = this->balls->begin();
    ball_iterator E = this->balls->end();
    
    while(I != E)
    {
        CBall *ball = (*I);
        
        if (ball->is_removable())
        {
            delete ball;
            I = this->balls->erase(I);
        }
        else
        {
            ball->update(dt);
            ++I;
        }
    }    
}

void CPlayState::update_bonus(const float dt)
{
    bonuss_iterator I = this->bonus->begin();
    bonuss_iterator E = this->bonus->end();
    
    while(I != E)
    {
        CBonus *b = (*I);
        
        if (b->is_removable())
        {
            /* Cambiar el estado del paddle */
            delete b;
            I = this->bonus->erase(I);
        }
        else
        {
            b->update(dt);
            b->collision_bonus_paddle(this->paddle);
            ++I;
        }
    }
}


bool CPlayState::update_bricks(const float dt)
{
    brick_iterator I = this->bricks->begin();
    brick_iterator E = this->bricks->end();
    
    while(I != E)
    {
        CBrick *brick = (*I);
        brick->update(dt);
        
        ball_iterator II = this->balls->begin();
        ball_iterator EE = this->balls->end();

        while(II != EE)
        {
            CBall *ball = (*II);
            
            //if (!ball->is_collisionable())
            //    continue;
            
            if (ball->collision_ball_brick(brick, dt))
            {
                brick->quit_life();
                brick->play_animation();
                break;
            }
            
            ++II;
        }
        
        if (brick->is_removable())
        {
            /* tenemos que insertar un bonus */
            this->insert_bonus(brick);
            delete brick;
            I = this->bricks->erase(I);
            
            this->total_bricks--;
            
            if (this->total_bricks <= 0)
                return true;
        }
        else
        {
            ++I;
        }        
    }
    
    return false;
}

void CPlayState::update_paddle(const float dt)
{
    this->paddle->update(dt);
}



void CPlayState::render_balls()
{
    ball_iterator I = this->balls->begin();
    ball_iterator E = this->balls->end();
    
    while(I != E)
    {
        CBall *ball = (*I);
        this->gc->window->draw(*ball);
        ++I;
    }
}

void CPlayState::render_bricks()
{
    brick_iterator I = this->bricks->begin();
    brick_iterator E = this->bricks->end();
    
    while(I != E)
    {
        CBrick *brick = (*I);
        this->gc->window->draw(*brick);
        ++I;
    }
}

void CPlayState::render_bonus()
{
    bonuss_iterator I = this->bonus->begin();
    bonuss_iterator E = this->bonus->end();
    
    while(I != E)
    {
        CBonus *b = (*I);
        this->gc->window->draw(*b);
        ++I;
    }
}

void CPlayState::render_paddle()
{
    this->gc->window->draw(*(this->paddle));
}


void CPlayState::clear_balls()
{
    ball_iterator I = this->balls->begin();
    ball_iterator E = this->balls->end();
    
    while(I != E)
    {
        CBall *ball = (*I);
        delete ball;
        ++I;
    }
    
    this->balls->clear();
}

void CPlayState::clear_bricks()
{
    brick_iterator I = this->bricks->begin();
    brick_iterator E = this->bricks->end();
    
    while(I != E)
    {
        CBrick *brick = (*I);
        delete brick;
        ++I;
    }
    
    this->bricks->clear();
}

void CPlayState::clear_bonus()
{
    bonuss_iterator I = this->bonus->begin();
    bonuss_iterator E = this->bonus->end();
    
    while(I != E)
    {
        CBonus *b = (*I);
        delete b;
        ++I;
    }
    
    this->bonus->clear();
}



void CPlayState::load_textures()
{
    this->rh->load(textures::MAIN, "media/textures/base.png");
    this->rh->load(game::game_fx::fx::REBOTE1, "media/fx/rebote1.wav");
}

void CPlayState::load_bricks()
{
    //Limpiamos el muro de bloques
    if (this->bricks->size() > 0)
        this->clear_bricks();
    
    if (brick_map[current_stage] == NULL)
        return;
    
    this->total_bricks = 0;
    const unsigned int *current_map{ brick_map[current_stage] };
    
    for(unsigned int i = 0; i < (BRICKS_PER_LINE * LINES); ++i )
    {
        game::game_bricks::bricks type = static_cast<game::game_bricks::bricks>(current_map[i]);
        
        if (type != game::game_bricks::bricks::NONE)
        {
            CBrick *b = new CBrick(this, type);
            
            unsigned int row = i / LINES;
            unsigned int column = i % BRICKS_PER_LINE;
            
            unsigned int x = MARGIN_LEFT + BRICK_BOUND.x * 2 * row;
            unsigned int y = MARGIN_TOP + BRICK_BOUND.y * 2 * column;
            
            b->setPosition(x, y);
            
            this->bricks->push_back(b);
            
            //Contamos los que son rompibles
            if (type != game::game_bricks::bricks::UNDESTROYABLE)
            {
                this->total_bricks++;
            }
        }        
    }
}//End load_bricks()

void CPlayState::next_stage()
{
    current_stage++;
    
    this->clear_balls();
    this->clear_bonus();
    
    this->load_bricks();
    
    this->paddle->reset();
    //Creamos la nueva bola
    CBall *b = new CBall(this, this->paddle);
    b->set_in_paddle();
    this->balls->push_back(b);
}

void CPlayState::insert_bonus(CBrick* brick)
{
    // 10% de posibilidades de que haya bonus
    unsigned int percent = (std::rand() % 1000) + 1;
    
    if (percent > 50) return;
    
    // Ahora, de los que salgan tambien calcularemos las posibilidades
    percent = (std::rand() % 100) + 1;
        
    game::game_bonus::bonus type;
    
    if (percent <= 5)
    {//Vida extra
        type = game::game_bonus::bonus::X;
    }
    else if (percent <= 12)
    {//Laser
        type = game::game_bonus::bonus::L;
    }
    else if (percent <= 25)
    {//Expandir
        type = game::game_bonus::bonus::E;
    }
    else if (percent <= 40)
    {//Encoger
        type = game::game_bonus::bonus::R;
    }
    else if (percent <= 55)
    {//Pegajoso
        type = game::game_bonus::bonus::C;
    }
    else if (percent <= 70)
    {//Multibola
        type = game::game_bonus::bonus::D;
    }
    else if (percent <= 85)
    {//Reducir velocidad bolas
        type = game::game_bonus::bonus::S;
    }    
    else
    {//Speed up
        type = game::game_bonus::bonus::P;
    }
    
    CBonus *b = new CBonus(this, type);
    b->setPosition(sf::Vector2f(brick->getPosition().x, brick->getPosition().y));
    
    this->bonus->push_back(b);   
}

unsigned int CPlayState::get_current_state()
{
    current_stage;
}

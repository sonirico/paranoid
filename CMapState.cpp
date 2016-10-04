/* 
 * File:   CMapState.cpp
 * Author: sheldor
 * 
 * Created on 17 de marzo de 2014, 21:12
 */

#include "CMapState.hpp"

using namespace game::game_states;
using namespace game::game_bricks;

struct SSelector : public CEntity
{
    const float dt_animation = 0.2f;
    //Tipos de animaciones
    const unsigned int USUAL = 0;
    const unsigned int SELECTED = 1;
        
    Animation *v_animation [2];
    
    sf::Vector2f ij;//map[i][j]
    
    bool moved;
    float t_moved;
    const float t_moved_value = .2f;
    
    bool selected;
    
    bool printed;
    
    int brick_type_map[LINES][BRICKS_PER_LINE];
    int current_brick_type;
    
    CMapState *state;
    
    sf::Sprite *wall;
    
    SSelector(CMapState *st)
    {
        this->state = st;
        this->moved = this->selected = this->printed = false;
        
        this->ij = sf::Vector2f(0, 0);
                
        this->v_animation[USUAL] = new Animation();
        this->v_animation[SELECTED] = new Animation();        
         
        this->v_animation[USUAL]->setSpriteSheet
        (
            st->rh->get(game::game_textures::textures::MAIN)
        );
        this->v_animation[SELECTED]->setSpriteSheet
        (
            st->rh->get(game::game_textures::textures::MAIN)
        );
                
        this->v_animation[USUAL]->addFrame(sf::IntRect(128, 48, 16, 8));
        
        this->v_animation[SELECTED]->addFrame(sf::IntRect(128, 56, 16, 8));
        this->v_animation[SELECTED]->addFrame(sf::IntRect(128, 64, 16, 8));
        
        this->current_animation = this->v_animation[USUAL];
        
        this->animated_sprite = AnimatedSprite(sf::seconds(dt_animation), true, false);
        this->animated_sprite.play(*current_animation);
        
        this->setPosition(MARGIN_LEFT, MARGIN_TOP);
        this->scale(this->scalation);
        
        for (unsigned int i = 0; i < LINES; i++)
            for(unsigned int j = 0; j < BRICKS_PER_LINE; j++)
                this->brick_type_map[i][j] = 0;
        
        this->current_brick_type = 0;//NONE
        
        this->wall = new sf::Sprite();
        this->wall->setTexture(this->state->rh->get(game::game_textures::textures::BRICKS));
        this->wall->scale(this->scalation.x, this->scalation.y);
    }
    
    virtual ~SSelector()
    {
        delete this->current_animation;
        
        for (unsigned int i = 0; i < 2; i++)
            delete this->v_animation[i];
        
        delete this->v_animation;
        
        delete this->wall;
    }
    
    void init(){}
    
    void update(float dt)
    {
        this->animated_sprite.update(sf::seconds(dt));
        this->animated_sprite.play(*current_animation);
        
        this->t_moved += dt;
        
        if (this->t_moved >= this->t_moved_value)
        {
            this->t_moved = 0;
            this->moved = false;
        }
    }
    
    void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= this->getTransform();
        /* Drawing the lines */
        for (unsigned int i = 0; i <= LINES; ++i)
        {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(MARGIN_LEFT, MARGIN_TOP + BRICK_BOUND.y * 2 * i)),
                sf::Vertex(sf::Vector2f(game::WIDTH - MARGIN_LEFT, MARGIN_TOP + BRICK_BOUND.y * 2 * i))
            };

            target.draw(line, 2, sf::Lines);
        }
        
        for (unsigned int i = 0; i <= BRICKS_PER_LINE; ++i)
        {
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(MARGIN_LEFT + BRICK_BOUND.x * 2 * i, MARGIN_TOP )),
                sf::Vertex(sf::Vector2f(MARGIN_LEFT + BRICK_BOUND.x * 2 * i, MARGIN_TOP + BRICK_BOUND.y * 2 * LINES))
            };

            target.draw(line, 2, sf::Lines);
        }
        /**/
        /*Drawing the bricks*/
        
        for (unsigned int i = 0; i < LINES; i++)
        {
            for(unsigned int j = 0; j < BRICKS_PER_LINE; j++)
            {
                int t = this->brick_type_map[i][j];
                
                if (t != 0)
                {                    
                    this->wall->setTextureRect(sf::IntRect(BRICK_BOUND.x * (t - 1), 0, BRICK_BOUND.x, BRICK_BOUND.y));
                    this->wall->setPosition(MARGIN_LEFT + BRICK_BOUND.x * 2 * i, MARGIN_TOP + BRICK_BOUND.y * 2 * j);
                    
                    target.draw(*(this->wall));
                }
            }
        }
        /*Drawing the selector*/
        target.draw(this->animated_sprite, states);
    }
    
    void reset()
    {
    }
    
    void set_brick(int dy)
    {
        unsigned int limit = static_cast<int>(bricks::COUNT) - 1;//NONE also
        
        if (this->current_brick_type + dy < 0)
            this->current_brick_type = limit;
        else if (this->current_brick_type + dy > limit)
            this->current_brick_type = 0;
        else
            this->current_brick_type += dy;
        
        //std::cout << "x: " << ij.x << " y: " << ij.y << " current type: " << this->current_brick_type<< std::endl;
        
        unsigned int i = static_cast<int>(this->ij.x);
        unsigned int j = static_cast<int>(this->ij.y);
        
        this->brick_type_map[i][j] = this->current_brick_type;
    }
    
    void print_map()
    {
        if (this->printed) return;
        
        for (unsigned int i = 0; i < LINES; i++)
            for(unsigned int j = 0; j < BRICKS_PER_LINE; j++)
                std::cout << this->brick_type_map[i][j] << ",";
        
        this->printed = true;
    }
    
    void move(int dx, int dy)
    {
        if (this->moved) return;
        if (this->selected)
        {
            this->set_brick(dy);
            this->moved = true;
            this->t_moved = 0;
            return;
        }
        if (ij.x + dx < 0)
            ij.x = BRICKS_PER_LINE - 1;
        else if (ij.x + dx == BRICKS_PER_LINE)
            ij.x = 0;
        else
            ij.x += dx;
        
        if (ij.y + dy < 0)
            ij.y = LINES - 1;
        else if (ij.y + dy == LINES)
            ij.y = 0;
        else
            ij.y += dy;
        
        this->setPosition(MARGIN_LEFT + (ij.x * BRICK_BOUND.x * 2), MARGIN_TOP + (ij.y * BRICK_BOUND.y * 2) );
        this->moved = true;
        this->t_moved = 0;
    }
    
    
    void change_animation()
    {
        if (this->selected)
            this->current_animation = this->v_animation[SELECTED];
        else
            this->current_animation = this->v_animation[USUAL];
    }
    
    void set_selected(bool b)
    {
        this->selected = b;
        
        this->change_animation();
    }
    
};//End struct selector

CMapState::CMapState( CGameContainer *gc )
{
    this->gc = gc;
    this->rh = new CResourceHolder();
}

CMapState::CMapState(const CMapState& orig)
{
}

CMapState::~CMapState()
{   
    delete this->map;
    delete this->rh;
    delete this->paddle;//Hace lo mismo que this->paddle->~CPaddle();
    delete this->balls;
    delete this->balls_iterator;
    delete this->selector;
    delete this->gc;
    delete this->bricks;
    delete this->bricks_iterator;
}

void CMapState::init()
{
    this->rh = new CResourceHolder();
    this->load_textures();
 
    this->last_id = 0;
    
    this->selector = new SSelector(this);
    
    this->paddle = new CPaddle(this);
    
    this->balls = new ball_list();
    //this->balls->push_back(new CBall(this, this->paddle));
    
    this->bricks = new brick_list();
    
    for(int i = 1; i <= 11; ++i )
    { 
        CBrick *b = new CBrick(this, static_cast<game::game_bricks::bricks>(i));
        b->setPosition(100, 100 + 20 * i);
        this->bricks->push_back(b);
    }
   
}

void CMapState::events()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        this->selector->print_map();
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        this->selector->move(-1, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        this->selector->move(1, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        this->selector->move(0, -1);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        this->selector->move(0, 1);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
    {
        this->selector->set_selected(true);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
    {
        this->selector->set_selected(false);
    }
}

int CMapState::update(float dt)
{
    this->update_paddle(dt);
    this->update_balls(dt);    
    this->update_bricks(dt);
    
    this->selector->update(dt);
    
    
    return states::NULLSTATE;
}



void CMapState::clear()
{
    this->clear_balls();
    this->clear_bricks();
}



void CMapState::render()
{
    this->render_balls();
    this->render_paddle();
    this->render_bricks();
    
    this->gc->window->draw(*selector);
}

void CMapState::update_balls(const float dt)
{
    ball_iterator I = this->balls->begin();
    ball_iterator E = this->balls->end();
    
    while(I != E)
    {
        CBall *ball = (*I);
        ball->update(dt);
        ++I;
    }    
}

void CMapState::update_bricks(const float dt)
{
    brick_iterator I = this->bricks->begin();
    brick_iterator E = this->bricks->end();
    
    while(I != E)
    {
        CBrick *b = (*I);
        b->update(dt);
        ++I;
    }
}

void CMapState::update_paddle(const float dt)
{
    this->paddle->update(dt);
}



void CMapState::render_balls()
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

void CMapState::render_bricks()
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

void CMapState::render_paddle()
{
    this->gc->window->draw(*(this->paddle));
}


void CMapState::clear_balls()
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

void CMapState::clear_bricks()
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


void CMapState::get_map()
{
    std::ofstream map;
    std::stringstream path;
    path << "media/maps/" << "piloto.dat";
    
    map.open(path.str(), std::ios::out);
    
    if (!map.is_open())
    {
        throw std::runtime_error("Failed to open map: ./media/maps/piloto.dat");
    }
    
    map << "hola\n";
    map << "adios\n";
    
    map.close();
}

void CMapState::load_textures()
{
    this->rh->load(game::game_textures::textures::MAIN, "media/textures/base.png");
    this->rh->load(game::game_textures::textures::BRICKS, "media/textures/bricks.png");
}

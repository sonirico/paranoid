/* 
 * File:   CBall.cpp
 * Author: sheldor
 * 
 * Created on 3 de abril de 2014, 1:03
 */

#include "CBall.hpp"

CBall::CBall(CState *st, CPaddle *pd)
{
    this->state = st;
    this->paddle = pd;
    
    this->init();
}

CBall::CBall(const CBall& orig)
{
}

CBall::~CBall()
{
    delete this->current_animation;
}
/*
void CBall::set_id(unsigned int id)
{
    this->id = id;
}

int CBall::get_id()
{
    return this->id;
}*/


void CBall::init()
{
    this->removable = false;
    this->in_paddle = false;
    
    this->velocity.x = this->vel * 0.5;
    this->velocity.y = this->vel * -0.5;
    
    this->scale(this->scalation.x, this->scalation.y);
    
    this->current_animation = new Animation();
    
    this->current_animation->setSpriteSheet(this->state->rh->get(game::game_textures::textures::MAIN));
    this->current_animation->addFrame(sf::IntRect(64, 16, 5, 4));//sf::IntRect(48, 8, 8, 8));
    
    this->animated_sprite = AnimatedSprite(sf::seconds(-1), true, false);
    this->animated_sprite.play(*(this->current_animation));
    
    this->bounds.x = this->animated_sprite.getGlobalBounds().width;
    this->bounds.y = this->animated_sprite.getGlobalBounds().height;
    
    this->bounds.x *= this->scalation.x;
    this->bounds.y *= this->scalation.y;
}

void CBall::update(const float dt)
{
    if (this->in_paddle)
    {
        this->set_in_paddle();
    }
    else
    {
        this->move(this->velocity * dt);
        this->collision_ball_paddle(dt);
    }
    /*
    //Tiempo para
    this->c_collisionable += dt;
    
    if (this->c_collisionable >= this->t_collisionable)
    {
        this->collisionable = true;
        this->c_collisionable = 0;
    }
    */
    this->check_bounds();            
}

void CBall::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= this->getTransform();
    
    target.draw(this->animated_sprite, states);    
}

void CBall::reset()
{
}

void CBall::set_in_paddle()
{
    sf::Vector2f pos;
    
    sf::Vector2f p_pos = this->paddle->getPosition();
    sf::FloatRect p_bounds = this->paddle->get_bounds();
    
    pos.x = p_pos.x + (p_bounds.width * 2 - this->bounds.x) / 2;
    pos.y = p_pos.y - this->bounds.y;
    
    this->setPosition(pos);
    
    this->set_in_paddle(true);
}

void CBall::set_in_paddle(bool b)
{
    this->in_paddle = b;
}

void CBall::check_bounds()
{
    sf::Vector2f pos = this->getPosition();
    
    unsigned int w = static_cast<int>(this->bounds.x);
    unsigned int h = static_cast<int>(this->bounds.y);
    
    if (pos.x < 0)
    {
        pos.x = 0;
        this->velocity.x *= -1;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
        
    }
    else if (pos.x + w > game::WIDTH)
    {
        pos.x = game::WIDTH - w;
        this->velocity.x *= -1;        
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    if (pos.y < 0)
    {
        pos.y = 0;
        this->velocity.y *= -1;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    else if (pos.y + h > game::HEIGHT)
    {
        this->removable = true;
    }
    
    this->setPosition(pos);
}

bool CBall::is_in_paddle()
{
    return this->in_paddle;
}

bool CBall::is_collisionable()
{
    return false;//this->collisionable;
}

void CBall::collision_ball_paddle(const float dt)
{
    
    if (this->paddle == NULL ) return;
    
    float x1, w1, h1, y1, x2, y2, w2, h2;
    
    x1 = this->getPosition().x;
    y1 = this->getPosition().y;
    w1 = this->bounds.x;
    h1 = this->bounds.y;
    
    x2 = this->paddle->getPosition().x;
    y2 = this->paddle->getPosition().y;
    w2 = this->paddle->get_bounds().width  * 2;
    h2 = this->paddle->get_bounds().height * 2;
    
    if ( (x1 + w1) < x2 ) return;
    if ( (y1 + h1) < y2 ) return;
    if ( (x2 + w2) < x1 ) return;
    if ( (y2 + h2) < y1 ) return;
    
    this->state->gc->play_fx(game::game_fx::fx::REBOTE2);
    
    //Si llegamos aqui, ha colisionado con la pala
    //Recalculamos la posicion de la bola
    //Recalculamos su vector velocidad en funcion de si ha golpeado
    //el paddle por los lados o por arriba
    sf::Vector2f new_pos;
    //Si la bola golpea los lados, cae y perdemos
    if ( y1 > y2 )
    {
        new_pos.y = y1;
        
        float overlap_right = std::abs(x2 + w2 - x1);
        float overlap_left = std::abs(x1 + w1 - x2);
        //Si viene por la derecha
        if ( overlap_left > overlap_right)
        {
            new_pos.x = x2 + w2;
        }
        //Por la izquierda
        else
        {
            new_pos.x = x2 - w1;
        }
        
        this->setPosition(new_pos);
        this->velocity.x *= -1;
    }
    else
    {
        new_pos.y = y2 - w1 - 1;
        new_pos.x = x1;
        this->setPosition(new_pos);
        
        float cp = (x2 + (w2 / 2));//Centro de la pala
        float cb = (x1 + (w1 / 2));//Centro de la bola
        //Calculamos los radianes: (distancia entre centros) / 
        // (radio de la circunferencia goniometrica de radio la mitad de la pala)
        float radians = std::abs((cp - cb) / (w2 / 2));//numero entre -1 y 1
        //Aplicamos ciertos limites para que no quede muy paralela
        if (radians > 0.7f)
            radians = 0.7f;
        //Nueva direccion en el eje x.
        int dir = cp > cb ? -1 : 1;
        //Self explanatory
        this->velocity.x = this->vel * radians * dir;
        //En el eje Y siempre saldrá negativa
        this->velocity.y = - (this->vel * (1 - radians));
    }
    
}

bool CBall::check_point_rect(sf::Vector2f point, CBrick* b)
{
    float x, y, w, h;
    
    x = b->getPosition().x;
    y = b->getPosition().y;
    w = b->get_bounds().width  * 2;
    h = b->get_bounds().height * 2;
    
    float xx = point.x;
    float yy = point.y;
    
    if ( xx < x || xx > x + w ) return false;
    if ( yy < y || yy > y + h ) return false;
    
    return true;
}

bool CBall::collision_ball_brick(CBrick* b, const float dt)
{
    bool collided = false;
    
    while(this->check_collision(b))
    {
        //Se mueve la bola hacia atrás hasta que los puntos cardinales
        //no colisionen ...
        this->move(this->velocity * -dt);
        collided = true;
    }
    
    if (collided)
    {
        /* THE HORRIBLE HACK */
        // ... pero cuando chocan las esquinas, hay que rectificar un poco más
        float m_w = this->bounds.x / 2;
        float m_h = this->bounds.y / 2;        
        
        if (this->dirx)
        {
            this->velocity.x *= -1;
            
            if (this->getPosition().x < b->getPosition().x)
                this->move(-m_w, 0);
            else
                this->move(m_w, 0);
        } 
        if (this->diry)
        {
            this->velocity.y *= -1;
            
            if (this->getPosition().y < b->getPosition().y)
                this->move(0, -m_h);
            else
                this->move(0, m_h);
        } 
        //Estos booleans indican true si el sentido de la velocidad ha cambiado
        this->dirx = false;
        this->diry = false;
    }
    
    return collided;
}

bool CBall::check_collision(CBrick *b)
{    
    sf::Vector2f left   = sf::Vector2f(this->getPosition().x , this->getPosition().y + this->bounds.y / 2);
    sf::Vector2f right  = sf::Vector2f(this->getPosition().x + this->bounds.x, this->getPosition().y + this->bounds.y / 2);
    sf::Vector2f top    = sf::Vector2f(this->getPosition().x + this->bounds.x / 2, this->getPosition().y);
    sf::Vector2f bottom = sf::Vector2f(this->getPosition().x + this->bounds.x / 2, this->getPosition().y + this->bounds.y);
    
    bool col = false;
    //Eje X
    if (this->check_point_rect(left, b))
    {
        dirx = true;
        col = true;
    }
    else if (this->check_point_rect(right, b))
    {
        dirx = true;
        col = true;
    }
    //Eje Y
    if (this->check_point_rect(top, b))
    {
        diry = true;
        col = true;
    }    
    else if (this->check_point_rect(bottom, b))
    {
        diry = true;
        col = true;
    }
    
    return col;
}

bool CBall::is_removable()
{
    return this->removable;
}
/*(((x1+w1)>=x2) &&
                ((y1+h1)>=y2) &&
                    ((x2+w2)>=x1) &&
                        ((y2+h2)>=y1))*/

#include "map.h"
#include <algorithm>
#include <ostream>
#include <random>
#include <fstream>

static double rand_0to1()
{
    return (double)std::rand() / ((double)RAND_MAX);
}

static double distance(std::array<double, 2> pt1, std::array<double, 2> pt2)
{
    return sqrt((pt1.at(0) - pt2.at(0)) * (pt1.at(0) - pt2.at(0)) + (pt1.at(1) - pt2.at(1)) * (pt1.at(1) - pt2.at(1)));
}

bool Safe_zone::is_in(std::array<double,2> pt) const{
    return distance(pt,{x,y})<radius;
}

Map::Map(double width, double height, unsigned int nb_birds, bool periodic, unsigned int size_brain, double speed_bird, double max_angle)
    : m_height(height), m_width(width), m_nb_birds(nb_birds), m_nb_alive_birds(nb_birds),
    m_periodic(periodic)
{
    if (nb_birds > MAX_BIRDS_NUMBER)
    {
        std::cerr << "Too much birds" << std::endl;
        return;
    }
    for (int i = 0; i < nb_birds; i++)
    {
        double x = rand_0to1() * width;
        double y = rand_0to1() * height;
        // Bird bird(x, y, size_brain);
        Bird bird(x, y, size_brain, speed_bird,max_angle);
        m_birds.at(i) = bird;
    }
}

Map::Map(Brain brain, double width, double height, unsigned int nb_birds, bool periodic, double speed_bird, double max_angle)
    : m_height(height), m_width(width), m_nb_birds(nb_birds), m_nb_alive_birds(nb_birds),
    m_periodic(periodic)
{
    for (int i = 0; i < nb_birds; i++)
    {
        double x = rand_0to1() * width;
        double y = rand_0to1() * height;
        Bird bird(x, y, brain, speed_bird,max_angle);
        m_birds.at(i) = bird;
    }
}

bool Map::is_in(std::array<double, 2> pt) const
{
    // std::cout << " Is " << pt[0] << " , " << pt[1] << " in ?" << std::endl;
    return (pt[0] < m_width && pt[0] > 0 && pt[1] < m_height && pt[1] > 0);
}

void Map::reproduce_alive(double mutation_size,bool rnd)
{
    if (m_nb_alive_birds<=0)
    {
        std::cerr << "No bird to reproduce" << std::endl;
        return;
    }
    int reproduction_rate=m_nb_birds/m_nb_alive_birds;
    // std::cout << "nb_birds=" << m_nb_birds << " and nb_alive = " << m_nb_alive_birds << std::endl;
    // std::cout << "the reproduction rate is : " << reproduction_rate<< std::endl;
    // size_t writing_index=0;
    size_t index_alive_bird=0;
    size_t index;
    //std::cout << "we have max " << m_nb_birds << " and currently" << m_nb_alive_birds <<std::endl;
    std::array<Bird,MAX_BIRDS_NUMBER> new_birds;
    for (size_t reading_index = 0; reading_index < m_nb_birds; reading_index++)
    {
        if (!m_birds.at(reading_index).is_alive()) continue;
        for (size_t i = 0; i < reproduction_rate; i++)
        {
            // std::cout << "we try to access " << index_alive_bird*reproduction_rate+i << std::endl;
            // std::cout << "Reading index = " << reading_index << " and wiriting index = " << writing_index<< std::endl;
            // if (writing_index>=m_nb_birds){
            //     //std::cerr << "Error in the reproduction" << std::endl;
            //     return;
            // }
            index=index_alive_bird*reproduction_rate+i;
            new_birds.at(index)=m_birds.at(reading_index);
            //std::cout << "index: " << index << std::endl;
            new_birds.at(index).mutate(mutation_size);
            if (rnd) {
                new_birds.at(index).setPosition({rand_0to1()*m_width,rand_0to1()*m_height});
            }
            //writing_index++;
        }
        //std::cout << "We ended at index : " << index_alive_bird*reproduction_rate+3<<std::endl;
        index_alive_bird++;
    }
    m_birds=new_birds;
}

void Map::do_one_step(std::array<double, 4> data)
{ // the data is {safe_zone.x, safe_zone.y, predator.x, predator.y}
    for (size_t i = 0; i < m_nb_birds; i++)
    {
        if (!m_birds.at(i).is_alive()){
            continue;
        }
        m_birds.at(i).update_direction({data.at(0), data.at(1), data.at(2), data.at(3), m_width, m_height});
        std::array<double, 2> next_position = m_birds.at(i).get_next_position();
        if (is_in(next_position))
        {
            m_birds[i].update_position();
        }
    }
}

void Map::kill_birds_in_circle(double x, double y, double radius)
{
    for (size_t i = 0; i < m_nb_birds; i++)
    {
        if (distance({x, y}, {m_birds.at(i).getX(), m_birds.at(i).getY()}) < radius)
            m_birds.at(i).kill();
    }
    int count = 0;
    for (size_t i = 0; i < m_nb_birds; i++)
    {
        if (m_birds[i].is_alive())
            count++;
    }
    m_nb_alive_birds = count;
}

unsigned int Map::get_nb_alive_birds() const
{
    return m_nb_alive_birds;
}

std::array<double, 2> Map::get_nearest_alive_bird(std::array<double, 2> pt) const
{
    std::array<double,2> nearest_bird;
    double dist=+INFINITY;
    double current_dist;
    Bird current_bird;

    for (size_t i = 0; i < m_nb_birds; i++)
    {
        current_bird=m_birds.at(i);
        current_dist=distance(pt,{current_bird.getX(),current_bird.getY()});
        if (dist>current_dist && current_bird.is_alive())
        {
            dist=current_dist;
            nearest_bird={current_bird.getX(),current_bird.getY()};
        }
    }
    return nearest_bird;
    
}

std::array<double,2> Map::get_nearest_alive_bird(std::array<double,2> pt,Safe_zone safe) const
{
    std::array<double,2> nearest_bird;
    double dist=+INFINITY;
    double current_dist;
    Bird current_bird;

    for (size_t i = 0; i < m_nb_birds; i++)
    {
        current_bird=m_birds.at(i);
        current_dist=distance(pt,{current_bird.getX(),current_bird.getY()});
        if (dist>current_dist && current_bird.is_alive() && !safe.is_in({current_bird.getX(),current_bird.getY()}))
        {
            dist=current_dist;
            nearest_bird={current_bird.getX(),current_bird.getY()};
        }
    }
    return nearest_bird;
    
}

void Map::draw(sf::RenderWindow *window) const
{
    //the birds
    sf::CircleShape bird_to_display;
    for (size_t i = 0; i < m_nb_birds; i++)
    {
        if (m_birds.at(i).is_alive())
        {

            m_birds.at(i).draw(window);
        }
    }
    
}

// save
/*
void Rectangle::save(std::string path){
    try
    {
        if (path == "")
            throw "Path empty";
        if (path.size() > 300)
            throw "Path too long";
        std::ofstream fichier_sortant (path.c_str(), std::ios::binary);
        if (!fichier_sortant)
        {
            std::cerr << "\a\n\nFile not created\n\n";
            return;
        }
        fichier_sortant.write ((char*) this, sizeof (*this));
        fichier_sortant.close();
    }
    catch (const char *exception)
    {
        std::cerr << "\n*** " << exception << " ***\n";
    }
    catch (...)
    {
        std::cerr << "\n*** En error occured ! ***\n";
    }
}

void Rectangle::read (std::string path)
{
    try
    {
        if (path == "")
            throw "Path empty";
        if (path.size() > 300)
            throw "Path too long";
        std::ifstream fichier_entrant(path.c_str(), std::ios::binary);
        if (!fichier_entrant)
        {
            std::cerr << "\a\n\nImpossible de lire le fichier de sauvegarde\n\n";
            return;
        }
        fichier_entrant.read((char*) this, sizeof(*this));
        fichier_entrant.close();
    }
    catch (const char *exception)
    {
        std::cerr << "\n*** " << exception << " ***\n";
    }
    catch (...)
    {
        std::cerr << "\n*** Une erreur s'est produite ! ***\n";
    }
}
*/

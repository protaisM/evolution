#include "experiment.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cstring>
#include <string>

static double rand_0to1(){
  return (double)std::rand() / ((double)RAND_MAX );
}

static double distance(std::array<double,2> pt1, std::array<double,2> pt2){
    return sqrt((pt1.at(0)-pt2.at(0))*(pt1.at(0)-pt2.at(0))+(pt1.at(1)-pt2.at(1))*(pt1.at(1)-pt2.at(1)));

}


    Chase::Chase(char title[40], double width, double height,
             unsigned int nb_birds , unsigned int reproduction_rate ,
             double mutation_size , unsigned int size_brain,
             bool randomized_position, double size_predator,
             double speed_predator, double size_safe_zone,
             double speed_bird, double max_angle_bird, int duration_day, bool periodic)
    :m_day(0),
    m_time(0),
    m_nb_birds(nb_birds),
    m_reproduction_rate(reproduction_rate),
    m_map(width, height, nb_birds,periodic,size_brain,speed_bird,max_angle_bird),
    m_map_width(width),
    m_map_height(height),
    m_mutation_size(mutation_size),
    m_size_brain(size_brain),
    m_randomized_positions(randomized_position),
    m_speed_bird(speed_bird),
    m_duration_day(duration_day)
{
    strcpy(m_title,title);
    m_safe_zone.x = rand_0to1() * m_map_width / 2;
    m_safe_zone.y = rand_0to1() * m_map_height / 2;
    m_safe_zone.radius = size_safe_zone;
    m_predator.x = (1 + rand_0to1()) * m_map_width / 2;
    m_predator.y = (1 + rand_0to1()) * m_map_height / 2;
    m_predator.radius = size_predator;
    m_predator.speed=speed_predator;

}

Chase::Chase(char title[40], Brain brain, double width, double height,
             unsigned int nb_birds , unsigned int reproduction_rate ,
             double mutation_size , unsigned int size_brain,
             bool randomized_position, double size_predator,
             double speed_predator, double size_safe_zone,
             double speed_bird, double max_angle_bird, int duration_day, bool periodic)
    :m_day(0),
    m_time(0),
    m_nb_birds(nb_birds),
    m_reproduction_rate(reproduction_rate),
    m_map(brain,width,height,nb_birds,periodic,speed_bird,max_angle_bird),
    //m_map(width, height, nb_birds,periodic,size_brain,speed_bird,max_angle_bird),
    m_map_width(width),
    m_map_height(height),
    m_mutation_size(mutation_size),
    m_size_brain(size_brain),
    m_randomized_positions(randomized_position),
    m_speed_bird(speed_bird),
    m_duration_day(duration_day)
{
    strcpy(m_title,title);
    m_safe_zone.x = rand_0to1() * m_map_width / 2;
    m_safe_zone.y = rand_0to1() * m_map_height / 2;
    m_safe_zone.radius = size_safe_zone;
    m_predator.x = (1 + rand_0to1()) * m_map_width / 2;
    m_predator.y = (1 + rand_0to1()) * m_map_height / 2;
    m_predator.radius = size_predator;
    m_predator.speed=speed_predator;

}
 /* Chase::Chase(std::string title)
:m_map(1000,1000,1000)
{
    if (title.length()>40) {
        std::cerr << "Title is limited to 40 char"<<std::endl;
        return;
    }
    char *m_title=title.data();
    std::cout << m_title << "end" << std::endl;
    this->resume();
} */


// Chase::Chase(char title[40], unsigned int width, unsigned int height, Brain brain,
//              unsigned int nb_birds , unsigned int reproduction_rate ,
//              double mutation_size , unsigned int size_brain,
//              bool randomized_position, double size_predator,
//              double speed_predator, double size_safe_zone,
//              double speed_bird, bool periodic){
// }

// Chase::Chase(char title[40], unsigned int width, unsigned int height, double mutation_size, unsigned int nb_birds, unsigned int reproduction_rate)
//       :m_day(0),
//       m_time(0),
//       m_nb_birds(nb_birds),
//       m_reproduction_rate(reproduction_rate),
//       m_map(width, height, nb_birds),
//       m_map_width(width),
//       m_map_height(height),
//       m_mutation_size(mutation_size)
// {
//
//     strcpy(m_title,title);
//     m_safe_zone.x = rand_0to1() * m_map_width / 2;
//     m_safe_zone.y = rand_0to1() * m_map_height / 2;
//     m_safe_zone.radius = 150;
//     m_predator.x = (1 + rand_0to1()) * m_map_width / 2;
//     m_predator.y = (1 + rand_0to1()) * m_map_height / 2;
//     m_predator.radius = 50;
// }
//
// Chase::Chase(char title[40], unsigned int width, unsigned int height, Brain brain, double mutation_size, unsigned int nb_birds, unsigned int reproduction_rate)
//     : m_day(0), m_time(0), m_nb_birds(nb_birds),
//       m_reproduction_rate(reproduction_rate), m_map(width, height, nb_birds, brain), m_map_width(width), m_map_height(height),
//       m_mutation_size(mutation_size)
// {
//     strcpy(m_title,title),
//     m_safe_zone.x = rand_0to1() * m_map_width / 2;
//     m_safe_zone.y = rand_0to1() * m_map_height / 2;
//     m_safe_zone.radius = 150;
//     m_predator.x = (1 + rand_0to1()) * m_map_width / 2;
//     m_predator.y = (1 + rand_0to1()) * m_map_height / 2;
//     m_predator.radius = 50;
// }
//
void Chase::do_one_step()
{
    m_map.do_one_step({m_safe_zone.x, m_safe_zone.y, m_predator.x, m_predator.y});
    move_predator();
    m_map.kill_birds_in_circle(m_predator.x, m_predator.y, m_predator.radius);
    m_time++;
    if (condition_end_of_day())
    {
        //std::cout << "end of the day !"<< std::endl;
        m_map.reproduce_alive(m_mutation_size,m_randomized_positions);
        move_safe_zone();
        m_day++;
        m_time -= m_time;
        save_current_state();
    }
}

void Chase::move_predator() {
    std::array<double,2> nearest_bird=m_map.get_nearest_alive_bird({m_predator.x,m_predator.y},m_safe_zone);
    std::array<double,2> next_pos=m_predator.get_next_position_toward(nearest_bird);
    if (m_map.is_in(next_pos) && distance(next_pos,{m_safe_zone.x,m_safe_zone.y})>=m_safe_zone.radius)
    {
        m_predator.x=next_pos.at(0);
        m_predator.y=next_pos.at(1);
    }else if (m_map.is_in(next_pos)){
        m_predator.x=rand_0to1()*m_map_width;
        m_predator.y=rand_0to1()*m_map_height;
    }
}

void Chase::move_safe_zone() {
    do
    {
        m_safe_zone.x=rand_0to1()*m_map_width;
        m_safe_zone.y=rand_0to1()*m_map_height;
    } while (distance({m_predator.x,m_predator.y},{m_safe_zone.x,m_safe_zone.y})<m_predator.radius+m_safe_zone.radius);
}

bool Chase::condition_end_of_day()
{
    //std::cout << "nb of alive : " << m_map.get_nb_alive_birds() << " on "<<m_nb_birds / m_reproduction_rate<< std::endl;
    return (m_map.get_nb_alive_birds() <= m_nb_birds / m_reproduction_rate) || m_time>=20000;
}

void Chase::run_on_background()
{
    while (true)
    {
        do_one_step();
    }
}

void Chase::run_and_display(){
    double space_right = sf::VideoMode::getDesktopMode().width - m_map_width;
    double space_bottom = sf::VideoMode::getDesktopMode().height - m_map_height;
    sf::RenderWindow window(sf::VideoMode(m_map_width + space_right, m_map_height + space_bottom), std::string(m_title));
    run_on_window(&window);

}

void handle_event(sf::RenderWindow* window,sf::Event evnt,int& screen) {
    switch (evnt.type)
    {
        case sf::Event::Closed: //close the window
            window->close();
            break;
        case (sf::Event::KeyReleased ):
            if (evnt.key.code==sf::Keyboard::Num1) //display screen 1
            {
                screen=1;
            }
            if (evnt.key.code==sf::Keyboard::Num2) //display screen 2 (no legend)
            {
                screen=2;
            }
            if (evnt.key.code==sf::Keyboard::Num3) //display screen 3 (only legend)
            {
                screen=3;
                window->clear();
            }
            if (evnt.key.code==sf::Keyboard::Num4) //display screen 4 (nothing)
            {
                screen=4;
                window->clear();
            }
            break;
        default:
            break;
    }
}

void Chase::run_on_window(sf::RenderWindow* window){
    int screen=1;
    double space_right = sf::VideoMode::getDesktopMode().width - m_map_width;
    double space_bottom = sf::VideoMode::getDesktopMode().height - m_map_height;
    while (window->isOpen())
    {
        sf::Event evnt;
        while (window->pollEvent(evnt)) {
            handle_event(window,evnt,screen);
        }
        do_one_step();
        draw(window,screen,space_right,space_bottom);

        /* switch (screen)
        {
        case 1:
            window->clear();
            m_map.draw(window);
            m_predator.draw(window);
            m_safe_zone.draw(window);
            draw_legend(window);
            window->display();
            break;
        } */

        //save_brain_to_file();
        //std::cout << "Number of birds : "<< m_nb_birds << " at time " << m_time << " and day " << m_day<<std::endl;
    }


}

void Chase::draw(sf::RenderWindow* window, int screen, int space_right, int space_bottom) const{
    window->clear();
    switch (screen) {
        case 1:
            m_map.draw(window);
            m_predator.draw(window);
            m_safe_zone.draw(window);
            draw_legend(window,space_right,space_bottom);
            window->display();
            break;
        case 2:
            m_map.draw(window);
            m_predator.draw(window);
            m_safe_zone.draw(window);
            window->display();
            break;
        case 3:
            draw_legend(window,space_right,space_bottom);
            window->display();
            break;
        case 4:
            break;
        default:
            break;
    }
}

void Chase::draw_legend(sf::RenderWindow* window,int space_right, int space_bottom) const{
    sf::Font font;
    font.loadFromFile("UbuntuMono-R.ttf");

    //legend at the bottom
    sf::Text legend;
    legend.setFont(font);
    std::string text_legend = " Birds alive : " + std::to_string(m_map.get_nb_alive_birds())+"\n";
    text_legend+= " Day " + std::to_string(m_day) + " and time " + std::to_string(m_time);
    legend.setString(text_legend);
    legend.setFillColor(sf::Color::White);
    legend.setPosition(0.0f,m_map_height);
    legend.setCharacterSize(20);

    //the right panel
    sf::Text panel;
    panel.setFont(font);
    std::string text_panel="Parameters of the experiment : \n";
    text_panel+= "Title : " + std::string(m_title) + "\n";
    text_panel+= "Map : (" + std::to_string(m_map_width) + "," + std::to_string(m_map_height)+")\n";
    text_panel+= "Max number of birds : " + std::to_string(m_nb_birds) + "\n";
    text_panel+= "Size of the brains : " + std::to_string(m_size_brain) + "\n";
    text_panel+= "Speed of the birds : " + std::to_string(m_speed_bird) + "\n";
    text_panel+= "Duration of the day : " + std::to_string(m_duration_day) + "\n";
    text_panel+= "Reproduction rate : " + std::to_string(m_reproduction_rate) + "\n";
    text_panel+= "Mutation size : " + std::to_string(m_mutation_size) + "\n";
    text_panel+= "Size of the safe zone : " + std::to_string(m_safe_zone.radius) + "\n";
    text_panel+= "Size of the predator : " + std::to_string(m_predator.radius) + "\n";
    text_panel+= "Speed of the predator : " + std::to_string(m_predator.speed) + "\n";
    text_panel+= "\n";
    text_panel+= "Day : " + std::to_string(m_day) + ", Time :  " + std::to_string(m_time) + "\n";
    text_panel+= "Remaining birds : " + std::to_string(m_map.get_nb_alive_birds()) + "\n";

    panel.setString(text_panel);
    panel.setFillColor(sf::Color::White);
    panel.setPosition(m_map_width+10,0.0f);
    panel.setCharacterSize(20);

    //the black boundary
    sf::RectangleShape boundary_right(sf::Vector2f(space_right,m_map_height+space_bottom));
    boundary_right.setPosition(m_map_width,0.0f);
    boundary_right.setFillColor(sf::Color::Black);

    sf::RectangleShape boundary_down(sf::Vector2f(m_map_width, space_bottom));
    boundary_down.setPosition(0.0f,m_map_height);
    boundary_down.setFillColor(sf::Color::Black);

    //the lines around the map
    sf::Vertex line_bottom[] =
    {
        sf::Vertex(sf::Vector2f(0.0f,m_map_height)),
        sf::Vertex(sf::Vector2f(m_map_width,m_map_height))
    };

    sf::Vertex line_right[] =
    {
        sf::Vertex(sf::Vector2f(m_map_width,0.0f)),
        sf::Vertex(sf::Vector2f(m_map_width,m_map_height))
    };
    window->draw(boundary_right);
    window->draw(boundary_down);
    window->draw(line_bottom, 2, sf::Lines);
    window->draw(line_right, 2, sf::Lines);

    window->draw(panel);
    window->draw(legend);
}


void Chase::save_brain_to_file(/*ostream ?*/) {}
void Chase::save_current_state() {
    //std::remove(m_title+".log");
    //std::cout << m_day<< std::endl;
    std::fstream out(m_title,std::ios::out | std::ios::binary);
    if (!out)
    {
        std::cerr << "\a\n\nFile not created\n\n";
        return;
    }
    out.write (reinterpret_cast<char*>(this), sizeof (Chase));
    out.close();
}

void Chase::resume() {
    std::fstream in(m_title, std::ios::in | std::ios::binary);
    if (!in)
    {
        std::cerr << "\a\n\nLog file not found\n\n";
        return;
    }
    in.read(reinterpret_cast<char*>(this), sizeof (Chase));
    in.close();
    // std::ifstream fichier_entrant(m_title + ".log", std::ios::binary);
    // if (!fichier_entrant)
    // {
    //     std::cerr << "\a\n\nCannot resume : no log file\n\n";
    //     return;
    // }
    // fichier_entrant.read((char*) this, sizeof(*this));
    // fichier_entrant.close();
}

void Chase::set_random_pos(bool rnd){
    m_randomized_positions= rnd;
}

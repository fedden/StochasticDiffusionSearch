#pragma once

#include "ofMain.h"
#include <random>

class agent
{
public:
    size_t x, y;
    bool happy;
   
    void set_happy(const std::vector<std::vector<int>>& world)
    {
        happy = world[x][y] == 1;
    }
};

class random_uniform
{
private:
    std::random_device random_device;
    std::mt19937 random_number_generator;
    std::uniform_int_distribution<size_t> uniform_distribution;
    
public:
    random_uniform(size_t min, size_t max) :
        random_device{},
        random_number_generator{random_device()},
        uniform_distribution{min, max}
    {}
    
    size_t get_next()
    {
        const size_t result = uniform_distribution(random_number_generator);
        return result;
    }
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
private:
    std::unique_ptr<random_uniform> uniform_random;
    
    std::size_t grid_size;
    std::size_t partial_size;
    std::vector<std::vector<int>> partial_grid;
    
    float draw_scalar;
    
    size_t agent_size;
    std::vector<agent> happy_agents;
    std::vector<agent> unhappy_agents;
    std::vector<agent> agents;
    
    bool run;
		
};

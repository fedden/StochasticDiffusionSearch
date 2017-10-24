#pragma once

#include "ofMain.h"
#include <random>

class agent
{
public:
    size_t x, y;
    bool happy;
   
    bool set_happy(const std::vector<std::vector<int>>& world)
    {
        happy = world[x][y] == 1;
        return happy;
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
    void mousePressed(int x, int y, int button);
    
private:
    std::unique_ptr<random_uniform> uniform_random;
    
    std::size_t grid_size;
    std::size_t partial_size;
    std::vector<std::vector<int>> partial_grid;
    std::map<size_t, size_t> most_frequent_hill_indices;
    std::array<size_t, 2> best_hill_coordinates;
    
    float draw_scalar;
    
    size_t agent_size;
    std::vector<std::shared_ptr<agent>> happy_agents;
    std::vector<std::shared_ptr<agent>> unhappy_agents;
    std::vector<std::shared_ptr<agent>> agents;
    
    bool run;
		
};

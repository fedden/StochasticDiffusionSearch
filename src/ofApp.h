#pragma once

#include "ofMain.h"
#include <random>

//--------------------------------------------------------------
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

//--------------------------------------------------------------
class random_uniform
{
private:
    std::random_device random_device;
    std::mt19937 random_number_generator;
    std::uniform_real_distribution<double> uniform_distribution;
    
public:
    random_uniform() :
        random_device{},
        random_number_generator{random_device()},
        uniform_distribution{0.0, 1.0}
    {}
    
    size_t get_next(size_t max)
    {
        const double result = uniform_distribution(random_number_generator);
        return size_t(std::round(result * max));
    }
};

//--------------------------------------------------------------
class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    
private:
    random_uniform uniform_random;
    
    size_t grid_size;
    size_t partial_size;
    std::vector<std::vector<int>> partial_grid;
    std::map<size_t, size_t> most_frequent_hill_indices;
    std::array<size_t, 2> best_hill_coordinates;
    
    float draw_scalar;
    
    size_t agent_size;
    std::vector<std::shared_ptr<agent>> happy_agents;
    std::vector<std::shared_ptr<agent>> unhappy_agents;
    std::vector<std::shared_ptr<agent>> agents;
    
    bool run;
    unsigned long long iteration;
		
};

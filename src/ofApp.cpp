#include "ofApp.h"

//--------------------------------------------------------------
void set_agent_randomly_in_same_quadrant(const agent& happy, agent& unhappy, size_t quad_size, random_uniform& uniform_random)
{
    size_t quadrant_start_x = happy.x - (happy.x % quad_size);
    size_t quadrant_start_y = happy.y - (happy.y % quad_size);
    size_t random_x = uniform_random.get_next() % quad_size;
    size_t random_y = uniform_random.get_next() % quad_size;
    unhappy.x = random_x + quadrant_start_x;
    unhappy.y = random_y + quadrant_start_y;
}

//--------------------------------------------------------------
array<size_t, 2> get_hill_position(size_t index, size_t quad_size)
{
    size_t remainder = index % quad_size;
    return {remainder, index - remainder};
}

//--------------------------------------------------------------
size_t get_hill_index(agent& agent, size_t quad_size)
{
    size_t quadrant_start_x = agent.x - (agent.x % quad_size);
    size_t quadrant_start_y = agent.y - (agent.y % quad_size);
    return quadrant_start_x + quadrant_start_y * quad_size;
}

//--------------------------------------------------------------
void grid_world_middle_bias(std::vector<std::vector<int>>& world,
                            random_uniform& uniform_random)
{
    for (size_t x = 0; x < world.size(); ++x)
    {
        for (size_t y = 0; y < world[x].size(); ++y)
        {
            const float centre_x = float(world.size()) / 2.0;
            const float centre_y = float(world.size()) / 2.0;
            const size_t prob_x = abs(centre_x - x);
            const size_t prob_y = abs(centre_y - y);
            const size_t prob = size_t(float(prob_x + prob_y) / 2.0);
            world[x][y] = uniform_random.get_next() % (prob + 1) == 1;
        }
    }
}


//--------------------------------------------------------------
void ofApp::setup()
{
    run = false;
    grid_size = 100;
    partial_size = 5;
    
    // We need complete hills
    assert(grid_size % partial_size == 0);
    
    uniform_random = std::make_unique<random_uniform>(0, std::max(agent_size, grid_size));
    
    partial_grid.clear();
    partial_grid.resize(grid_size);
    for (auto& col : partial_grid)
        col.resize(grid_size, 0);
    
    agent_size = 100;
    
    agents.clear();
    agents.reserve(agent_size);
    for (size_t i = 0; i < agent_size; ++i)
    {
        auto a = std::make_shared<agent>();
        a->x = uniform_random->get_next() % agent_size;
        a->y = uniform_random->get_next() % agent_size;
        a->happy = false;
        agents.push_back(a);
    }
    
    happy_agents.reserve(agents.size());
    unhappy_agents.reserve(agents.size());
    
    for (auto& col : partial_grid)
        for (auto& element : col)
            element = uniform_random->get_next() % 100 == 0;
    
    grid_world_middle_bias(partial_grid, (*uniform_random));
    
    // Needs to be true for layout
    assert(ofGetWidth() == ofGetHeight());
    
    draw_scalar = float(ofGetWidth()) / float(grid_size);
}

//--------------------------------------------------------------
void ofApp::update(){
    happy_agents.clear();
    unhappy_agents.clear();
    
    size_t max_indices = 0;
    size_t best_index = 0;
    most_frequent_hill_indices.clear();
    for (auto& agent : agents)
    {
        if (agent->set_happy(partial_grid))
        {
            happy_agents.push_back(agent);
            const size_t hill_index = get_hill_index((*agent), partial_size);
            
            if (++most_frequent_hill_indices[hill_index] > max_indices)
            {
                max_indices = most_frequent_hill_indices[hill_index];
                best_index = hill_index;
            }
        }
        else
            unhappy_agents.push_back(agent);
    }
    
    best_hill_coordinates = get_hill_position(best_index, partial_size);
    
    for (auto& agent : unhappy_agents)
    {
        if (happy_agents.size() > 0)
        {
            size_t random_index = uniform_random->get_next() % agents.size();
            const auto a = (*agents[random_index]);
            if (a.happy)
            {
                set_agent_randomly_in_same_quadrant(a,
                                                    (*agent),
                                                    partial_size,
                                                    (*uniform_random));
            }
            else
            {
                agent->x = uniform_random->get_next() % agent_size;
                agent->y = uniform_random->get_next() % agent_size;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // Gold or not gold?
    for (size_t x = 0; x < grid_size; ++x)
    {
        for (size_t y = 0; y < grid_size; ++y)
        {
            const ofColor c = (partial_grid[x][y] == 0) ? ofColor::black : ofColor::gold;
            ofSetColor(c);
            ofDrawRectangle(x * draw_scalar, y * draw_scalar, draw_scalar, draw_scalar);
        }
    }
    
    // Grid lines
    ofSetColor(255);
    for (size_t i = 0; i < grid_size / partial_size; ++i)
    {
        const float pos = i * partial_size * draw_scalar;
        const float max = grid_size / partial_size * partial_size * draw_scalar;
        ofDrawLine(pos, 0, pos, max);
        ofDrawLine(0, pos, max, pos);
    }
    
    // Best partial
    ofSetColor(255, 20);
    ofDrawRectangle(best_hill_coordinates[0] * draw_scalar, best_hill_coordinates[1] * draw_scalar, partial_size * draw_scalar, partial_size * draw_scalar);
    
    
    // Agents
    ofSetColor(ofColor::red);
    const float increment = float(partial_size) / 2.0;
    for (auto& agent : agents)
    {
        ofDrawCircle(agent->x * draw_scalar + increment, agent->y * draw_scalar + increment, increment / 2.0);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    run = !run;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    setup();
}

#include "ofApp.h"



//--------------------------------------------------------------
std::array<size_t, 2> get_hill_position(size_t index,
                                        size_t quad_size,
                                        size_t grid_size,
                                        float draw_scalar)
{
    const auto step = grid_size / quad_size;
    const size_t remainder = index % step;
    const size_t x = remainder * draw_scalar * quad_size;
    const size_t y = (float(index - remainder) / step) * draw_scalar * quad_size;
    return {x, y};
}

//--------------------------------------------------------------
size_t get_hill_index(agent& agent,
                      size_t quad_size,
                      size_t grid_size)
{
    const size_t quadrant_start_x = (agent.x - (agent.x % quad_size)) / quad_size;
    const size_t quadrant_start_y = (agent.y - (agent.y % quad_size)) / quad_size;
    return quadrant_start_x + quadrant_start_y * grid_size / quad_size;
}

//--------------------------------------------------------------
void grid_world_middle_bias(std::vector<std::vector<std::pair<int, bool>>>& world,
                            random_uniform& uniform_random)
{
    for (size_t x = 0; x < world.size(); ++x)
    {
        for (size_t y = 0; y < world[x].size(); ++y)
        {
            const float centre_x = float(world.size()) / 2.0f;
            const float centre_y = float(world.size()) / 2.0f;
            const size_t prob_x = abs(centre_x - x);
            const size_t prob_y = abs(centre_y - y);
            const size_t prob = size_t(float(prob_x + prob_y) / 3.0f);
            world[x][y].first = uniform_random.get_next(prob * prob + 1) == 1;
            world[x][y].second = false;
        }
    }
}

//--------------------------------------------------------------
void grid_world_moving_noise(std::vector<std::vector<std::pair<int, bool>>>& world,
                             unsigned long long iteration)
{
    const float scale = 0.01f;
    const float speed = 0.005f;
    for (float x = 0; x < float(world.size()); ++x)
    {
        for (float y = 0; y < float(world.size()); ++y)
        {
            world[x][y].first = ofNoise(x * scale, y * scale, float(iteration) * speed) > 0.9;
            world[x][y].second = false;
        }
    }
}

//--------------------------------------------------------------
void clear_grid_world(std::vector<std::vector<std::pair<int, bool>>>& world,
                      std::vector<std::shared_ptr<agent>>& agents)
{
    for (size_t x = 0; x < world.size(); ++x)
        for (size_t y = 0; y < world.size(); ++y)
            world[x][y].second = false;
    
    for (auto& agent : agents)
        world[agent->x][agent->y].second = true;
}

//--------------------------------------------------------------
bool agent_is_in_same_position(std::shared_ptr<agent>& prospective_agent,
                               std::vector<std::shared_ptr<agent>>& agents)
{
    for (auto& agent : agents)
        if (agent->x == prospective_agent->x && agent->y == prospective_agent->y)
            return false;
    return true;
}

//--------------------------------------------------------------
void set_agent_randomly_in_same_quadrant(const std::shared_ptr<agent>& happy,
                                         std::shared_ptr<agent>& unhappy,
                                         std::vector<std::shared_ptr<agent>>& agents,
                                         std::vector<std::vector<std::pair<int, bool>>>& world,
                                         size_t quad_size,
                                         std::size_t grid_size,
                                         random_uniform& uniform_random)
{
    const size_t quadrant_start_x = happy->x - (happy->x % quad_size);
    const size_t quadrant_start_y = happy->y - (happy->y % quad_size);
    const size_t random_x = uniform_random.get_next(quad_size);
    const size_t random_y = uniform_random.get_next(quad_size);
    const size_t x = std::min(random_x + quadrant_start_x, grid_size - 1);
    const size_t y = std::min(random_y + quadrant_start_y, grid_size - 1);
    const bool already_being_mined = world[x][y].second;
    
    world[unhappy->x][unhappy->y].second = false;
    if (already_being_mined)
    {
        unhappy->x = uniform_random.get_next(grid_size - 1);
        unhappy->y = uniform_random.get_next(grid_size - 1);
    }
    else
    {
        unhappy->x = x;
        unhappy->y = y;
    }
    world[unhappy->x][unhappy->y].second = true;
}


//--------------------------------------------------------------
void ofApp::setup()
{
    save_output = true;
    max_iteration = 150;
    
    run = false;
    noise = false;
    moving = false;
    iteration = 0;
    grid_size = 200;
    partial_size = 20;
    
    // We need complete hills
    assert(grid_size % partial_size == 0);
    
    partial_grid.clear();
    partial_grid.resize(grid_size);
    for (auto& col : partial_grid)
        col.resize(grid_size, std::make_pair(0, false));
    
    agent_size = 100;
    
    agents.clear();
    agents.reserve(agent_size);
    for (size_t i = 0; i < agent_size; ++i)
    {
        auto a = std::make_shared<agent>();
        a->x = uniform_random.get_next(grid_size - 1);
        a->y = uniform_random.get_next(grid_size - 1);
        a->happy = false;
        partial_grid[a->x][a->y].second = true;
        agents.push_back(a);
    }
    
    happy_agents.clear();
    happy_agents.reserve(agents.size());
    unhappy_agents.clear();
    unhappy_agents.reserve(agents.size());
    
    if (noise)
        grid_world_moving_noise(partial_grid, iteration);
    else
        grid_world_middle_bias(partial_grid, uniform_random);
    
    draw_scalar = float(ofGetWidth()) / float(grid_size);
    
    ofBackground(80);
    
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "Run-%d-%m-%Y-%H-%M-%S");
    save_name = oss.str();
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (ofGetMousePressed())
    {
        setup();
        ofSetWindowTitle(std::string("Setting up!"));
    }
    else if (run)
    {
        if (noise && moving)
            grid_world_moving_noise(partial_grid, iteration);
        else
            clear_grid_world(partial_grid, agents);
        
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
                const size_t hill_index = get_hill_index((*agent), partial_size, grid_size);
                
                if (++most_frequent_hill_indices[hill_index] > max_indices)
                {
                    max_indices = most_frequent_hill_indices[hill_index];
                    best_index = hill_index;
                }
            }
            else
                unhappy_agents.push_back(agent);
        }
        
        best_hill_coordinates = get_hill_position(best_index, partial_size, grid_size, draw_scalar);
        
        for (auto& agent : unhappy_agents)
        {
            if (happy_agents.size() > 0)
            {
                size_t random_index = uniform_random.get_next(agent_size - 1);
                if (agents[random_index]->happy)
                {
                    set_agent_randomly_in_same_quadrant(agents[random_index],
                                                        agent,
                                                        agents,
                                                        partial_grid,
                                                        partial_size,
                                                        grid_size,
                                                        uniform_random);
                }
                else
                {
                    agent->x = uniform_random.get_next(grid_size - 1);
                    agent->y = uniform_random.get_next(grid_size - 1);
                    
                }
            }
            else
            {
                agent->x = uniform_random.get_next(grid_size - 1);
                agent->y = uniform_random.get_next(grid_size - 1);
            }
        }
        
        ofSetWindowTitle(save_name + std::string(": ") + std::to_string(iteration));
    }
    else
    {
        ofSetWindowTitle(std::string("NOT RUNNING"));
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
            const ofColor c = (partial_grid[x][y].first == 0) ? ofColor::black : ofColor::gold;
            ofSetColor(c);
            ofDrawRectangle(x * draw_scalar, y * draw_scalar, draw_scalar, draw_scalar);
        }
    }
    
    // Grid lines
    ofSetColor(255, 125);
    for (size_t i = 1; i < grid_size / partial_size; ++i)
    {
        const float pos = i * partial_size * draw_scalar;
        const float max = grid_size / partial_size * partial_size * draw_scalar;
        ofDrawLine(pos, 0, pos, max);
        ofDrawLine(0, pos, max, pos);
    }
    
    // Best partial
    ofSetColor(255, 127);
    ofDrawRectangle(best_hill_coordinates[0], best_hill_coordinates[1], partial_size * draw_scalar, partial_size * draw_scalar);
    ofSetColor(ofColor::red);
    ofDrawLine(best_hill_coordinates[0], best_hill_coordinates[1], best_hill_coordinates[0] + partial_size * draw_scalar, best_hill_coordinates[1]);
    ofDrawLine(best_hill_coordinates[0], best_hill_coordinates[1], best_hill_coordinates[0], best_hill_coordinates[1] + partial_size * draw_scalar);
    ofDrawLine(best_hill_coordinates[0], best_hill_coordinates[1] + partial_size * draw_scalar, best_hill_coordinates[0] + partial_size * draw_scalar, best_hill_coordinates[1] + partial_size * draw_scalar);
    ofDrawLine(best_hill_coordinates[0] + partial_size * draw_scalar, best_hill_coordinates[1] + partial_size * draw_scalar, best_hill_coordinates[0] + partial_size * draw_scalar, best_hill_coordinates[1]);
    
    // Agents
    const float increment = float(partial_size) / 2.0;
    const float inc = draw_scalar / 2.0;
    for (auto& agent : agents)
    {
        ofDrawCircle(agent->x * draw_scalar + inc, agent->y * draw_scalar + inc, draw_scalar / 3.0);
    }
    
    if (run)
    {
        ++iteration;
        if (save_output)
            ofSaveScreen(save_name + std::to_string(iteration) + ".png");
        
        if (save_output && iteration > max_iteration)
            ofExit();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    run = !run;
}

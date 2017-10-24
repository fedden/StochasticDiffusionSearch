#include "ofApp.h"

void set_agent_randomly_in_same_quadrant(const agent& happy, agent& unhappy, size_t quad_size, random_uniform& uniform_random)
{
    size_t quadrant_start_x = size_t(((happy.x) / float(quad_size)) * quad_size);
    size_t quadrant_start_y = size_t(((happy.y) / float(quad_size)) * quad_size);
    size_t random_x = uniform_random.get_next() % quad_size;
    size_t random_y = uniform_random.get_next() % quad_size;
    unhappy.x = random_x + quadrant_start_x;
    unhappy.y = random_y + quadrant_start_y;
}

//--------------------------------------------------------------
void ofApp::setup()
{
    run = false;
    grid_size = 100;
    partial_size = 10;
    
    uniform_random = std::make_unique<random_uniform>(0, std::max(agent_size, grid_size));
    
    partial_grid.clear();
    partial_grid.resize(grid_size);
    for (auto& col : partial_grid)
        col.resize(grid_size, 0);
    
    agent_size = 100;
    
    agents.clear();
    agents.resize(agent_size);
    for (size_t i = 0; i < agent_size; ++i)
    {
        agents[i].x = uniform_random->get_next() % agent_size;
        agents[i].y = uniform_random->get_next() % agent_size;
        agents[i].happy = false;
    }
    
    happy_agents.reserve(agents.size());
    unhappy_agents.reserve(agents.size());
    
    for (auto& col : partial_grid)
        for (auto& element : col)
            element = uniform_random->get_next() % 10 == 0;
    
    // Needs to be true for layout
    assert(ofGetWidth() == ofGetHeight());
    
    draw_scalar = float(ofGetWidth()) / float(grid_size);
}

//--------------------------------------------------------------
void ofApp::update(){
    happy_agents.clear();
    unhappy_agents.clear();
    
    for (auto& agent : agents)
    {
        agent.set_happy(partial_grid);
        if (agent.happy)
            happy_agents.push_back(agent);
        else
            unhappy_agents.push_back(agent);
    }
    
    for (auto& agent : unhappy_agents)
    {
        if (happy_agents.size() > 0)
        {
            size_t random_index = uniform_random->get_next() % happy_agents.size();
            set_agent_randomly_in_same_quadrant(happy_agents[random_index],
                                                agent,
                                                partial_size,
                                                (*uniform_random));
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    for (size_t x = 0; x < grid_size; ++x)
    {
        for (size_t y = 0; y < grid_size; ++y)
        {
            const ofColor c = (partial_grid[x][y] == 0) ? ofColor::black : ofColor::gold;
            ofSetColor(c);
            ofDrawRectangle(x * draw_scalar, y * draw_scalar, draw_scalar, draw_scalar);
        }
    }
    
    ofSetColor(ofColor::red);
    const float increment = float(partial_size) / 2.0;
    for (auto& agent : agents)
    {
        ofDrawCircle(agent.x * draw_scalar + increment, agent.y * draw_scalar + increment, increment / 2.0);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    run = !run;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    setup();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

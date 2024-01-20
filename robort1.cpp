#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <new>
#include <string>
#include <stdexcept>
#include <set>

using namespace std;

// Constants
const int MAP_SIZE = 10;
const int POPULATION_SIZE = 200;
const int NUM_GENES = 16;
const int ENERGY_COST_MOVE = 1;
const int ENERGY_GAIN_BATTERY = 5;
const double MUTATION_RATE = 0.05;

// Map class
class Map {
public:
    int width;
    int height;
    int** grid;

    Map(int w, int h);
    ~Map();
    void initialize_map();
};

Map::Map(int w, int h) {
    width = w;
    height = h;
    grid = new int*[width];
    for (int i = 0; i < width; i++) {
        grid[i] = new int[height];
        for (int j = 0; j < height; j++) {
            grid[i][j] = (rand() % 100 < 40) ? ENERGY_GAIN_BATTERY : 0;  // 40% chance for battery
        }
    }
}

Map::~Map() {
    for (int i = 0; i < width; i++) {
        delete[] grid[i];
    }
    delete[] grid;
}

void Map::initialize_map() {
    // Implementation goes here, if needed
}

// Robot class
class Robot {
public:
    int energy;
    map<string, pair<int, int>> movement_rules;
    int fitness_score;

    Robot(int e, map<string, pair<int, int>> mr);
};

Robot::Robot(int e, map<string, pair<int, int>> mr) {
    energy = e;
    movement_rules = mr;
    fitness_score = 0;
}

// Function Prototypes
void initialize_population(vector<Robot*>& population);
int run_robot(Robot* robot, Map* map);
vector<Robot*> select_parents(const vector<Robot*>& population);
Robot* crossover(const Robot* parent1, const Robot* parent2);
vector<Robot*> breed(const vector<Robot*>& parents);
void print_robot(const Robot* robot);

int main() {
    Map* map = new Map(MAP_SIZE, MAP_SIZE);
    vector<Robot*> population;

    // Initialize population and map
    initialize_population(population);
    map->initialize_map();


    // Run simulation for a number of generations
    int num_generations = 50;

    for (int generation = 0; generation < num_generations; generation++) {
        // Run each robot through the map to collect energy and calculate fitness score
        for (Robot* robot : population) {
            int fitness_score = run_robot(robot, map);
            robot->fitness_score = fitness_score;
        }

        // Select parents based on fitness scores
        vector<Robot*> parents = select_parents(population);

        // Breed new generation
        vector<Robot*> children = breed(parents);

        // Replace old population with new generation
        population = parents;
        for (Robot* child : children) {
            population.push_back(child);
        }

        // Print average fitness for the generation
        double total_fitness = 0;
        for (Robot* robot : population) {
            total_fitness += robot->fitness_score;
        }
        double average_fitness = total_fitness / population.size();
        cout << "Generation " << generation + 1 << ", Average Fitness: " << average_fitness << endl;
    }

    // Output the final generation of robots
    for (Robot* robot : population) {
        print_robot(robot);
    }

    // Clean up
    delete map;
    for (Robot* robot : population) {
        delete robot;
    }

    return 0;
}

// Initialize the map with batteries
void initialize_map(Map* map) {
    // Already implemented in the Map constructor
}

// Initialize the population with random robots
void initialize_population(vector<Robot*>& population) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        map<string, pair<int, int>> movement_rules;
        movement_rules["up"] = {rand() % 3 - 1, rand() % 3 - 1};
        movement_rules["down"] = {rand() % 3 - 1, rand() % 3 - 1};
        movement_rules["left"] = {rand() % 3 - 1, rand() % 3 - 1};
        movement_rules["right"] = {rand() % 3 - 1, rand() % 3 - 1};
        Robot* robot = new Robot(5, movement_rules);
        population.push_back(robot);
    }
}

// Run the robot through the map
int run_robot(Robot* robot, Map* map) {
    int x = rand() % map->width;
    int y = rand() % map->height;
    int turns_survived = 0;

    while (robot->energy > 0) {
        // Assuming movement_rules are "up", "down", "left", "right"
        for (const auto& movementRule : robot->movement_rules) {
            const string& direction = movementRule.first;
            const pair<int, int>& movement = movementRule.second;

            int new_x = x + movement.first;
            int new_y = y + movement.second;

            // Check if the new position is within the bounds of the map
            if (new_x >= 0 && new_x < map->width && new_y >= 0 && new_y < map->height) {
                // Update the robot's position
                x = new_x;
                y = new_y;

                // Consume energy for movement
                robot->energy -= ENERGY_COST_MOVE;

                // Check if the new position contains a battery
                if (map->grid[x][y] == ENERGY_GAIN_BATTERY) {
                    // Gain energy from the battery
                    robot->energy += ENERGY_GAIN_BATTERY;
                    // Mark the battery as consumed
                    map->grid[x][y] = 0;
                }
            }
        }

        // Add any additional logic for sensor readings and other actions here

        turns_survived++;
    }

    return turns_survived;
}

// Function to compare robots based on fitness scores
bool compareRobots(const Robot* a, const Robot* b) {
    return a->fitness_score > b->fitness_score;
}

// Select parents based on fitness scores using tournament selection
vector<Robot*> select_parents(const vector<Robot*>& population) {
    vector<Robot*> parents;

    // Perform tournament selection for each parent
    for (int i = 0; i < POPULATION_SIZE / 2; i++) {
        // Randomly choose two robots from the population
        int index1 = rand() % population.size();
        int index2 = rand() % population.size();

        // Ensure the indices are different
        while (index2 == index1) {
            index2 = rand() % population.size();
        }

        // Compare the fitness scores of the two robots and select the one with higher fitness
        Robot* parent1 = max(population[index1], population[index2], compareRobots);

        // Repeat the process to select the second parent
        index1 = rand() % population.size();
        index2 = rand() % population.size();
        while (index2 == index1) {
            index2 = rand() % population.size();
        }
        Robot* parent2 = max(population[index1], population[index2], compareRobots);

        // Add the selected parents to the vector
        parents.push_back(parent1);
        parents.push_back(parent2);
    }

    return parents;
}

// Breed new generation
vector<Robot*> breed(const vector<Robot*>& parents) {
    vector<Robot*> children;

    // Perform crossover for each pair of parents
    for (int i = 0; i < parents.size(); i += 2) {
        // Ensure there are at least two parents remaining
        if (i + 1 < parents.size()) {
            // Perform crossover to create two children
            Robot* child1 = crossover(parents[i], parents[i + 1]);
            Robot* child2 = crossover(parents[i + 1], parents[i]);

            // Add the children to the vector
            children.push_back(child1);
            children.push_back(child2);
        }
    }

    return children;
}

// Print robot information
void print_robot(const Robot* robot) {
    cout << "Fitness Score: " << robot->fitness_score << endl;
    cout << "Movement Rules: ";
    for (auto it = robot->movement_rules.begin(); it != robot->movement_rules.end(); ++it) {
        cout << it->first << ": (" << it->second.first << ", " << it->second.second << ") ";
    }
    cout << endl;
}

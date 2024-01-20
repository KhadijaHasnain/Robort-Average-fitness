// robort7.cpp
//
//
//

#include <iostream>
#include <ctime>

const int GRID_SIZE = 10;
const int INITIAL_ENERGY = 5;
const int POPULATION_SIZE = 200;
const int NUM_GENES = 16;
const int NUM_ACTIONS = 5;
const int MUTATION_RATE = 5; // 5% mutation rate
const int MAX_GENERATIONS = 100;

enum SensorState {
    NO_OBJECT,
    WALL,
    BATTERY,
    DONT_CARE
};

enum RobotAction {
    MOVE_NORTH,
    MOVE_SOUTH,
    MOVE_EAST,
    MOVE_WEST,
    MOVE_RANDOM
};

struct Gene {
    SensorState sensorState;
    RobotAction action;
};

struct Robot {
    Gene genes[NUM_GENES];
    int energy;
    int x; // current x position on the grid
    int y; // current y position on the grid

    Robot() : energy(INITIAL_ENERGY), x(0), y(0) {}
};

Gene getRandomGene() {
    Gene gene;
    gene.sensorState = static_cast<SensorState>(rand() % 4);
    gene.action = static_cast<RobotAction>(rand() % NUM_ACTIONS);
    return gene;
}

Robot getRandomRobot() {
    Robot robot;
    for (int i = 0; i < NUM_GENES; ++i) {
        robot.genes[i] = getRandomGene();
    }
    return robot;
}

void performAction(Robot &robot, RobotAction action) {
    int energyCost = 1; // Default energy cost for movement
    switch (action) {
        case MOVE_NORTH:
            if (robot.y > 0) {
                robot.y--;
            }
            break;
        case MOVE_SOUTH:
            if (robot.y < GRID_SIZE - 1) {
                robot.y++;
            }
            break;
        case MOVE_EAST:
            if (robot.x < GRID_SIZE - 1) {
                robot.x++;
            }
            break;
        case MOVE_WEST:
            if (robot.x > 0) {
                robot.x--;
            }
            break;
        case MOVE_RANDOM:
            // Move randomly in any direction
            performAction(robot, static_cast<RobotAction>(rand() % 4));
            return;
    }

    // Update energy cost
    robot.energy -= energyCost;
}

void simulateTurn(Robot &robot) {
    for (int i = 0; i < NUM_GENES; ++i) {
        // Check if the current sensor state matches the gene's sensor state
        switch (robot.genes[i].sensorState) {
            case NO_OBJECT:
                // No specific check needed for "No object"
                break;
            case WALL:
                // Check if there is a wall in the direction the robot is facing
                // Implement logic to check for walls
                if (true /* check for a wall */) {
                    continue; // Skip to the next gene if there's a wall
                }
                break;
            case BATTERY:
                // Check if there is a battery in the direction the robot is facing
                // Implement logic to check for batteries
                if (true /* check for a battery */) {
                    performAction(robot, robot.genes[i].action);
                    return; // Stop at the first matching gene
                }
                break;
            case DONT_CARE:
                // No specific check needed for "Don't care"
                break;
        }

        // If the sensor state matches, perform the associated action
        performAction(robot, robot.genes[i].action);
        // No break here to check the remaining genes
    }
}

int evaluateFitness(const Robot &robot) {
    // Fitness calculation based on the remaining energy
    return robot.energy;
}

void reproduce(Robot population[POPULATION_SIZE]) {
    // Simple bubble sort for illustration purposes
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        for (int j = 0; j < POPULATION_SIZE - i - 1; ++j) {
            if (evaluateFitness(population[j]) < evaluateFitness(population[j + 1])) {
                // Swap robots based on fitness
                Robot temp = population[j];
                population[j] = population[j + 1];
                population[j + 1] = temp;
            }
        }
    }

    // Resize the population
    for (int i = 0; i < POPULATION_SIZE / 2; ++i) {
        // Select parents
        const Robot &parent1 = population[i];
        const Robot &parent2 = population[i + 1];

        // Create children
        Robot child1;
        Robot child2;

        // Perform gene crossover
        for (int j = 0; j < NUM_GENES; ++j) {
            if (rand() % 2 == 0) {
                // Take gene from parent 1
                child1.genes[j] = parent1.genes[j];
                // Take gene from parent 2
                child2.genes[j] = parent2.genes[j];
            } else {
                // Take gene from parent 2
                child1.genes[j] = parent2.genes[j];
                // Take gene from parent 1
                child2.genes[j] = parent1.genes[j];
            }
        }

        // Apply mutation with a certain probability
        if (rand() % 100 < MUTATION_RATE) {
            // Randomly select a gene to mutate
            int geneToMutate = rand() % NUM_GENES;
            // Mutate the gene (change either sensor state or action)
            child1.genes[geneToMutate].sensorState = static_cast<SensorState>(rand() % 4);
            child1.genes[geneToMutate].action = static_cast<RobotAction>(rand() % NUM_ACTIONS);
        }

        if (rand() % 100 < MUTATION_RATE) {
            int geneToMutate = rand() % NUM_GENES;
            child2.genes[geneToMutate].sensorState = static_cast<SensorState>(rand() % 4);
            child2.genes[geneToMutate].action = static_cast<RobotAction>(rand() % NUM_ACTIONS);
        }

        // Add children to the population
        population[i + POPULATION_SIZE / 2] = child1;
        population[i + POPULATION_SIZE / 2 + 1] = child2;
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    Robot population[POPULATION_SIZE];
    int totalFitness = 40;

    // Simulate each robot in the population
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        while (population[i].energy > 0) {
            simulateTurn(population[i]);
        }
        // Accumulate fitness for average calculation
        totalFitness += evaluateFitness(population[i]);
    }

    // Calculate average fitness for the generation
    int averageFitness = (totalFitness / POPULATION_SIZE ) + MAX_GENERATIONS;

    // Telling What Program is doing 
    std::cout << "Calculate average fitness for the generation of Roborts" << std::endl;

    // Print the average fitness only once per generation
    std::cout << "Average Fitness: " << averageFitness << std::endl;

    // Reproduce to generate the next generation
    reproduce(population);

    return 0;
}

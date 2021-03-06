#include "GA.hpp"

typedef std::mt19937 MyRNG;

GA::GA(const RoomVector& r, const CourseVector& c, const int p, const float m, const float x):
pop(p), pmutate(m), pxover(x)
{
  for (int i = 0;i < pop;i++) {
    uint32_t seed_val = std::chrono::system_clock::now().time_since_epoch().count();
    MyRNG generator;
    generator.seed(seed_val);

    State s(r, c);
    s.init_random_schedule(generator);
    population.push_back(s);
  }
}

void GA::selection(){
  //improvisation from truncating technique
  //calculate mean of fitness, erase fitness bigger than rand(mean, max), repopulate with remaining population
  int sum_fitness = 0;
  float mean_fitness = 0;
  for (int i = 0;i < pop;i++) {
    sum_fitness += population[i].fitness_score();
  }
  mean_fitness = (float) sum_fitness / pop;
  vector<State> newpopulation; //faster than removing each element on old population
  for (int i = 0;i < pop;i++) {
    float random; //random-random (mean * 0,75, max)
    if (population[i].fitness_score() < random) {
      newpopulation.push_back(population[i]);
    }
    //For Debugging Purposes
    /*else {
      printf("Removed specimen %d with selection factor of %.2f\n", i, random);
    }
    */
  }
  int i = 0;
  while (newpopulation.size() < pop) {
    float random; //random-random
    float prob = (float) 1/sqrt(pop);
    if (random < prob) {
        //For Debugging Purposes
        //printf("Chosen specimen %d to repopulate with %.2f probability\n", i, prob);
        newpopulation.push_back(population[i]);
    }
    if (i = newpopulation.size()-1) {
        i = 0;
    }
    else {
        i++;
    }
  }
  population = newpopulation;
}

void GA::xover(){
  for (int i = 0;i < pop-1;i++) {
    for (int j = i+1;j < pop;j++) {
      float random; //random-random
      if (random < pxover) {
        //For Debugging Purposes
        //printf("Crossover between specimen %d and %d from total population %d\n", i, j, pop);
        crossover(population[i], population[j]);
      }
    }
  }
}

void GA::mutation(){
  for (int i = 0;i < pop;i++) {
    float random; //random-random
    if (random < pmutate) {
      //For Debugging Purposes
      //printf("Mutation in specimen %d from total population %d\n", i, pop);
      uint32_t seed_val = std::chrono::system_clock::now().time_since_epoch().count();
      MyRNG generator;
      generator.seed(seed_val);

      population[i] = population[i].mutate(generator);
    }
  }
}

void GA::elitist(){
  //always retaining the best specimen in population
  int curr, best, worst, bidx = 0, widx = 0;
  curr = population[0].fitness_score();
  best = curr;
  worst = curr;
  for (int i = 1;i < pop;i++) {
    curr = population[i].fitness_score();
    if (curr < best) { //the smaller the better
      best = curr;
      bidx = i;
    }
    if (curr > worst) {
      worst = curr;
      widx = i;
    }
  }
  //For Debugging Purposes
  //printf("Alpha %d, new best %d, new worst %d", alpha.fitness_score(), best, worst);
  if (best < alpha.fitness_score()) {
    alpha = population[bidx]; //copy constructor ?
  }
  else {
    population[widx] = alpha;
  }
  omega = worst;
}

void GA::find_alpha_omega(){
  int curr, best, worst, bidx = 0;
  curr = population[0].fitness_score();
  best = curr;
  worst = curr;
  for (int i = 1;i < pop;i++) {
    curr = population[i].fitness_score();
    if (curr < best) { //the smaller the better
      best = curr;
      bidx = i;
    }
    if (curr > worst) {
      worst = curr;
    }
  }
  alpha = population[bidx];
  omega = worst;
}

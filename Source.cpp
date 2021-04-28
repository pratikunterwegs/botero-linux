#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <fstream>
#include <string>

#include "individual.h"
#include <cassert>

int PopSize = 5000;
int gmax = 1000;
int tmax = 5;

//Mutation
const double mrate = 0.001;
const double mmean = 0.0;
const double mshape = 0.05;

//Costs
const double kd = 0.02;
const double ka = 0.01;
const double tau = 0.25;

//Varied Parameters
const double A = 1.0;   //Deterministic scaling constant, default value 
const double B = 0.0;   //stochastic scaling constant, default value

//R between 1 and 100000, P between 0 and 1
std::vector<double> vecR = { 1.0, pow(10.0, 0.5), 10.0, pow(10.0, 1.5)};
std::vector<double> vecP = { 0.0, 0.2, 0.4, 0.6, 0.8, 1.0 };
//std::vector<double> vecR = { 1.0 };
//std::vector<double> vecP = { 1.0 };
std::bernoulli_distribution RandomChance(0.5);

void viability_selection(std::vector<Individual>& pop, double kd, double ka, double tau) {
    double death_rate;
    for (int i = 0; i < static_cast<int>(pop.size()); ++i) {
        //Time to kill!
        death_rate = pop[i].calculate_deathrate(kd, ka, tau);
        if (std::bernoulli_distribution(death_rate)(rng)) {
            pop.erase(pop.begin() + i);
            --i;
        }
    }
}

void reproduction(std::vector<Individual>& OffspringMale, std::vector<Individual>& OffspringFemale, std::vector<Individual>& popMale, std::vector<Individual>& popFemale) {

    std::uniform_int_distribution<int> ReprFemale(0, popFemale.size() - 1);
    std::uniform_int_distribution<int> ReprMale(0, popMale.size() - 1);

    for (int i = 0; i < PopSize; i++) {
        int Mother = ReprFemale(rng);
        int Father = ReprMale(rng);
        Individual Offspring;
        if (RandomChance(rng)) {
            Offspring = popFemale[Mother];
        }
        else {
            Offspring = popMale[Father];
        }
        Offspring.mismatch = 0.0;
        Offspring.n = 0;
        if (RandomChance(rng)) {
            OffspringFemale.push_back(Offspring);
        }
        else {
            OffspringMale.push_back(Offspring);
        }
    }

}

//void adjust_popsize(std::vector<Individual>& tmp_pop, const int targetsize) {
//
//  while (static_cast<int>(tmp_pop.size()) < targetsize) {
//    int duplicate = std::uniform_int_distribution<int>(0, tmp_pop.size() - 1)(rng);
//    tmp_pop.push_back(tmp_pop[duplicate]);
//  }
//
//  while (static_cast<int>(tmp_pop.size()) > targetsize) {
//    int remove = std::uniform_int_distribution<int>(0, tmp_pop.size() - 1)(rng);
//    tmp_pop.erase(tmp_pop.begin() + remove);
//  }
//
//}

void test_deathrate() {
    Individual this_id;
    double this_deathrate = this_id.calculate_deathrate(0.01, 0.02, 0.25);
    assert(this_deathrate > 0.0 && this_deathrate <= 1.0f && "test_deathrate: bad deathrate");

    std::cout << "tested deathrate function\n";
}

int main() {
    std::cout << M_PI << "\n";

    test_deathrate();

    for (int Seed = 0; Seed < 5; Seed++) {
        rng.seed(Seed);
        std::cout << "Seed Nr: " << Seed << std::endl;
        //double R = 10.f;  //Environmental variation
        //double P = 1.f;   //predictability
        ///
        std::uniform_real_distribution<double> env_dist (-1.0, 1.0); // not explicitly stated in botero 2015
        double E;
        double Cue;
        const std::string outfile = "data/AverageValues_Seed_" + std::to_string(Seed) + ".csv";
        //const std::string outfile2 = "IndividualValues_Seed_" + std::to_string(Seed) + ".csv";
        std::ofstream ofs(outfile);
        //std::ofstream ofs2(outfile2);
        ofs << "Seed" << ';' << "R" << ';' << "P" << ';' << "h" << ';' << "I01" << ';' << "I02" << ';' << "b1" << ';' << "b2" << ';' << "s" << ';' << "a" << "\n";
        //ofs2 << "Seed" << ';' << "R" << ';' << "P" << ';' << "h" << ';' << "I01" << ';' << "I02" << ';' << "b1" << ';' << "b2" << ';' << "s" << ';' << "a" << ';' << "sex" << "\n";
        for (int r = 0; r < vecR.size(); ++r) {
            double R = vecR[r];
            std::cout << "R is " << R << std::endl;
            for (int p = 0; p < vecP.size(); ++p) {
                double P = vecP[p];
                std::cout << "P is " << P << std::endl;
                std::vector<Individual> popMale(PopSize / 2); // population size: 5000
                std::vector<Individual> popFemale(PopSize / 2); // population size: 5000
                //Initialization
                E = 0.0;
                if (1.0 - P == 0.0) {
                    Cue = E;
                }
                else {
                    Cue = std::normal_distribution<double>(P * E, ((1.0 - P) / 3.0))(rng);
                }
                for (int i = 0; i < static_cast<int>(popMale.size()); i++) {
                    popMale[i].update_I_g(Cue);
                }
                for (int i = 0; i < static_cast<int>(popFemale.size()); i++) {
                    popFemale[i].update_I_g(Cue);
                }
                for (int g = 0; g < gmax; g++) {
                    ////std::cout << g << ';';
                    //std::cout << "generation" << ';' << g << std::endl;
                    //  for (int countingN = 0; countingN < pop.size(); countingN++) {
                    //      std::cout << "Time: "<<g<<", n of individual nr "<<countingN<<" of the new pop is: " << pop[countingN].n << std::endl;
                    //      std::cout << "Time: " << g << ", mismatch of individual nr " << countingN << " of the new pop is: " << pop[countingN].mismatch << std::endl;
                    //  }
                    for (int t = 0; t < tmax; t++) {
                        //update environment
                          //std::cout << "Loop t-time: n of first individual of new pop is: " << pop[0].n << std::endl;

                        E = A * std::sin((2.0 * static_cast<double>(M_PI) * static_cast<double>(static_cast<double>(g) * tmax + t)) / (static_cast<double>(tmax) * R)) + B * env_dist(rng);
                        //calculate cue
                        if (1.0 - P == 0.0) {
                            Cue = E;
                        }
                        else {
                            Cue = std::normal_distribution<double>(P * E, ((1.0 - P) / 3.0))(rng);
                        }
                        /// Is Cue calculated once for the whole population, or per individual?
                        //individual update
                        for (int i = 0; i < static_cast<int>(popMale.size()); ++i) {
                            popMale[i].update_I_t(Cue);         //Insulation
                            popMale[i].update_mismatch(E);      //phenotypic mismatch

                        }
                        for (int i = 0; i < static_cast<int>(popFemale.size()); ++i) {
                            popFemale[i].update_I_t(Cue);         //Insulation
                            popFemale[i].update_mismatch(E);      //phenotypic mismatch

                        }
                    }

                    // viability selection
                    viability_selection(popMale, kd, ka, tau);
                    viability_selection(popFemale, kd, ka, tau);
                    //Reproduction
                    std::vector<Individual> OffspringMale;
                    std::vector<Individual> OffspringFemale;
                    reproduction(OffspringMale, OffspringFemale, popMale, popFemale);
                    //Mutation & irreversible plasticity
                    for (int i = 0; i < static_cast<int>(OffspringMale.size()); i++) {
                        OffspringMale[i].mutate(mrate, mmean, mshape);
                        OffspringMale[i].update_I_g(Cue);
                    }
                    for (int i = 0; i < static_cast<int>(OffspringFemale.size()); i++) {
                        OffspringFemale[i].mutate(mrate, mmean, mshape);
                        OffspringFemale[i].update_I_g(Cue);
                    }
                    //std::cout << "n of first individual of old pop is: " << pop[0].n << std::endl;
                    std::swap(popFemale, OffspringFemale);
                    std::swap(popMale, OffspringMale);
                    OffspringFemale.clear();
                    OffspringMale.clear();
                }
                //    std::string filetype = ".png";
                //calculate population average
                double sum_h = 0.0;
                double sum_I01 = 0.0;
                double sum_I02 = 0.0;
                double sum_b1 = 0.0;
                double sum_b2 = 0.0;
                double sum_s = 0.0;
                double sum_a = 0.0;
                for (int i = 0; i < static_cast<int>(popMale.size()); ++i) {
                    sum_h = sum_h + popMale[i].h;
                    sum_I01 = sum_I01 + popMale[i].I01;
                    sum_I02 = sum_I02 + popMale[i].I02;
                    sum_b1 = sum_b1 + popMale[i].b1;
                    sum_b2 = sum_b2 + popMale[i].b2;
                    sum_s = sum_s + popMale[i].s;
                    sum_a = sum_a + popMale[i].a;

                }
                for (int i = 0; i < static_cast<int>(popFemale.size()); ++i) {

                    sum_h = sum_h + popFemale[i].h;
                    sum_I01 = sum_I01 + popFemale[i].I01;
                    sum_I02 = sum_I02 + popFemale[i].I02;
                    sum_b1 = sum_b1 + popFemale[i].b1;
                    sum_b2 = sum_b2 + popFemale[i].b2;
                    sum_s = sum_s + popFemale[i].s;
                    sum_a = sum_a + popFemale[i].a;

                }
                double average_h = sum_h / static_cast <double> (PopSize);
                double average_I01 = sum_I01 / static_cast <double> (PopSize);
                double average_I02 = sum_I02 / static_cast <double> (PopSize);
                double average_b1 = sum_b1 / static_cast <double> (PopSize);
                double average_b2 = sum_b2 / static_cast <double> (PopSize);
                double average_s = sum_s / static_cast <double> (PopSize);
                double average_a = sum_a / static_cast <double> (PopSize);


                ofs << Seed << ';' << R << ';' << P << ';' << average_h << ';' << average_I01 << ';' << average_I02 << ';' << average_b1 << ';' << average_b2 << ';' << average_s << ';' << average_a << "\n";

                /*for (int i = 0; i < static_cast<int>(popMale.size()); ++i) {

                    ofs2 << Seed << ';' << R << ';' << P << ';' << popMale[i].h << ';' << popMale[i].I01 << ';' << popMale[i].I02 << ';' << popMale[i].b1 << ';' << popMale[i].b2 << ';' << popMale[i].s << ';' << popMale[i].a << ';' << "male" << "\n";

                }
                for (int i = 0; i < static_cast<int>(popFemale.size()); ++i) {

                    ofs2 << Seed << ';' << R << ';' << P << ';' << popFemale[i].h << ';' << popFemale[i].I01 << ';' << popFemale[i].I02 << ';' << popFemale[i].b1 << ';' << popFemale[i].b2 << ';' << popFemale[i].s << ';' << popFemale[i].a << ';' << "female" << "\n";

                }*/
            }
        }
        ofs.close();
        //ofs2.close();
    }
    return 0;
}









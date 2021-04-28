#ifndef INDIVIDUAL_H_INCLUDED
#define INDIVIDUAL_H_INCLUDED
#include <random>
static std::minstd_rand rng;
struct Individual {
	Individual();
	void update_I_t(const double C);
	void update_I_g(double C);
	inline void update_mismatch(const double E) { mismatch += abs(E - I_realized); }
	double calculate_deathrate(double kd, double ka, double tau);
	void mutate(double mrate, double mmean, double mshape);
	//genes
	double h;
	double I01;
	double I02;
	double b1;
	double b2;
	double s;
	double a;
	//lifetime params
	bool h_coin;
	int n;
	double I_realized;
	double mismatch;
};

#endif
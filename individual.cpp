#include "individual.h"

Individual::Individual() {
    h = 1.0;
    mismatch = 0.0;
    n = 0;
    h_coin = true;
    I_realized = 0.0;
    std::uniform_real_distribution<double>dist1(0.0, 1.0);
    std::uniform_real_distribution<double>dist2(-1.0, 1.0);
    std::uniform_real_distribution<double>dist3(-2.0, 2.0);

    s = dist1(rng);
    if (s > 0.5) {
        a = dist1(rng);
        b1 = dist3(rng);
        b2 = dist3(rng);
    }
    else {
        a = 0.0;
        b1 = 0.0;
        b2 = 0.0;
    }
    I01 = dist2(rng);
    I02 = dist2(rng);
}


void Individual::update_I_t(const double C) {
    if (s > 0.5) {
        if (a > 0.0) {
            if (a >= 1 || std::bernoulli_distribution(a)(rng)) {
                n++;
                if (h_coin) {
                    I_realized = b1 * C + I01;
                }
                else {
                    I_realized = b2 * C + I02;
                }
            }
        }
    }
}


void Individual::update_I_g(double C) {
    if (h >= 1) {
        h_coin = true;
    }
    else {
        h_coin = std::bernoulli_distribution(h)(rng);
    }
    if (s > 0.5) {
        if (h_coin) {
            I_realized = b1 * C + I01;
        }
        else {
            I_realized = b2 * C + I02;
        }
    }
    else {
        if (h_coin) {
            I_realized = I01;
        }
        else {
            I_realized = I02;
        }
    }
}


double Individual::calculate_deathrate(double kd, double ka, double tau) {
    double deathrate;
    if (s > 0.5) {
        deathrate = 1.0 - exp(-tau * mismatch - kd - n * ka);
    }
    else {
        deathrate = 1.0 - exp(-tau * mismatch);
    }
    return deathrate;
}


void Individual::mutate(double mrate, double mmean, double mshape) {
    std::bernoulli_distribution mdist(mrate);
    std::normal_distribution<double> sdist(mmean, mshape);
    if (mdist(rng)) {
        h += sdist(rng);
    }
    if (mdist(rng)) {
        I01 += sdist(rng);
    }
    if (mdist(rng)) {
        I02 += sdist(rng);
    }
    if (mdist(rng)) {
        s += sdist(rng);
    }
    if (mdist(rng)) {
        b1 += sdist(rng);
    }
    if (mdist(rng)) {
        b2 += sdist(rng);
    }
    if (mdist(rng)) {
        a += sdist(rng);
    }
}

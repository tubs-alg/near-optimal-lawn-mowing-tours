// For compiling with Mersenne twister random number generator include MTWISTER
// compiling directive

#ifndef UTIL_H
#define UTIL_H

#include <time.h>
#include <unistd.h>
#include <sys/times.h>

void randomize();

void setSeed(const unsigned int seed);

/* generates an integer i in {0,...,maxValue-1} */
unsigned int intRandom(const unsigned int maxValue);

double doubleRandom(const double maxValue);

double wallClock();

double cpuTime();

#endif /* ifndef UTIL_H */

#pragma once
#include <iostream>
#include <random>
using namespace std;

inline int getRandom(int min, int max) {
  static std::random_device rd;
  static mt19937 gen(rd());

  uniform_int_distribution<> dist(min, max);
  return dist(gen);
}

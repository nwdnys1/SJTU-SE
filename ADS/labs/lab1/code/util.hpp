#ifndef UTIL_HPP
#define UTIL_HPP
#include <algorithm>
#include <cassert>
#include <random>
#include <climits>

#define CANDIDATE_CNT 100000
#define ZIPF_ALPHA 0.5

int zipf(double alpha, int n);

// This is the help function to generate a random number following zipf distribution
int zipf()
{
  return zipf(ZIPF_ALPHA, CANDIDATE_CNT) * (INT_MAX / CANDIDATE_CNT);
}

int zipf(double alpha, int n)
{
  static int first = true;  // Static first time flag
  static double c = 0;      // Normalization constant
  static double *sum_probs; // Pre-calculated sum of probabilities
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<double> dis(0.0, 1.0); // Range from 0 to 1
  double z;                                                    // Uniform random number (0 < z < 1)
  int zipf_value;                                              // Computed exponential value to be returned
  int i;                                                       // Loop counter
  int low, high, mid;                                          // Binary-search bounds

  // Compute normalization constant on first call only
  if (first == true)
  {
    for (i = 1; i <= n; i++)
      c = c + (1.0 / std::pow((double)i, alpha));
    c = 1.0 / c;

    sum_probs = (double *)malloc((n + 1) * sizeof(*sum_probs));
    sum_probs[0] = 0;
    for (i = 1; i <= n; i++)
    {
      sum_probs[i] = sum_probs[i - 1] + c / pow((double)i, alpha);
    }
    first = false;
  }

  // Pull a uniform random number (0 < z < 1)
  do
  {
    z = dis(gen);
  } while ((z == 0) || (z == 1));

  // Map z to the value
  low = 1, high = n, mid;
  do
  {
    mid = floor((low + high) / 2);
    if (sum_probs[mid] >= z && sum_probs[mid - 1] < z)
    {
      zipf_value = mid;
      break;
    }
    else if (sum_probs[mid] >= z)
    {
      high = mid - 1;
    }
    else
    {
      low = mid + 1;
    }
  } while (low <= high);

  // Assert that zipf_value is between 1 and N
  assert((zipf_value >= 1) && (zipf_value <= n));

  return (zipf_value);
}

#endif
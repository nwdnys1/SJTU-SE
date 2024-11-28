#include <iostream>
#include <fstream>
#include <string>
#include "MurmurHash3.h"

class BloomFilter {
public:
    BloomFilter(int m, int k, int size, uint64_t n) : m_(m), k_(k), size_(size), n_(n) {
        hash_ = new uint64_t*[k_];
        for (int i = 0; i < k_; i++) {
            hash_[i] = new uint64_t[size_];
        }
        bloom_filter_ = new bool[m_]{false};
    }

    ~BloomFilter() {
        for (int i = 0; i < k_; i++) {
            delete[] hash_[i];
        }
        delete[] hash_;
        delete[] bloom_filter_;
    }

    void insert(uint64_t element) {
        for (int j = 0; j < k_; j++) {
            MurmurHash3_x64_128(&element, sizeof(element), j, &hash_[j][element % size_]);
            bloom_filter_[hash_[j][element % size_] % m_] = true;
        }
    }

    bool check(uint64_t element) {
        for (int j = 0; j < k_; j++) {
            if (!bloom_filter_[hash_[j][element % size_] % m_]) {
                return false;
            }
        }
        return true;
    }

   void test() {
    int wrong = 0;

    // Insert hash value
    for (uint64_t i = 0; i < size_; i++) {
        for (int j = 0; j < k_; j++) {
            MurmurHash3_x64_128(&(i), sizeof(i), j, &hash_[j][i]);
        }
    }

    // Populate the Bloom filter
    for (uint64_t i = 0; i < n_; i++) {
        insert(i);
    }

    // Test for false positives
    for (uint64_t i = n_; i < size_; i++) {
        if (check(i)) {
            wrong++;
        }
    }

    // Write the result
    std::ofstream outfile("res.txt", std::ios_base::app);
    if (outfile.is_open()) {
        outfile << m_ << "\t" << k_ << "\t" << n_ << "\t" << wrong << "\t" << static_cast<double>(wrong) / static_cast<double>(size_-n_) << std::endl;
        outfile.close();
    } else {
        std::cerr << "Error opening file." << std::endl;
    }
}
private:
    int m_;  // Bloom filter size
    int k_;  // Number of hash functions
    int size_;  // Data size
    uint64_t n_;  // Dataset size
    uint64_t** hash_;  // Hash values
    bool* bloom_filter_;  // Bloom filter array
};

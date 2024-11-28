#include "bloom_filter.h"
#include "MurmurHash3.h"

int main() {
    std::ofstream outfile("res.txt");
    if (outfile.is_open()) {
        outfile << "m\tk\tfalse_positive\tfalse_positive_rate" << std::endl;
        outfile.close();
    } else {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    
    int n=1000000;

    for (int m = 2*n; m <= 5*n; m+=n) {
        for (int k = 1; k <= 5; k++) {
            BloomFilter bf(m, k,3000000,n);
            bf.test();
        }
    }

    return 0;
}

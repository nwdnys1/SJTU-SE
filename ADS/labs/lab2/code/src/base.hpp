#include <iostream>
#include <vector>

namespace HNSWLab
{

    class AlgorithmInterface
    {
    public:
        virtual void insert(const int *item, int label) = 0;

        virtual std::vector<int> query(const int *query, int k) = 0;

        virtual ~AlgorithmInterface()
        {
        }
    };
    class UtilInterface
    {
    public:
        virtual long l2distance(const int *a, const int *b, int vec_dim) = 0;

        virtual int get_random_level() = 0;

        virtual ~UtilInterface()
        {
        }
    };

}

#include <iostream>
#include <sys/timeb.h>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

typedef unsigned short int UInt;

struct Pair
{
    UInt min, max;

    Pair()
    {
    }

    Pair(UInt min, UInt max) :
        min(min),
        max(max)
    {
    }

    bool operator ==(const Pair &other) const
    {
        return min == other.min and max == other.max;
    }

    bool operator<(const Pair &other) const
    {
        if (min!=other.min) {
            return min>other.min;
        }

        return max<other.max;
    }
};

namespace std {
template<>
struct hash<Pair>
{
    size_t operator ()(const Pair &pair) const
    {
        return pair.min ^ (pair.max << 16);
    }
};
}

ostream& operator <<(ostream &out, const Pair &pair)
{
    out << "(" << pair.min << ", " << pair.max << ")";

    return out;
}

ostream& operator <<(ostream &out, const unordered_set<Pair> &pairs)
{
    out << "{";

    for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
        if (i != pairs.cbegin()) {
            out << ", ";
        }

        out << *i;
    }

    out << "}";

    return out;
}

ostream& operator <<(ostream &out, const set<Pair> &pairs)
{
    out << "{";

    for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
        if (i != pairs.cbegin()) {
            out << ", ";
        }

        out << *i;
    }

    out << "}";

    return out;
}

ostream& operator <<(ostream &out, const unordered_map<Pair, unordered_set<Pair>> &sets)
{
    out << "{";

    for (auto i = sets.cbegin(); i != sets.cend(); ++i) {
        if (i != sets.cbegin()) {
            out << ", ";
        }

        out << i->first << " : " << i->second;
    }

    out << "}";

    return out;
}

class ProductSum
{
    mutable unordered_map<Pair, unordered_set<Pair>> k1cache;
    mutable unordered_map<Pair, unordered_set<Pair>> k2cache;
    mutable unordered_map<Pair, unordered_set<Pair>> k3cache;
    mutable unordered_map<Pair, unordered_set<Pair>> k4cache;
    static const unordered_map<Pair, unordered_set<Pair>> emptymap;
    static const unordered_set<Pair> emptyset;
    const Pair minmax;

public:
    ProductSum(Pair minmax) :
        minmax(minmax)
    {
    }

    const unordered_set<Pair>& k1(const Pair &pair) const
    {
        auto j = k1cache.find(pair);

        if (j == k1cache.end()) {
            Pair p;
            unsigned int prod = (unsigned int)pair.min*pair.max;

            k1cache.insert(std::pair<Pair, unordered_set<Pair>>(pair, emptyset));
            j = k1cache.find(pair);
            p.min = (minmax.max+prod-1)/minmax.max;

            if (p.min < minmax.min) {
                p.min = minmax.min;
            }

            while ((unsigned int)p.min*p.min <= prod && prod <= (unsigned int)p.min*minmax.max) {
                p.max = prod/p.min;

                if ((unsigned int)p.min*p.max == prod) {
                    (*j).second.insert(p);
                }

                ++p.min;
            }
        }

        return (*j).second;
    }

    const unordered_set<Pair>& k2(const Pair &pair) const
    {
        auto j = k2cache.find(pair);

        if (j == k2cache.end()) {
            Pair p;
            unsigned int sum = (unsigned int)pair.min+pair.max;

            k2cache.insert(std::pair<Pair, unordered_set<Pair>>(pair, emptyset));
            j = k2cache.find(pair);
            p.min = minmax.min;

            while ((unsigned int)p.min+p.min <= sum && sum <= (unsigned int)p.min+minmax.max) {
                p.max = sum-p.min;
                k2cache[pair].insert(p);
                ++p.min;
            }
        }

        return (*j).second;
    }

    const unordered_set<Pair>& k3(const Pair &pair) const
    {
        auto j = k3cache.find(pair);

        if (j == k3cache.end()) {
            k3cache.insert(std::pair<Pair, unordered_set<Pair>>(pair, emptyset));
            j = k3cache.find(pair);

            auto &s = k1(pair);

            for (auto k = s.cbegin(); k != s.cend(); ++k) {
                if (c2(*k) && c3(*k)) {
                    (*j).second.insert(*k);
                }
            }
        }

        return (*j).second;
    }

    const unordered_set<Pair>& k4(const Pair &pair) const
    {
        auto j = k4cache.find(pair);

        if (j == k4cache.end()) {
            k4cache.insert(std::pair<Pair, unordered_set<Pair>>(pair, emptyset));
            j = k4cache.find(pair);

            auto s = k2(pair);

            for (auto k = s.cbegin(); k != s.cend(); ++k) {
                if (c4(*k)) {
                    (*j).second.insert(*k);
                }
            }
        }

        return (*j).second;
    }

    bool c1(const Pair &pair) const
    {
        return k1(pair).size()>1;
    }

    bool c2(const Pair &pair) const
    {
        return k2(pair).size()>1;
    }

    bool c3(const Pair &pair) const
    {
        auto s = k2(pair);

        for (auto i = s.cbegin(); i != s.cend(); ++i) {
            if (!c1(*i)) {
                return false;
            }
        }

        return true;
    }

    bool c4(const Pair &pair) const
    {
        return k3(pair).size()==1;
    }

    bool c5(const Pair &pair) const
    {
        return k4(pair).size() == 1;
    }

    set<Pair> solve() const
    {
        set<Pair> result;

        for (int a = minmax.min; a<=minmax.max; ++a) {
            for (int b = a; b<=minmax.max; ++b) {
                Pair p(a, b);

                if (c1(p) && c2(p) && c3(p) && c4(p) && c5(p)) {
                    result.insert(p);
                }
            }
        }

        return result;
    }
};

const unordered_map<Pair, unordered_set<Pair>> ProductSum::emptymap;
const unordered_set<Pair> ProductSum::emptyset;

int main(int argc, const char* argv[])
{
    timeb t0;

    ftime(&t0);

    Pair minmax(2, 200);
    ProductSum p(minmax);

    cout << p.solve() << endl;
    timeb t1;
    ftime(&t1);

    double dt = (t1.time - t0.time) + (t1.millitm - t0.millitm)*0.001;

    return 0;
}

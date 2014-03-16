#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

#include <cassert>

typedef std::pair<int, int> Pii;
typedef std::priority_queue< Pii, std::vector<Pii>, std::greater<Pii> > Queue;


struct Proc
{
    std::string id;
    int ti;
    int tw;
    int tc;
    std::vector<Pii> io;
};


namespace std
{
    std::istream &operator >>(std::istream &in, Pii &p)
    {
        return in >> p.first >> p.second;
    }
}


std::istream &operator >>(std::istream &in, Proc &proc)
{
    std::string line;
    do {
        std::getline(in, line);
        if (!in) return in;
    } while (line.empty());

    std::istringstream stream(line);
    stream >> proc.id;
    stream >> proc.ti;
    stream >> proc.tw;
    proc.io.clear();
    proc.io.insert(proc.io.begin(),
                   std::istream_iterator<Pii>(stream),
                   (std::istream_iterator<Pii>()));
    std::reverse(proc.io.begin(), proc.io.end());

    return in;
}


Pii popNextProc(int t, Queue &queue,
                std::vector<Proc> const &procs, int const c)
{
    assert(!queue.empty());

    std::vector<Pii> results;
    while (!queue.empty() && queue.top().first <= t) {
        results.push_back(queue.top());
        queue.pop();
    }

    if (results.empty()) {
        Pii result(queue.top());
        queue.pop();
        return result;
    }

    size_t resultIndex = 0;
    for (size_t i = 0; i < results.size(); ++i) {
        Proc const &proc = procs[results[i].second];
        bool better = proc.tw <= proc.tc + c
            || (!proc.io.empty() && proc.io.back().first <= proc.tc + c);
        if (better)
            resultIndex = i;
    }

    for (size_t i = 0; i < results.size(); ++i) {
        if (i != resultIndex)
            queue.push(results[i]);
    }

    return results[resultIndex];
}


int main()
{
    int c;
    std::cin >> c;
    std::vector<Proc> procs(std::istream_iterator<Proc>(std::cin),
                            (std::istream_iterator<Proc>()));

    Queue queue;
    for (size_t i = 0; i < procs.size(); ++i) {
        procs[i].tc = 0;
        queue.push(Pii(procs[i].ti, i));
    }

    int t = 0;
    while (!queue.empty()) {
        Pii procInfo(popNextProc(t, queue, procs, c));
        int ti = procInfo.first;
        Proc &proc = procs[procInfo.second];

        if (ti > t)
            std::cout << t << " IDLE" << '\n';

        t = std::max(t, ti);
        std::cout << t << ' ' << proc.id << '\n';

        if (proc.tw - proc.tc <= c) {
            t += proc.tw - proc.tc;
        } else if (!proc.io.empty()
                   && proc.io.back().first - proc.tc <= c) {
            t += proc.io.back().first - proc.tc;
            proc.tc = proc.io.back().first + proc.io.back().second;
            queue.push(Pii(t + proc.io.back().second, procInfo.second));
            proc.io.pop_back();
        } else {
            t += c;
            proc.tc += c;
            queue.push(Pii(t, procInfo.second));
        }
    }

    return 0;
}

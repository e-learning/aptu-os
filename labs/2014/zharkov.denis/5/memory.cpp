#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <map>

using namespace std;

typedef unsigned long long ll;

struct desc_record {
    ll base, limit;
    int dpl;
};

ll bits(ll n, size_t from, size_t to) {
    size_t sz = to - from + 1;
    return (n >> from) & (((1ULL << sz) - 1ULL));
}

vector<desc_record> read_desc_table() {
    vector<desc_record> result;
    int n;
    cin >> dec >> n;
    for (size_t i = 0; i < n; ++i) {
        ll record;
        cin >> hex >> record;
        
        ll base = bits(record, 16, 39) | (bits(record, 32+24, 32+31) << 24);
        ll limit = bits(record, 0, 15) | (bits(record, 32+16, 32+19) << 16);

        bool gran = bits(record, 32+23, 32+23) > 0;
        
        if (gran) {
            limit <<= 12;
        }

        int dpl = bits(record, 45, 46);
        
        result.push_back({base, limit, dpl});
    }

    return result;
}

vector<ll> read_page_table() {
    vector<ll> result;
    int n;
    cin >> dec >> n;
    for (size_t i = 0; i< n; ++i) {
        ll next;
        cin >> hex >> next;
        result.push_back(next);
    }

    return result;
}

void assert_that(bool v) {
    if (!v) {
        throw exception();
    }
}

void solve() {
    ll ss, offset;
    cin >> hex >> offset >> ss;

    auto gdt = read_desc_table();
    auto ldt = read_desc_table();
    
    int rpl = bits(ss, 0, 1);
    int is_ldt = bits(ss, 2, 2);
    auto & dt = is_ldt ? ldt : gdt;
    
    size_t idx = bits(ss, 3, 15);
    assert_that(is_ldt || idx > 0);

    auto & descr = dt.at(idx);
    
    assert_that(rpl >= descr.dpl);
    assert_that(offset < descr.limit);

    ll linear_address = descr.base + offset;

    auto pd = read_page_table();
    auto pt = read_page_table();

    size_t pde_idx = bits(linear_address, 22, 31);
    assert_that(pd.at(pde_idx) & 1);
    
    size_t pte_idx = bits(linear_address, 12, 21);
    auto pte = pt.at(pte_idx);

    assert_that(pte & 1);
    
    ll page_base = bits(pte, 12, 31) << 12;
    ll page_offset = bits(linear_address, 0, 11);
    
    ll result = page_base + page_offset;

    assert_that(result < (1ULL << 32));
    
    cout<< hex << result;
}

int main() {
    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);

    try {
        solve();
    } catch (exception & e) {
        cout<< "INVALID";
        return 0;
    }

    return 0;
}

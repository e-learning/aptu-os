#include <iostream>
#include <vector>
using namespace std;

typedef uint64_t i64;

typedef vector<i64> Table;

bool granularity = false;

i64 read_hex (){
    uint64_t h;
    cin>>hex>>h;
    return h;
}

Table read_table (){
    size_t size;
    cin>>dec>>size;

    Table res;
    for(size_t i=0;i<size;++i){
        res.push_back(read_hex());
    }
    return res;
}

void assert (bool cond){
    if (!cond){
        cout<<"INVALID"<<std::endl;
        exit(0);
    }
}

i64 getBits(i64 num,  int least ,int most) {
    num >>= least;
    num &= (1LL << (most - least + 1)) - 1;
    return num;
}

i64 log_to_lin (i64 log_addr,i64 selector,Table gdt,Table ldt)
{
    int rpl = selector & 3;
    assert ((selector>>2)!=0); // if gdt then index must be gtreater then 0

    Table dt = ((selector & 4) == 0) ? gdt : ldt;

    size_t index =static_cast<size_t> (selector>>3);
    assert(index<dt.size());

    i64 seg_desriptor = dt[index];

    bool p_flag = ((seg_desriptor>>(32+15)&1)!=0);//check presentation flag
    assert(p_flag);

    granularity = (seg_desriptor>>(32+23)&1) != 0;
    i64 seg_limit = getBits(seg_desriptor,0,15);
    seg_limit |=getBits(seg_desriptor,32+16,32+19)<<16;
    if(granularity)    
        seg_limit*=0x1000;




    assert (log_addr<seg_limit);

    int dpl = getBits(seg_desriptor,32+13,32+14);

    assert (rpl>=dpl);


    i64 base = getBits(seg_desriptor,16,16+23);
    base |= getBits(seg_desriptor,32+24,63)<<24;

    return base+log_addr;
}

i64 lin_to_phis (i64  lin_addr,Table pd,Table pt)
{
    i64 offset = getBits(lin_addr,0,11);
    size_t t_index =static_cast<size_t>(getBits(lin_addr,12,21));
    size_t d_index =static_cast<size_t>(getBits(lin_addr,22,31));

    assert(t_index<pt.size());//must be in table
    assert(d_index<pd.size());//must be in table
    assert((pt[t_index]&1) !=0);//check that dir is present
    assert((pd[d_index]&1) !=0);//check that page is present

    i64 base_page_addr = getBits(pt[t_index],12,31);
    if (granularity)
            base_page_addr<<=12;

    return base_page_addr + offset;
}




int main()
{
    i64 l_addr = read_hex();

    i64 selector = read_hex();


    Table gdt = read_table();


    Table ldt = read_table();

    Table pd = read_table();

    Table pt = read_table();


    i64 lin_addr = log_to_lin(l_addr,selector,gdt,ldt);
    i64 p_addr = lin_to_phis(lin_addr,pd,pt);

    cout<<"\n"<<hex<<p_addr<<std::endl;

    return 0;
}


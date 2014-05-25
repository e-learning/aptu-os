#include <iostream>
#include <string>
#include <vector>

using std::string;

using std::cin;
using std::cout;
using std::endl;

typedef std::vector<size_t> record;
typedef std::vector<record> table;


size_t h2d(char c){
	size_t r =  (c > '9')? (c &~ 0x20) - 'A' + 10: (c - '0');
	return r;
} 

size_t inphexshort(){
	size_t res = 0;
	string buf;
	cin >> buf;
	for (size_t i = 0 ; i< buf.size(); i++){
		res = (res<<4) + h2d(buf[i]);
	}
	return res;
}

record inprecord(){
	record res(4,0);
	string buf;
	cin >> buf;
	for (size_t i = 0 ; i< buf.size(); i++){
		res[i/4] = (res[i/4]<<4) + h2d(buf[i]);
	}
	//cout << buf << endl;
	//cout << std::hex << res[3] << ' '<< res[2] << ' ' << res[1]<< ' '<<res[0]<<endl;
	return res;	
}

namespace sd{
	enum sd_fields{index, rpl, ti};
}
namespace dt{
	enum dt_fields{limit, base, type, S, dpl, P, avl, L, DB, G};
}
namespace pd{
	enum pd_fields{fst, rw, us, pwt, pcd, a, d, pat, g, addr};
}
namespace la{ //linear addr
	enum la_fields{dir, table, offset};
}

record inpsegrecord(){
	record res = inprecord();
	record seg(10,0);
	seg[dt::limit] = res[3] + (res[0]&15 << 16);//seg limit  0:19 
    seg[dt::base] = res[2] + (res[1]&255 << 16) + ((res[0]>> 8)<<24);//base addr  
	seg[dt::type] = (res[1]>>8)&15; // type 0:4
	seg[dt::S] = (res[1]>>12)&1; // S descriptor type (0 - system, 1 - code|data)
	seg[dt::dpl] = (res[1]>>13)&3; //dpl privelege level 
	seg[dt::P] = (res[1]>>15);  // P - segment present
	seg[dt::avl] = (res[0]>>4)&1; // avl - available for use by sys. s/w
	seg[dt::L] = (res[0]>>5)&1; // l - 64-bit code segment
	seg[dt::DB] = (res[0]>>6)&1; // d/b - def. operation size (0 - 16-bit, 1- 32-bit)
	seg[dt::G] = (res[0]>>7)&1; // g - granularity
	return seg;
	//todo check dpl
	//todo check segment limit vs offset
}

record inppagerecord(){
	record res = inprecord();
	record pag(10,0);
	pag[pd::fst] = res[3]&1;
	pag[pd::rw] =  (res[3]>>1)&1;
	pag[pd::us] =  (res[3]>>2)&1;
	pag[pd::pwt] = (res[3]>>3)&1;
	pag[pd::pcd] = (res[3]>>4)&1;
	pag[pd::a] =   (res[3]>>5)&1;
	pag[pd::d] =   (res[3]>>6)&1;
	pag[pd::pat] = (res[3]>>7)&1;
	pag[pd::g] =   (res[3]>>8)&1;
	pag[pd::addr] =((res[3]>>12)&15) + (res[2]<<4);


	return pag;
}

table inptable( record(* t)(void) ){
	int size = 0;
	cin >> size;
	table res(size);
	for (int i = 0 ; i != size; ++i)
		res[i] = t();

	return res;
}

void bad(){
	cout << "INVALID" << endl;
	exit(1);
}

int main(){
	//input data
	size_t logad = inphexshort();
	
	size_t sel = inphexshort();
	record sel_r(3);
	sel_r[sd::rpl] = sel & 3; //priv level
	sel_r[sd::ti] = (sel & 4)>>2; // TI flag 
	sel_r[sd::index] = sel  >> 3; //index
	
	table GDT = inptable(inpsegrecord);
	table LDT = inptable(inpsegrecord);
	table PTC = inptable(inppagerecord);
	table PT =  inptable(inppagerecord);
	
	//checks
	
	//null segment selector
	if (!sel_r[1] && ! sel_r[2] ){
		bad();
	}
	
	
	//linear addr
	record seg_dsc;
	if (sel_r[1]) {
		if (GDT.size() > sel_r[sd::index]){
			seg_dsc = GDT[sel_r[sd::index]];
		}else{
			bad();
		}

	} else{
		if (LDT.size() > sel_r[sd::index]){
			seg_dsc = LDT[sel_r[sd::index]];
		}else{
			bad();
		}
	}

	//checks for seg_dsc
	//todo ... 
	//limit check;
	size_t max_size = seg_dsc[dt::limit];
	if (seg_dsc[dt::G]) max_size  <<= 12;
	if (seg_dsc[dt::S] && seg_dsc[dt::type]>>3 == 0 && (seg_dsc[dt::type] & 4)){
	    if (logad <= max_size) bad();
		if (seg_dsc[dt::DB] == 0 && logad > 0xFFFF) bad();
	}else{
		if (max_size < logad) bad();		
	}

	//pl check
	
	if (seg_dsc[dt::S] && (seg_dsc[dt::type]&8)==0 && seg_dsc[dt::dpl] > sel_r[sd::rpl]) bad(); // data seg
	if (seg_dsc[dt::S] && (seg_dsc[dt::type]&8)) { //code seg
		if (seg_dsc[dt::type]&4 && seg_dsc[dt::dpl] < sel_r[sd::rpl] ) bad(); //conform 
		if ((seg_dsc[dt::type]&4)==0 && seg_dsc[dt::dpl] != sel_r[sd::rpl] ) bad(); //nonconform 
	}
	if (!seg_dsc[dt::S] && seg_dsc[dt::dpl] > sel_r[sd::rpl]) bad(); //system segment 
	size_t linad = logad + seg_dsc[dt::base];
	record lin_r (3,0);
	lin_r[la::dir] = linad >> 22;
	lin_r[la::table] = (linad >>12 )&1023;
	lin_r[la::offset] = linad & 4095; // 2^13-1

	//checks for table directory enry

	if(PTC.size() <= lin_r[la::dir] ) bad();
	if(PTC[lin_r[la::dir]][pd::fst] == 0) bad(); 
	if (PTC[lin_r[la::dir]][pd::pat] == 1){
		// big page
		size_t phys_addr = (PTC[lin_r[la::dir]][pd::addr]&0xFFE0)+ (linad&(~0xFFE0));
		cout << std::hex << phys_addr << endl;
		return 0;
	}

	//checks for page entry
	if (PT.size() <= lin_r[la::table] )	bad();
	if (PT[lin_r[la::table]][pd::fst] == 0) bad();

	size_t phys_addr = (PT[lin_r[la::table]][pd::addr] << 12) + lin_r[la::offset];
	cout << std::hex << phys_addr << endl;
	
	return 0;
}











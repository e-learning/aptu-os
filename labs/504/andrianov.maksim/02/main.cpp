#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>


#define METKA 0xFFFFFF

struct Resault{
public:
    std::string  ID;
    unsigned int Time;
    Resault(){
        ID  = "0";
        Time  = 0;
    }
    Resault& operator = (const Resault& other)
    {
        if (this != &other)
        {
            this->ID = other.ID;
            this->Time = other.Time;
        }
        return *this;
    }
    void static look_at_me(Resault* proc, unsigned int count_proc){
        if(count_proc)
            for(unsigned int i = 0; i < count_proc; i++){
                std::cout << proc[i].ID <<"  "<<
                proc [i].Time<< std::endl;
        }
        else{
            std::cout << proc[0].ID <<"  "<<
            proc[0].Time<<std::endl;
        }
    }
};


struct Process{
public:
    std::string  ID;
    unsigned int Ti;
    unsigned int Tw;
    unsigned int IOT;
    unsigned int IOL;
    Process(){
        ID  = "0";
        Ti  = 0;
        Tw  = 0;
        IOT = 0;
        IOL = 0;
    }
    Process& operator = (const Process& other)
    {
        if (this != &other)
        {
            this->ID = other.ID;
            this->IOL = other.IOL;
            this->IOT = other.IOT;
            this->Ti = other.Ti;
            this->Tw = other.Tw;
        }
        return *this;
    }

    int static min_search_iol(Process* proc, unsigned int count_proc){
        unsigned int min = proc->IOL;
        unsigned int counter = 0;
        for(unsigned int i = 0; i < count_proc; i++){
            if(min > (proc + i)->IOL){
                counter = i;
            }
        }
        return counter;
    }


    int static min_search_iot(Process* proc, unsigned int count_proc){
        unsigned int min = proc->IOT;
        unsigned int counter = 0;
        for(unsigned int i = 0; i < count_proc; i++){
            if(min > (proc + i)->IOT){
                counter = i;
            }
        }
        return counter;
    }

    int static min_search_ti(Process* proc, unsigned int count_proc){
        unsigned int min = proc->Ti;
        unsigned int counter = 0;
        for(unsigned int i = 0; i < count_proc; i++){
            if(min > (proc + i)->Ti){
                counter = i;
                //min = (proc + i)->Ti;
            }
        }
        return counter;
    }
    void static look_at_me(Process* proc, unsigned int count_proc){
        if(count_proc)
            for(unsigned int i = 0; i < count_proc; i++){
                std::cout << (proc + i)->ID <<"  "<<
                (proc + i)->Ti<<"  "<<
                (proc + i)->Tw<<"  "<<
                (proc + i)->IOT<<"  "<<
                (proc + i)->IOL << std::endl;
        }
        else{
            std::cout << proc->ID <<"  "<<
            proc->Ti<<"  "<<
            proc->Tw<<"  "<<
            proc->IOT<<"  "<<
            proc->IOL << std::endl;
        }
    }
};

class Parser{
private:
    std::string input_line;
    Process *proc;
    Process *to_struct(std::string str_in);

public:
    unsigned int  time_slice;
    unsigned int number_of_proc;
    Parser();
    ~Parser();
    void get_string();
    Process *array_proc;
};

class Whoisnumberone{
private:
    unsigned int num_proc;
    Process* array_proc;
    unsigned int TIME_SLICE;
public:
    unsigned int N_res;
    Resault* res_proc;
    Whoisnumberone(unsigned int number_proc, Process *proc,unsigned int time);
    void sort_proc();
    ~Whoisnumberone();
};


int main()
{

    Parser* p = new Parser();
    p->get_string();
    Whoisnumberone *who_proc = new Whoisnumberone(p->number_of_proc, p->array_proc, p->time_slice);
    //Process::look_at_me(who_proc->array_proc, p->number_of_proc);
    who_proc->sort_proc();
    Resault::look_at_me(who_proc->res_proc, who_proc->N_res);

    return 0;
}



Parser::Parser(){
    Parser::input_line = "0";
    number_of_proc = 0;
    time_slice = 0;
    proc = NULL;
    array_proc = new Process[256]();
}

Parser::~Parser(){
    delete[] array_proc;
}

void Parser::get_string(){
    std::cout << "Please complete for input on a blank line, press X, and then of Enter." << std::endl;
    for (std::string input_line; std::getline(std::cin, input_line);){
        if(input_line == "X"){
            number_of_proc--;
            break;
        }
        if(!number_of_proc)
            time_slice = atoi(input_line.c_str());
        else{
            array_proc[number_of_proc-1] = *to_struct(input_line);
            if(array_proc[number_of_proc-1].Tw < array_proc[number_of_proc-1].IOT + array_proc[number_of_proc-1].IOL){
                std::cerr << "Error: Tw < IOT + IOL" << std::endl;
                exit(0);
            }

        }
        number_of_proc++;
    }

}

Process* Parser::to_struct(std::string str_in){
    Process* proc = new Process();
    unsigned int cnt = 0;
    unsigned int buff = 0;
    unsigned int len = str_in.length();
    for ( unsigned int i = 0; i < len ; i++){
        if(cnt == 3){
            proc->IOT = atoi(str_in.substr(buff+1,i).c_str());
            proc->IOL = atoi(str_in.substr(i + 1,len).c_str());
        }
        else if(cnt == 2)
            proc->Tw = atoi(str_in.substr(buff+1,i).c_str());
        else if(cnt == 1){
            proc->ID = str_in.substr(0,buff);
            proc->Ti = atoi(str_in.substr(buff+1,i).c_str());
        }
        if(str_in[i] == ' '){
            cnt++;
            buff = (unsigned int)i;
        }
    }
    return proc;
}


Whoisnumberone::Whoisnumberone(unsigned int number_proc, Process *proc, unsigned int time){
    array_proc = new Process[number_proc]();
    array_proc = proc;
    num_proc = number_proc;
    TIME_SLICE = time;
    res_proc = new Resault[1024];
    N_res = 0;

}

Whoisnumberone::~Whoisnumberone(){
    delete[] array_proc;
    delete[] res_proc;
}

void Whoisnumberone::sort_proc(){
    unsigned int COUNTER = 0;
    unsigned int TIME = 0;
    bool FLAG = false;
    bool flag = false;
    bool FLAG_IDLE = false;
    unsigned int cnt = 0,t = 0;
    unsigned int* start_proc = new unsigned int[num_proc];
    unsigned int* buff_IOT = new unsigned int[num_proc];
    unsigned int* buf_IOL  = new unsigned int[num_proc];
    unsigned int counter_sp = 0;
    unsigned int min_iter = array_proc[0].IOL + array_proc[0].IOT + array_proc[0].Ti;
    for(unsigned int i = 0 ; i < num_proc ; i++){
            if(min_iter > array_proc[i].IOL + array_proc[i].IOT + array_proc[i].Ti)
                t = i;
        }
    while(1){
    //for(int cnt = 0; cnt < 10; cnt ++){
        std::vector<unsigned int> buf_proc;

        unsigned int count_min_ti = 0;
        unsigned int num_min_ti = Process::min_search_ti(array_proc,num_proc);

        if (FLAG_IDLE == false && (array_proc+num_min_ti)->Ti > 0){\
                res_proc[COUNTER].ID = "IDLE";
                res_proc[COUNTER].Time = 0;
                //std::cout<<"HELO";
                if((array_proc+num_min_ti)->Ti % TIME_SLICE == 0){
                    TIME += (array_proc+num_min_ti)->Ti;
                }
                else
                    TIME = (unsigned int)(((array_proc+num_min_ti)->Ti) / TIME_SLICE) + TIME_SLICE;
                COUNTER++;
                FLAG_IDLE = true;
        }
        else{

            FLAG_IDLE = true;
            if(!((array_proc+num_min_ti)->Ti > TIME)){

                FLAG = true;
                for (unsigned int i = 0; i < num_proc; i++){
                    if((array_proc+i)->Ti == (array_proc+num_min_ti)->Ti)
                        count_min_ti++;
                }
                if(count_min_ti>1){
                    for(unsigned int i = 0; i < num_proc; i++){
                        if((array_proc+i)->Ti == (array_proc+num_min_ti)->Ti){
                            buf_proc.push_back(i);
                            cnt++;
                        }
                    }
                    cnt--;


                    unsigned int count_count_min_ti = count_min_ti;
                    for(unsigned int i = 0; i < count_min_ti; i++){
                        unsigned int min_ti_wiht_min_iol = METKA;
                        unsigned int num_min_ti_wiht_min_iol = 0;

                        for(unsigned int j = 0; j < count_count_min_ti; j++){
                            if(min_ti_wiht_min_iol > array_proc[buf_proc[j]].IOT){
                                min_ti_wiht_min_iol = array_proc[buf_proc[j]].IOT;
                                num_min_ti_wiht_min_iol = j;
                            }
                        }


                        res_proc[COUNTER].ID = array_proc[buf_proc[num_min_ti_wiht_min_iol]].ID;
                        res_proc[COUNTER].Time = TIME;

                        buff_IOT[counter_sp] = array_proc[buf_proc[num_min_ti_wiht_min_iol]].IOT;
                        start_proc[counter_sp] = res_proc[COUNTER].Time;
                        buf_IOL[counter_sp] = array_proc[buf_proc[num_min_ti_wiht_min_iol]].IOL;
                        counter_sp++;

                        TIME += array_proc[buf_proc[num_min_ti_wiht_min_iol]].IOT;
                        array_proc[buf_proc[num_min_ti_wiht_min_iol]].Tw = array_proc[buf_proc[num_min_ti_wiht_min_iol]].Tw -
                                array_proc[buf_proc[num_min_ti_wiht_min_iol]].IOT - array_proc[buf_proc[num_min_ti_wiht_min_iol]].IOL;
                        array_proc[buf_proc[num_min_ti_wiht_min_iol]].IOT = METKA;
                        COUNTER++;
                    }
                    for(unsigned int counter = 0; counter < count_min_ti; counter++){
                        array_proc[buf_proc[counter]].Ti = METKA;
                    }
                }

                else{
                    res_proc[COUNTER].ID = array_proc[num_min_ti].ID;
                    res_proc[COUNTER].Time = TIME;

                    buff_IOT[counter_sp] = array_proc[num_min_ti].IOT;
                    start_proc[counter_sp] = res_proc[COUNTER].Time;
                    buf_IOL[counter_sp] = array_proc[num_min_ti].IOL;
                    counter_sp++;

                    TIME += array_proc[num_min_ti].IOT;
                    array_proc[num_min_ti].Tw = array_proc[num_min_ti].Tw -
                            array_proc[num_min_ti].IOT - array_proc[num_min_ti].IOL;
                    array_proc[num_min_ti].IOT = METKA;
                    array_proc[num_min_ti].Ti = METKA;
                    COUNTER++;
                }

                unsigned int* arr_min = new unsigned int[num_proc];
                for(unsigned int f = 0; f < num_proc; f++){
                    arr_min[f] = buf_IOL[f] + buff_IOT[f] + start_proc[f] - 1;
                }
                std::sort(arr_min,arr_min+num_proc);


                unsigned int num_next_min = Process::min_search_ti(array_proc,num_proc);
                if(TIME <= arr_min[0]){
                    if(array_proc[num_next_min].Ti > TIME && cnt < num_proc - 1){
                        res_proc[COUNTER].ID = "IDLE";
                        res_proc[COUNTER].Time = TIME;
                        TIME = arr_min[0];
                        COUNTER++;

                    }
                }
                unsigned ti_end = 0;
                for (unsigned int g = 0; g < num_proc ; g++){
                    if(array_proc[g].Ti != METKA)
                        ti_end++;
                }


                if(!ti_end){
                    unsigned int g = 0;
                    unsigned int round = 0;
                        while(array_proc[g].Tw != 0){
                            if(TIME >= arr_min[g]){
                                if(!flag)
                                    g = t;
                                if(array_proc[g].Tw % TIME_SLICE == 0){
                                    while(array_proc[g].Tw != 0){
                                        res_proc[COUNTER].Time = TIME;
                                        res_proc[COUNTER].ID = array_proc[g].ID;
                                        array_proc[g].Tw -=TIME_SLICE;
                                        TIME += TIME_SLICE;
                                        COUNTER++;
                                    }
                                }
                                else{
                                    while(array_proc[g].Tw >= TIME_SLICE){
                                        array_proc[g].Tw -= TIME_SLICE;
                                        res_proc[COUNTER].Time = TIME;
                                        res_proc[COUNTER].ID = array_proc[g].ID;
                                        TIME += TIME_SLICE;
                                        COUNTER++;
                                    }
                                    res_proc[COUNTER].Time = TIME ;
                                    res_proc[COUNTER].ID = array_proc[g].ID;
                                    TIME += array_proc[g].Tw ;
                                    array_proc[g].Tw = 0;
                                    COUNTER++;
                                }
                                flag = true;
                            }

                            static unsigned int minimum = METKA;
                            if(buf_IOL[g] + buff_IOT[g] + start_proc[g] - TIME > 0){
                                if (minimum > buf_IOL[g] + buff_IOT[g] + start_proc[g] - TIME)
                                    minimum = buf_IOL[g] + buff_IOT[g] + start_proc[g] - TIME ;
                            }
                            //std::cout << array_proc[g].IOL <<" "<< buff_IOT[g] <<" "<< start_proc[g]<<" "<< TIME << " 11"<<std::endl;
                            if(round == num_proc*(num_proc-1)){
                                res_proc[COUNTER].ID = "IDLE";

                                res_proc[COUNTER].Time = TIME;
                                TIME = TIME + minimum;
                                COUNTER++;
                            }

                            g = (g + 1) % num_proc;
                            round++;
                        }
                }
            }
            else{
                if(FLAG){
                    unsigned int counter = 0;
                    while((array_proc + counter)->Ti != METKA){
                        counter++;
                    }
                    res_proc[COUNTER].ID = array_proc[counter].ID;
                    res_proc[COUNTER].Time = TIME;
                    if(array_proc[counter].Tw >= TIME_SLICE)
                        TIME += TIME_SLICE;
                    else
                        TIME += array_proc[counter].Tw;
                    array_proc[counter].Tw -= TIME_SLICE ;
                    COUNTER++;
                }
            }
        }
        N_res = COUNTER;
        unsigned tw_end = 0;
        for (unsigned int g = 0; g < num_proc ; g++){
            if(array_proc[g].Tw != 0)
                tw_end++;
        }
        if(!tw_end)
            break;
        cnt++;
    }
}

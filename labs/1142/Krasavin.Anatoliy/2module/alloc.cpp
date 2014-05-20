##include <stdio.h>
#include <iostream>
using namespace std;

int main(void)
{
        cout<<"N block=";
        int N, block, size;
        cin>>N;
        if((N<100)&&(N>10000)){
        cout<<"ERR! \n";
        }
        string command;
        while(1)
        {
        cout<<":";
        cin>>command;
        if (command=="exit") break;     
        if (command=="info"){
        cout<<"FreeBlock:"<< N <<"; "<<endl;
        cout<<"SumBlocks:"<< block <<"; "<<endl;
        cout<<"UsedSpace:"<< size <<"; "<<endl;
        } 
        if (command=="alloc"){   
                int i;
                cout<<"Size:";
                cin>>i;
                N-=i;      
                if (N<0){
                cout<< " - "<<endl;
                N+=i;
                        }
                else
                        {                       
                        block ++;     
                        size +=i;
                        cout<<" + "<<size<<endl;
                        }
                }
        }
}

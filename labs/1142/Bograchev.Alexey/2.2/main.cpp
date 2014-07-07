#include <iostream>

using namespace std;

int main(void){

cout<<"N=";
        int N, block=0, size = 0;
        cin>>N;

        string command;

        while(1){
                cout<<":";
                cin>>command;

                if (command=="exit") break;     

                if(command=="info"){
                        cout<<"Free: "<<N<<"."<<endl<<"Count blocks: "<<block<<"."<<endl<<"Used memory: "<<size<<"."<<endl;
                } 

                if(command=="alloc"){   
                        int i;
                        cout<<"Enter size:";
                        cin>>i;
                        N-=i;      
                        if (N<0){
                                cout<< "-"<<endl;
                                N+=i;
                        }else{                  
                                block ++;     
                                size +=i;
                                cout<<" + "<<size<<endl;
                        }
                }
        }
        return 0;
}
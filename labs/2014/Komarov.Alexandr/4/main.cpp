#include <iostream>
#include <string>
#include <algorithm>


using namespace std;
typedef unsigned short ush;


struct ServiceBlock{
    bool is_free;
    ush offset;
    ush size;
    ush next;
};


struct Memory {
    Memory(size_t n) : N(n) {

        memory = new char[N];
        ServiceBlock* p_sb = (ServiceBlock*) memory;
        p_sb->is_free = 1;
        p_sb->size = 0;
        p_sb->offset = 0;
        p_sb->next = 0;
    }

    ~Memory(){
        delete[] memory;
    }

    ServiceBlock* getServiceBlock( ush i) {
       return (ServiceBlock*) (memory + i);
    }      
    

    int alloc(size_t S) {
        ush cur = 0;  
          
        while( cur != getServiceBlock(cur)->next ) {
           
           ServiceBlock* curSB = getServiceBlock(cur);
           cur = curSB->next;
          
           if( !curSB->is_free ) continue;
           
           ush next = curSB->next;
           size_t free_size = next - curSB->offset - sizeof(ServiceBlock);
           
           if( free_size >= S ) {
               
               curSB->is_free = 0;     
               curSB->size = S;
               if( (free_size - S) > sizeof(ServiceBlock) ) {
                  
                  ServiceBlock* newSB = (ServiceBlock*) 
                     (memory + curSB->offset + sizeof(ServiceBlock) + S);
                  
                  newSB->is_free = 1;
                  newSB->size = 0;
                  newSB->offset = curSB->offset + sizeof(ServiceBlock) + S;
                  newSB->next = curSB->next;
                  
                  curSB->next = newSB->offset;                  
               }
               
               return curSB->offset + sizeof(ServiceBlock);
           }    
        }
        
        size_t free_size = N-cur-sizeof(ServiceBlock);
        if( free_size >= S) {
        
            ServiceBlock* curSB = getServiceBlock(cur);
            
            curSB->is_free = 0;     
            curSB->size = S;
            
            if( (free_size - S) > sizeof(ServiceBlock) ) {
            
               ServiceBlock* newSB = (ServiceBlock*) 
                     (memory + curSB->offset + sizeof(ServiceBlock) + S);
            
               newSB->is_free = 1;
               newSB->size = 0;
               newSB->offset = curSB->offset + sizeof(ServiceBlock) + S;
               newSB->next = newSB->offset;
             
               curSB->next = newSB->offset;                     
            }
            
            return curSB->offset + sizeof(ServiceBlock);
        }
        
        return -1;
    }



    bool free(size_t P) {
       
       size_t goal = P - sizeof(ServiceBlock);
       
       
       size_t prev = 0;
       size_t cur = 0;
       
       ServiceBlock* curSB = getServiceBlock(cur);
       ServiceBlock* prevSB = getServiceBlock(prev);
           
       while( cur != getServiceBlock(cur)->next ) {
           
           curSB = getServiceBlock(cur);
           prevSB = getServiceBlock(prev);
           
           if( cur == goal ) {
           
              ServiceBlock* nextSB = getServiceBlock(curSB->next); 
              
              if(nextSB -> is_free) {
                 
                 if( prevSB->is_free) {
                    if( nextSB->next == nextSB->offset) prevSB->next = prevSB->offset;
                    else prevSB->next = nextSB->next;              
                 }
                 else {
                 
                    if( nextSB->next == nextSB->offset) curSB->next = curSB->offset;
                    else curSB->next = nextSB->next;              
                 
                    curSB->is_free = 1;
                    curSB->size = 0;
                 } 
              }
              else {
                 if( prevSB->is_free ) {
                    
                    prevSB->next = curSB->next;                  
                 }
                 else {
                    curSB->is_free = 1;
                    curSB->size = 0;
                 }
              }
              return 1;
           }
           
           prev = cur;
           cur = curSB->next;
       }
       
       curSB = getServiceBlock(cur);
       prevSB = getServiceBlock(prev);
       
       if( cur == goal ) {
              
              if( prevSB->is_free ) {
                 
                 prevSB->next = prevSB->offset;                  
              }
              else {
              
                 curSB->is_free = 1;
                 curSB->size = 0;
              }
              return 1;
       }
                  
       return 0;
    }




    void map() {
       size_t cur = 0;
       
       while( cur != getServiceBlock(cur)->next ) {
           
           ServiceBlock* curSB = getServiceBlock(cur);
           
           for(int i=0; i!= sizeof(ServiceBlock); ++i) 
            cout << "m";
           
           for(int i=0; i!= curSB->size; ++i) 
            cout << "u";
           
           for(int i=0; i!= curSB->next - sizeof(ServiceBlock) - curSB->size - cur; ++i) 
            cout << "f";
           
           cur = curSB->next;
      }
      
      ServiceBlock* curSB = getServiceBlock(cur);
      
      for(int i=0; i!= sizeof(ServiceBlock); ++i) 
       cout << "m";
           
      for(int i=0; i!= curSB->size; ++i) 
       cout << "u";
           
      for(int i=0; i!= N - sizeof(ServiceBlock) - curSB->size - cur ; ++i) 
       cout << "f";
           
      cout << endl;
    }




    void info() {
       
       size_t allSize = 0;
       size_t allBlocks = 0;
       size_t maxSpace = 0;
       
       size_t cur = 0;
       ServiceBlock* curSB = getServiceBlock(cur);
           
       while( cur != getServiceBlock(cur)->next ) {
         
            curSB = getServiceBlock(cur);
            if(curSB->is_free) {
            
               maxSpace = max(curSB->next - curSB->offset - sizeof(ServiceBlock), maxSpace);   
            }
            else {
               ++allBlocks;
               allSize += curSB->size;
            }
            cur = curSB->next;
       }
       
       
       curSB = getServiceBlock(cur);
       if( curSB->is_free ) {
         maxSpace = max( N - curSB->offset - sizeof(ServiceBlock), maxSpace);   
       }
       else {
         ++allBlocks;
         allSize += curSB->size;
       }
       
       cout << allBlocks << " " << allSize << " " << maxSpace << endl;
    }



private:
    size_t N;
    char* memory;

private:


};




int main(){

    size_t N;
    cin >> N;

    Memory line(N);
    
    //cout << sizeof(ServiceBlock) << endl;    
    
    string command;
    while(1) {
        cin >> command;

        if(command == "ALLOC") {
            size_t S;
            cin >> S;
            int ans = line.alloc(S);
            if(ans < 0) {
                cout << "-" <<endl;
            }
            else {
                cout << "+ " << ans << endl;
            }
        }
        else if ( command == "FREE") {
            size_t P;
            cin >> P;
            bool b = line.free(P);
            if(b) {
                cout <<"+"<<endl;
            }
            else {
                cout <<"-"<<endl;
            }
        }
        else if ( command == "MAP") {
            line.map();
        }
        else if ( command == "INFO") {
            line.info();
        }
    }

    return 0;
}


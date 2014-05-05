#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
 using namespace std;

int main ()
{
string inp;
int size_mem=0;
int free_mem=0;
int user_mem=0;
bool run=true;
int curr_pos=0;
int block=0;

cout << "size of memory"<<endl;
getline(cin,inp);
size_mem=atoi(inp.c_str());


char *mem=new char[size_mem];
for (int i=0;i<size_mem;i++)
	mem[i]=0;

free_mem=size_mem-curr_pos;

while(run)
{
cout << ">";

getline(cin,inp);

switch(inp[0])
{
        case 'a':
          {
            if (inp.compare(0,5,"alloc")==0)
                     {
					   size_t pos=inp.find(" ");
                       string mem_len=inp.substr(pos+1,inp.length()-pos);
					   
					   if ((atoi(mem_len.c_str()))>=free_mem)
					   {
						   cout<<"-"<<endl;
						   break;
					   }
					   mem[curr_pos]=atoi(mem_len.c_str());
					   curr_pos++;
					   block++;
					   for (int i=curr_pos;i<curr_pos+atoi(mem_len.c_str());i++)
					   {
						   mem[i]='*';
					   }
					   curr_pos+=atoi(mem_len.c_str());
					   free_mem=size_mem-curr_pos;
					   user_mem+=atoi(mem_len.c_str());
					   break;
                     }
           }
		   case 'e':
			   {
			      if(inp.compare(0,4,"exit")==0)
				{
                 
		                                cout <<"exit program"<<endl;
						run=false;
						break;
				}
			   }
		   case 'i':
			   {
                if(inp.compare(0,4,"info")==0)
                {
                        cout <<"user blocks "<<endl<<block<<endl;
						cout <<" user memory "<<endl<<user_mem<<endl;
						cout <<" avaliable memory "<<endl<<free_mem<<endl;
						break;
                }
			   }
		   case 'm':
			{
				if (inp.compare(0,3,"map")==0)
				{
					for (int i=0;i<size_mem;i++)
					{
						switch(mem[i])
						{
							        case 0:
										{
											cout << "F|" ;
											break;
										}
									case '*':
										{
											cout << "U|" ;
											break;
										}
									default: 
										{
											cout << "M|" ;
											break;
										}
						}

					}
					cout << endl;
					break;
				}
			}
        default: 
                {
		  cout<<"there's no such cmd" << "\n";
                break;
                }
}
}
delete[] mem;
}

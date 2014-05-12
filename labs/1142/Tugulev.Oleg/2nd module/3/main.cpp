#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
 using namespace std;

int FreeSpace(char* mem,int size,int *amount)
{
  int length=0,max_length=0,ptr=0,tmp_ptr=0;
  bool flag=true;
  for (int i=0;i<size;i++)
    {
      if (mem[i]=='0')
	length++;
       	  if (flag==true)
	    {
	      tmp_ptr=i;
	      flag=false;
	    }
	  if ((length>max_length)&&(mem[i]!='0'))
	    {
	      max_length=length;
	      ptr=tmp_ptr;
	      length=0;
	      tmp_ptr=0;
	      flag=true;
	    }
      	  if((length<=max_length)&&(mem[i]!='0'))
	    {
	      flag=true;
	      length=0;
	      tmp_ptr=0;
	    }
    } 
  if ((max_length==0)&&(length!=0))
    
    *amount=length;
    
  else
    *amount=max_length;
  if (flag==false)
    ptr=tmp_ptr;
  return ptr;
}

int FreeMem(int adr,char* mem)
{
  int size=mem[adr]+1;
  if ((mem[adr]=='0')||(mem[adr]=='*'))
    return -1;
  for (int i=adr;i<adr+size;i++)
    mem[i]='0';
  return size;
}

int main ()
{
string inp;
int size_mem=0;
int free_mem=0;
int user_mem=0;
bool run=true;
int curr_pos=0;
int block=0;

while (run)
   {
     cout << "size of memory (100-10000)"<<endl;
     getline(cin,inp);
     size_mem=atoi(inp.c_str());
     if ((size_mem<100)||(size_mem>10000))
       {
	 cout << "select correct size!"<<endl;
	 size_mem=0;
       }
     else
       run=false;
   }
 run=true;
char *mem=new char[size_mem];
for (int i=0;i<size_mem;i++)
  mem[i]='0';
curr_pos=FreeSpace(mem,size_mem,&free_mem);
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
					   cout <<"+ "<<curr_pos<<endl;
					   curr_pos++;
				  	   block++;
					   for (int i=curr_pos;i<curr_pos+atoi(mem_len.c_str());i++)
					   {
						   mem[i]='*';
					   }
					   curr_pos=FreeSpace(mem,size_mem,&free_mem);
					   //curr_pos+=atoi(mem_len.c_str());
					   //free_mem=size_mem-curr_pos;
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
				 curr_pos=FreeSpace(mem,size_mem,&free_mem);
				 cout <<" avaliable memory "<<endl<<free_mem-1<<endl;
				 
				 break;
			       }
			   }
                   case 'f':
		     {
		       if (inp.compare(0,4,"free")==0)
			 {
			   size_t adr=inp.find(" ");
			   string blk_len=inp.substr(adr+1,inp.length()-adr);
			   int res=FreeMem(atoi(blk_len.c_str()),mem);
			 if (res==-1)
			   cout<<"-"<<endl;
			 else
			   {
			   cout<<"+"<<endl;
			   block--;
			   user_mem-=res;
			   curr_pos=FreeSpace(mem,size_mem,&free_mem);
			   }
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
							        case '0':
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

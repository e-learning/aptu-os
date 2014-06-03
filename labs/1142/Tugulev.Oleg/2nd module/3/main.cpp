#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
 using namespace std;

int FreeSpace(char* mem,int size,int *ptr_inp,int how_much)
{
  int length=0,tmp_ptr=0;
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
	  if ((length==how_much+1))
	    {
			*ptr_inp=tmp_ptr;
			return 0;
	    }
      	  if((mem[i]!='0'))
	    {
	      length=0;
	      flag=true;
	      tmp_ptr=0;
	    }
    } 
  //if ((max_length==0)&&(length!=0))
    
    //*amount=length;
    
  //else
    //*amount=max_length;
  //if (flag==false)
    //*ptr_inp=tmp_ptr;
  //return ptr;
  return -1;
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
bool run=true;
int curr_pos=0;

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
					   
					   if (FreeSpace(mem,size_mem,&curr_pos,atoi(mem_len.c_str()))==-1)
					   {
						   cout<<"-"<<endl;
						   break;
					   }
						
					   mem[curr_pos]=atoi(mem_len.c_str());
					   cout <<"+ "<<curr_pos<<endl;
					   curr_pos++;
				  	
					   for (int i=curr_pos;i<curr_pos+atoi(mem_len.c_str());i++)
					   {
						   mem[i]='*';
					   }
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
			       {int length=0,max_length=0,user_mem=0,user_block=0;
				
				 for (int i=0;i<size_mem;i++)
				{
				if (mem[i]=='0')
					{
					     length++;
						
						if (length>=max_length)
						    {
							max_length=length;
							
						    }
					}
				
				else
					{
					    length=0;
					    if(mem[i]=='*')
						user_mem++;
					    else
						user_block++;
					}
				}
				cout<<"avalible memory "<<max_length-1<<endl;
				cout<<"user memory  "<<user_mem<<endl;
				cout<<"user blocks " <<user_block<<endl;
				
				
				
				 
				 
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

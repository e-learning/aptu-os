///////////////////////////////////////////////////////////////////
//                                                               //
//             Vin92.18.05.2014                                  //
//                              FTP                              //
//                                                               //
///////////////////////////////////////////////////////////////////


#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

int sock;
int sock2;

using namespace std;

string s_comm_args(string in_zn, int mode){
	string rez0, rez1;
	char flag=0, flag2=0;
	for (int i=0; i<strlen(in_zn.c_str()); i++){
		if (!flag2){
			 if (in_zn[i] != ' ')
			 	flag2=1;
		}
		if (flag2){
			if (flag){
				rez1+=in_zn[i];
			}else{
				if (in_zn[i] == ' '){ 
					flag = 1;
				}else{
					rez0+=in_zn[i];
				}		
			}
		}
	}
	if (mode) return rez1;
	return rez0;
} 

int CMD(string zn){

	zn+="\n";

	send(sock, zn.c_str(), zn.length(), 0);
	char r[4];
	r[3]=0;

	char buf[255]; 

	for(int i=0; i<255; i++) buf[i] = 0;
	recv(sock, buf, 255, 0);
	for(int i=0; i<4; i++) r[i]=buf[i];
	cout<<buf;

	return atoi(r);
}



int main(void){


	cout<<"FTP"<<endl;

	cout<<endl<<"Create socket: ";
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sock2 = socket(AF_INET, SOCK_STREAM, 0);

	if ((sock > 0)&&(sock2 > 0)){
		cout<<"OK"<<endl;
	}else{
		cout<<"ERR"<<endl;
		return -1;
	}

/////////////////////////////////////////////////////////////////

	in_addr ip_addr;

	string ip_addr_s;
	cout<<"ip>>";
//	cin>>ip_addr_s;
	ip_addr_s = "192.168.1.38";

	ip_addr.s_addr = inet_addr(ip_addr_s.c_str());

	if (ip_addr.s_addr!=INADDR_NONE){
		cout<<"ip:OK"<<endl;
	}else{
		cout<<"ERR"<<endl;
		return -2;
	}

/////////////////////////////////////////////////////////////////
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(21);
	addr.sin_addr = ip_addr;

	
	cout<<"Connect to "<<ip_addr_s<<"...";

	if(!connect(sock, (struct sockaddr *)&addr, sizeof(addr) )){
		cout<<"OK"<<endl;
	}else{
		cout<<"ERR"<<endl;
		return -3;
	}


/////////////////////////////////////////////////////////////////
	{
		char* buf = new char[255];
		for(int i=0; i<255; i++) buf[i] = 0;
		recv(sock, buf, 255, 0);
		cout<<endl<<buf<<endl;
		delete [] buf;
	}

	string user_name;
	string user_pass;

	cout<<"Enter login? (y/n)";
	{
		string tmp;
		cin>>tmp;
		if (tmp == "y"){
			cout<<"User name:";
			cin>>user_name;
//			getline(cin,user_name);
//			while (user_name == "") getline(cin,user_name);

			tmp="USER " + user_name;

			int r = CMD(tmp);

			if ((r>200) && (r < 333)){
				if (r == 331){
					cout<<"Enter password:";
					cin>>user_pass;
//					getline(cin,user_pass);
					tmp="PASS " + user_pass;
					cout<<tmp;
					r = CMD(tmp);
					if (!((r>=200) && (r<300))){
						cout<<"ERR:"<<endl;
						return -3;
					}
				}	
			}
		}else{

		CMD("USER");

		}
	}

//////////////////////////////////////////////////////////////////////////////
	sockaddr_in addr_data;
	int port_nomer;
	{
	string zn="PASV\n";
	send(sock, zn.c_str(), zn.length(), 0);
	char buf[255]; 

	for(int i=0; i<255; i++) buf[i] = 0;
	recv(sock, buf, 255, 0);
cout<<buf;	
	int i=0;
	while((buf[i] != ')')&& (buf[i] != 0)) i++;
	
	while(buf[i] != ',') i--;

	string ch2="";
	i++;
	while(buf[i] != ')') { ch2+=buf[i]; i++; }

	while(buf[i] != ',') i--;
	i--;
	while(buf[i] != ',') i--;
	i++;
	string ch1="";
	while(buf[i] != ',') { ch1+=buf[i]; i++; }
	cout<<endl<<"ch2 "<<ch1<<endl<<"ch1 "<<ch2<<endl;
	port_nomer=atoi(ch2.c_str())*256 + atoi(ch1.c_str());
	cout<<"port: "<<port_nomer<<endl;
	}

	addr_data.sin_family = AF_INET;
	addr_data.sin_port = htons(port_nomer);
	addr_data.sin_addr = ip_addr;

	if(!connect(sock2, (struct sockaddr *)&addr_data, sizeof(addr_data))){
		cout<<"data connect"<<endl;
	}else{
		cout<<"Err data"<<endl;
	} 

////////////////////////////////////////////////////////////////////////////////

	string comm;
	int rez=0;

	while (1){
//		cin>>comm;
//		getline(cin,comm);
		while (comm == "") getline(cin,comm);


		if (comm == "exit"){ break; }

		rez = CMD(comm);

		if (rez==331){
		cout<<"Password:";
		getline(cin,comm);
		comm="PASS " + comm;
		rez = CMD(comm);
		}
		{
			char* buf = new char[255];
			for(int i=0; i<255; i++) buf[i] = 0;
			recv(sock2, buf, 255, 0);
			cout<<endl<<buf<<endl;
			delete [] buf;
		}




		comm="";
	}






////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////

	close(sock);

	return 0;
}

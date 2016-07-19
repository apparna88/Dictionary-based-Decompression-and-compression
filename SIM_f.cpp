/*
	Author : Aparna Hariyani
	UFID   : 69185846  
	Project 2: Compression Decompression using given formats
	"On my honor, I have neither given nor received unauthorized aid on this assignment"
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <bitset>
#include <stdlib.h>

using namespace std;

vector<string> Original;
vector<string> Compressed;
vector<string> Decompressed;
vector<string> Compressed_Input;
vector<string> Dictionary;
vector<string> SortOriginal;
vector<string> Count_Of_Sorted;	
vector<string> Reference;
vector<string> Temp_Decompression;
int Global_Buffer =0; 
string Bitmap;
string temp_dict;
string DictionaryData_temp;

void Get_Input();

void Freq_Sort();

void Index_Sort();

void CheckFirstArrival(string& , string&);

void Build_Dictionary();

void Map_To_Dictionary();

void Compress_File();

void Reinit(char *);

void Write_Compressed_To_File();

void Execute_Compression();
void Decompress();

void Get_Compressed_Input();

void Separate_Data_And_Dictionary();

void Merge_Reference();

void Reinit_Decompression();

void Match(char * , int );
void Direct_Match(char * , int );
void No_Match(char * , int );
void Two_Bit_Consecutive_Match(char * , int );
void One_Bit_Consecutive_Match(char * , int );
void Bitmask_Consecutive_Match(char * , int );
void Two_Bit_Anywhere_Match(char * , int );
void Run_Length_Encoding(char * , int );


void Print_Decompressed();

void Final_Decompress(string , int);

string MapDictionary(string);

void Write_Decompressed_To_File();

int main(int argc , char *argv[]){
	if( argc != 2){
		cout<<"\n Incorrect Usage";
		cout<<"\n Use ./SIM 1 - For Compression";
		cout<<"\n Use ./SIM 2 - For Decompression";
	}
	int argument = stoi(argv[1],nullptr);
	switch(argument){
		case 1: 
				Execute_Compression();
		break;
		case 2: 
				Decompress();
		break;
		default: cout<<"\n Incorrect Usage \n";
	}
}
void Get_Input(){
	ifstream Input_Stream;
	string BitCodes;
	Input_Stream.open("original.txt");
	while(Input_Stream >> BitCodes){
		Original.push_back(BitCodes);
		SortOriginal.push_back(BitCodes);
	}
	Input_Stream.close();
}

void Freq_Sort(){
	string temp;
	
	std::sort(SortOriginal.begin(),SortOriginal.end());
	for(int i=0; i<SortOriginal.size();i++){
		temp = SortOriginal[i];
		int count = 1;
		for(int j=i+1;j<SortOriginal.size();j++){
			if(SortOriginal[j] == temp){
				SortOriginal.erase(SortOriginal.begin()+j);
				count++;
				j=j-1;
			}
		}
		if(count < 10){
			Count_Of_Sorted.push_back(to_string(count)+ "  " + temp );
		}
		else if (count < 100){
			Count_Of_Sorted.push_back(to_string(count)+ " " + temp );
		}
	}
	sort(Count_Of_Sorted.rbegin(), Count_Of_Sorted.rend(), [] (const std::string& a, const std::string& b) { 
		std::string cmp_a { a.begin() , a.end() };
		std::string cmp_b { b.begin() , b.end() };
		return std::stoi(cmp_a) < std::stoi(cmp_b);
	});


}

void Index_Sort(){
	for(int i=0;i<Count_Of_Sorted.size()-1;i++){
		if(Count_Of_Sorted[i].substr(0,3)== Count_Of_Sorted[i+1].substr(0,3))
			CheckFirstArrival(Count_Of_Sorted[i],Count_Of_Sorted[i+1]);
	}
}

void CheckFirstArrival(string &A, string &B){
	int arrivalA = -1;
	int arrivalB = -1;
	for (int i = 0; i < Original.size(); ++i)
	{
		if(A.substr(3) == Original[i]){
			arrivalA = i;
			break;
		}
	}
	for (int i = 0; i < Original.size(); ++i)
	{
		if(B.substr(3) == Original[i]){
			arrivalB = i;
			break;
		}
	}
	if (arrivalB < arrivalA)
	{
		string temp;
		temp = B;
		B = A;
		A= temp;
	}
}
void Build_Dictionary(){
	for(int i=0;i<8;i++){
		string temp = bitset<3>(i).to_string();
		Dictionary.push_back( temp +" "+Count_Of_Sorted[i].substr(3));
	}
}
void Map_To_Dictionary(){
	int flag = 0;
	int flag1 = 0;
	int RLCount =0;
	int final_count = 0;
	string RLE;
	for(int i = 0;i!=Original.size();i++){
		flag1 =0;
		if((i!=0) && (Original[i] == Original[i-1]) & (RLCount < 4)){
			flag1 =1;
			RLCount++;
			if(RLCount == 4)
			{
				final_count =1;
				flag1 = 2;
			}
			else 
				final_count=0;
		}
		else {
			final_count =1;
		}
		if(final_count == 1){
			
			if(RLCount == 1){
				RLE = "00";
			}
			else if(RLCount == 2){
				RLE = "01";
			}
			else if(RLCount == 3){
				RLE = "10";
			}
			else if(RLCount == 4){
				RLE = "11";
			}
			else 
				goto skip;
			if(final_count == 1){
				Reference.push_back( "000 "+ RLE);
				final_count = 0;
				RLCount = 0;
				if(flag1 !=2)
					flag1 = 0;	
			}

		if (flag1 == 0 )
		{
	skip:	
			for(int j = 0; j != Dictionary.size() ; j++){
				if(Original[i] == Dictionary[j].substr(4)){
					Reference.push_back( "101 "+ Dictionary[j].substr(0,3));
					flag=1;
					break;
				}
				else
					flag=0;
				
			}
			if(flag == 0)
			{
				int Consecutive_Mismatch = 0;
				int Anywhere_Mismatch = 0;
				int ConsecutiveStartingBit = -1;
				int Anywhere_First_Bit = -1;
				int Anywhere_Second_Bit = -1;
				int Mismatch_Count = 0;
				int Bitmask = 0;

				for(int j= 0 ; j != Dictionary.size() ; j++){
					Mismatch_Count=0;
					string temp1 = Original[i];	
					string temp2 = Dictionary[j].substr(4);
					for(int k=0 ; k<=temp1.length() - 1 ; k++){
						if(temp1[k] != temp2[k]){
							if(Mismatch_Count==0){
								ConsecutiveStartingBit = k;
								if(temp1[k+1] != temp2[k+1])
								{
									Consecutive_Mismatch=1;
								}
							}
							Mismatch_Count++;
						}
					}
				
					if(Mismatch_Count==1) 
					{
						string TempStartBit = bitset<5>(ConsecutiveStartingBit).to_string(); 
						Reference.push_back( "010 "+ TempStartBit + " " + Dictionary[j].substr(0,3));
						Consecutive_Mismatch = 1;
						break;
					}
					if(Mismatch_Count==2 & Consecutive_Mismatch ==1)
					{
						string TempStartBit = bitset<5>(ConsecutiveStartingBit).to_string();
						Reference.push_back( "011 "+ TempStartBit + " " + Dictionary[j].substr(0,3));
						Consecutive_Mismatch = 1;
						break;
					}
								
					else
					   Consecutive_Mismatch=0;
				}
				if(Consecutive_Mismatch == 0 & Anywhere_Mismatch ==0){
					for(int j= 0 ; j != Dictionary.size() ; j++){
						Mismatch_Count=0;
						Bitmap.erase(Bitmap.begin() , Bitmap.end());
						string temp1 = Original[i];	
						string temp2 = Dictionary[j].substr(4);
						for(int k=0 ; k<=temp1.length() - 1 ; k++){
							if(temp1[k] != temp2[k]){
								if(Mismatch_Count==0)
								{
									ConsecutiveStartingBit = k;
									for(int x=0; x < 4; x++){
										if(temp1[k+x] != temp2[k+x])
										{
											Bitmask++;
											if(temp1[k+x] != temp2[k+x])
												Bitmap.push_back('1');
											else 
												Bitmap.push_back('0');
										}
										else {
											if(temp1[k+x] != temp2[k+x])
												Bitmap.push_back('1');
											else 
												Bitmap.push_back('0');
										}
										
									}
								}
								Mismatch_Count++;
							}
						}
						
					
						if((Mismatch_Count==2 & Bitmask ==2 ) || (Mismatch_Count==3 & Bitmask ==3 ) || (Mismatch_Count==4 & Bitmask ==4 )) //
						{
							string TempStartBit = bitset<5>(ConsecutiveStartingBit).to_string();
							Reference.push_back( "001 "+ TempStartBit + " " + Bitmap + " " + Dictionary[j].substr(0,3));
							Bitmask = 1;
							Mismatch_Count=0;
							break;
						}	
						else {
							Bitmask = 0;
							Consecutive_Mismatch =0;
						}
					}
				}					
				if(Consecutive_Mismatch == 0 & Bitmask == 0){
					for(int j= 0 ; j != Dictionary.size() ; j++){
					Mismatch_Count=0;
					string temp1 = Original[i];	
					string temp2 = Dictionary[j].substr(4);
					for(int k=0 ; k<=temp1.length() - 1 ; k++){
						if(temp1[k] != temp2[k]){
							if(Mismatch_Count==0){
								Anywhere_First_Bit = k;
								}
							if(Mismatch_Count==1){
								Anywhere_Second_Bit = k;
								Anywhere_Mismatch=1;
							}
							Mismatch_Count++;
							}
							
						}
						if(Mismatch_Count==2 & Anywhere_Mismatch == 1)
						{
							string Temp_Start_Bit_One = bitset<5>(Anywhere_First_Bit).to_string(); // To convert starting bit to 5 bit number
							string Temp_Start_Bit_Two = bitset<5>(Anywhere_Second_Bit).to_string();
							Reference.push_back( "100 "+ Temp_Start_Bit_One + " " + Temp_Start_Bit_Two +" "+Dictionary[j].substr(0,3));
							break;
						}
						else{
							Anywhere_Mismatch=0;
							Bitmask = 0;
							Consecutive_Mismatch =0;
						}
					}
					if(Anywhere_Mismatch == 0 & Consecutive_Mismatch == 0 & Bitmask == 0){//&& Mismatch_Count==0){
						Reference.push_back("111 "+ Original[i]);
					}			
				}
				
			}
			
		}
		}
	}
}
void Compress_File(){
	char temp[32];
	int buffer = 32;
	int k=0;
	Reinit(temp);
	for(int i=0;i<Reference.size();i++){
			string tempRef = Reference[i];
			for(int j=0;j<tempRef.length();j++){
				if(k<32){
					if(tempRef[j] != ' ' && tempRef[j] != '\n'){
					temp[k]=tempRef[j];
					k++;
					buffer--;
					if(k==32){
					Compressed.push_back(temp);
					Reinit(temp);
					k=0;
					buffer=32;
					}
					
				}

			}
				
			}
	}
	if(buffer!=32){
				for(int i=31;i>31-buffer;i--)
					temp[i]='1';
				Compressed.push_back(temp);
	}
	Compressed.push_back("xxxx");
	for(int i=0; i<Dictionary.size(); i++){
		Compressed.push_back(Dictionary[i].substr(4));
	}
}
void Reinit(char *a){
	for(int i=0;i<32;i++){
		a[i]='x';
	}
	a[32]='\0';
}

void Write_Compressed_To_File(){
	ofstream Output_Stream;
	Output_Stream.open("cout.txt");

	for (auto i = Compressed.begin() ; i!=Compressed.end();i++)
	{
		Output_Stream<<*i<<"\n";
	}
	Output_Stream.close();
}

void Execute_Compression(){

				Get_Input();
				Freq_Sort(); 
				Index_Sort();	
	
				for(int i=0;i<Count_Of_Sorted.size();i++)
					Index_Sort();
	

				Build_Dictionary();
				Map_To_Dictionary();
				Compress_File();
				Write_Compressed_To_File();
}
void Get_Compressed_Input(){
	ifstream Input_Stream;
	string BitCodes;
	Input_Stream.open("cout.txt");
	while(Input_Stream >> BitCodes){
		Compressed_Input.push_back(BitCodes);
	}
	Input_Stream.close();
}
void Separate_Data_And_Dictionary(){
	int flag=0;
	for(int i=0;i<Compressed_Input.size();i++){
		string temp = Compressed_Input[i];
		if(flag==0 && temp != "xxxx"){
			Reference.push_back(Compressed_Input[i]);
		}
		if(flag==1 && temp != "xxxx"){
			Dictionary.push_back(Compressed_Input[i]);
		}
		if(temp == "xxxx"){
			flag=1;
		}
	}
}
void Reinit_Decompression(char *a, int size){
	for(int i=0;i<size;i++){
		a[i]='x';
	}
	a[size]='\0';
}
void Merge_Reference(){
	int ReferenceSize = Reference.size();
	int CharArraySize = ReferenceSize*32; 
	Global_Buffer=CharArraySize;
	char temp[CharArraySize];
	Reinit_Decompression(temp, CharArraySize);
	int k=0;
	for(int i=0;i<Reference.size();i++){
		string tempRef = Reference[i];
		for(int j=0;j<tempRef.length();j++){
			if(tempRef[j] != ' ' && tempRef[j] != '\n'){
				if(k<CharArraySize){
					temp[k]=tempRef[j];
					k++;
				}
				else
					cout<<"\n I went out of bound";
			}
		}	
	}
	Match(temp,0);
}

void Match(char * TempArray , int curr){
	int current_pos=curr;
	char a = TempArray[current_pos];
	char b = TempArray[current_pos+1];
	char c = TempArray[current_pos+2];
	if(a=='0'){
		if(b=='0'){
			if(c=='0'){
				Run_Length_Encoding(TempArray,current_pos);
			}
			else if(c=='1'){
				Bitmask_Consecutive_Match(TempArray,current_pos);
			}
			
		}
		
		else if(b=='1'){
			if(c=='0'){
				One_Bit_Consecutive_Match(TempArray,current_pos);
			}
			if(c=='1'){
				Two_Bit_Consecutive_Match(TempArray,current_pos);
			}
		}
	}
	else if(a=='1'){
		if(b=='0'){
			if(c=='0'){
				Two_Bit_Anywhere_Match(TempArray,current_pos);
			}
			else if(c=='1'){
				Direct_Match(TempArray,current_pos); 
			}
			
		}
		
		else if(b=='1'){
			if(c=='1'){
				No_Match(TempArray,current_pos);
			}
		}
	}
}
void Direct_Match(char * temp,int current_pos){
	int buffer = 6;
	Global_Buffer = Global_Buffer - buffer;
	string buffer_String = "xxx xxx";
	string Dictionary_Mapping = "xxx";
	int k=0;
	for(int i=0;i<7;i++){
		if(i == 3){
			buffer_String[i]=' ';
		}
		else
		{
			buffer_String[i]= temp[current_pos+k];
			k++;
		}
	}
	if(Global_Buffer>-1){
		Temp_Decompression.push_back(buffer_String);
		Final_Decompress(buffer_String,5); // String + Mode
		Match(temp,current_pos+buffer);
	}
}
void No_Match(char * temp,int current_pos){
	int buffer = 35;
	Global_Buffer = Global_Buffer - buffer;
	string buffer_String = "xxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	int k=0;
	for(int i=0;i<36;i++){
		if(i == 3){
			buffer_String[i]=' ';
		}
		else
		{
			buffer_String[i]= temp[current_pos+k];
			k++;
		}
	}
	if(Global_Buffer>-1){
		Temp_Decompression.push_back(buffer_String);
		Final_Decompress(buffer_String,7); // String + Mode
		Match(temp,current_pos+buffer);
	}
}

void Bitmask_Consecutive_Match(char * temp,int current_pos){
	int buffer = 15;
	Global_Buffer = Global_Buffer - buffer;
	string buffer_String = "xxx xxxxx xxxx xxx";
	string Dictionary_Mapping = "xxx";
	int k=0;
	for(int i=0;i<18;i++){
		if(i == 3 || i==9 || i==14){
			buffer_String[i]=' ';
		}
		else
		{
			buffer_String[i]= temp[current_pos+k];
			k++;
		}
	}
	if(Global_Buffer>-1){
		Temp_Decompression.push_back(buffer_String);
		Final_Decompress(buffer_String,1); 
		Match(temp,current_pos+buffer);
	}
}
void Two_Bit_Consecutive_Match(char * temp,int current_pos){
	int buffer = 11;
	Global_Buffer = Global_Buffer - buffer;
	string buffer_String = "xxx xxxxx xxx";
	string Dictionary_Mapping = "xxx";
	int k=0;
	for(int i=0;i<13;i++){
		if(i == 3 || i==9){
			buffer_String[i]=' ';
		}
		else
		{
			buffer_String[i]= temp[current_pos+k];
			k++;
		}
	}
	if(Global_Buffer>-1){
		Temp_Decompression.push_back(buffer_String);
		Final_Decompress(buffer_String,3); 
		Match(temp,current_pos+buffer);
	}
}

void One_Bit_Consecutive_Match(char * temp,int current_pos){
	int buffer = 11;
	Global_Buffer = Global_Buffer - buffer;
	string buffer_String = "xxx xxxxx xxx";
	string Dictionary_Mapping = "xxx";
	int k=0;
	for(int i=0;i<13;i++){
		if(i == 3 || i==9){
			buffer_String[i]=' ';
		}
		else
		{
			buffer_String[i]= temp[current_pos+k];
			k++;
		}
	}
	if(Global_Buffer>-1){
		Temp_Decompression.push_back(buffer_String);
		Final_Decompress(buffer_String,2); 
		Match(temp,current_pos+buffer);
	}
}
void Two_Bit_Anywhere_Match(char * temp,int current_pos){
	int buffer = 16;
	Global_Buffer = Global_Buffer - buffer;
	string buffer_String = "xxx xxxxx xxxxx xxx";
	string Dictionary_Mapping = "xxx";
	int k=0;
	for(int i=0;i<19;i++){
		if(i == 3 || i == 9 || i==15){
			buffer_String[i]=' ';
		}
		else
		{
			buffer_String[i]= temp[current_pos+k];
			k++;
		}
	}
	if(Global_Buffer>-1){
		Temp_Decompression.push_back(buffer_String);
		Final_Decompress(buffer_String,4);
		Match(temp,current_pos+buffer);
	}
}

void Run_Length_Encoding(char * temp,int current_pos){
	int buffer = 5;
	Global_Buffer = Global_Buffer - buffer;
	string buffer_String = "xxx xx";
	string Dictionary_Mapping = "xxx";
	int k=0;
	for(int i=0;i<6;i++){
		if(i == 3){
			buffer_String[i]=' ';
		}
		else
		{
			buffer_String[i]= temp[current_pos+k];
			k++;
		}
	}
	if(Global_Buffer>-1){
		Temp_Decompression.push_back(buffer_String);
		Final_Decompress(buffer_String,0); 
		Match(temp,current_pos+buffer);
	}
}

void Print_Decompressed(){
	for (auto i = Decompressed.begin() ; i!=Decompressed.end();i++)
	{
		cout<<*i<<"\n";
	}
}
void Final_Decompress(string data , int mode){
	string Dictionary_Data;
	string Dictionary_Match="xxx";
	string Run_Length = "xx";
	char Start_Bit[6] = {'x','x','x','x','x','\0'};
	char First_Bit[6] = {'x','x','x','x','x','\0'};
	char Second_Bit[6] = {'x','x','x','x','x','\0'};
	int Start_Bit_Location;
	int First_Bit_Location;
	int Second_Bit_Location;
	int Run_Length_Count;

	switch(mode){
		case 0 :
				Run_Length[0]= data[4];
				Run_Length[1]= data[5];
				if(Run_Length == "00")
					Run_Length_Count=1;
				else if (Run_Length == "01")
					Run_Length_Count=2;
				else if (Run_Length == "10")
					Run_Length_Count=3;
				else if (Run_Length == "11")
					Run_Length_Count=4;
				for(int i=0; i < Run_Length_Count; i++){
					Decompressed.push_back(DictionaryData_temp);
				}
				break;
		case 1 : //Bitmask
				 Dictionary_Match[0]= data[15];
				 Dictionary_Match[1]= data[16];
				 Dictionary_Match[2]= data[17];
				 Dictionary_Data = MapDictionary(Dictionary_Match);
				 
				 Start_Bit[0] = data[4];
				 Start_Bit[1] = data[5];
				 Start_Bit[2] = data[6];
				 Start_Bit[3] = data[7];
				 Start_Bit[4] = data[8];
				 Start_Bit_Location = strtol(Start_Bit,nullptr,2);
				if(Dictionary_Data[Start_Bit_Location] != data[10]) 
					Dictionary_Data[Start_Bit_Location] = '1';
				else 
					Dictionary_Data[Start_Bit_Location] = '0';
				if(Dictionary_Data[Start_Bit_Location+1] == data[11])
					Dictionary_Data[Start_Bit_Location+1] = '0';
				else 
					Dictionary_Data[Start_Bit_Location+1] = '1';
				if(Dictionary_Data[Start_Bit_Location+2] == data[12])
					Dictionary_Data[Start_Bit_Location+2] = '0';
				else 
					Dictionary_Data[Start_Bit_Location+2] = '1';
				if(Dictionary_Data[Start_Bit_Location+3] == data[13])
					Dictionary_Data[Start_Bit_Location+3] = '0';
				else
					Dictionary_Data[Start_Bit_Location+3] = '1';
					
				 DictionaryData_temp = Dictionary_Data;
				 Decompressed.push_back(Dictionary_Data);
				break;
		case 2 : //1 bti mismatch
				 Dictionary_Match[0]= data[10];
				 Dictionary_Match[1]= data[11];
				 Dictionary_Match[2]= data[12];
				 Dictionary_Data = MapDictionary(Dictionary_Match);
				 //Start Bit Map
				 Start_Bit[0] = data[4];
				 Start_Bit[1] = data[5];
				 Start_Bit[2] = data[6];
				 Start_Bit[3] = data[7];
				 Start_Bit[4] = data[8];
				 Start_Bit_Location = strtol(Start_Bit,nullptr,2);
				 if(Dictionary_Data[Start_Bit_Location] == '0'){
				 	Dictionary_Data[Start_Bit_Location] = '1';
				 }
				 else{
				 	Dictionary_Data[Start_Bit_Location] = '0';
				 }
				 DictionaryData_temp = Dictionary_Data;
				 Decompressed.push_back(Dictionary_Data);
		break;
		case 3 : //2 bit consecutive mismatch
				 Dictionary_Match[0]= data[10];
				 Dictionary_Match[1]= data[11];
				 Dictionary_Match[2]= data[12];
				 Dictionary_Data = MapDictionary(Dictionary_Match);
				 //Start Bit Map
				 Start_Bit[0] = data[4];
				 Start_Bit[1] = data[5];
				 Start_Bit[2] = data[6];
				 Start_Bit[3] = data[7];
				 Start_Bit[4] = data[8];
				 Start_Bit_Location = strtol(Start_Bit,nullptr,2);
				 if(Dictionary_Data[Start_Bit_Location] == '0'){
				 	Dictionary_Data[Start_Bit_Location] = '1';
				 }
				 else{
				 	Dictionary_Data[Start_Bit_Location] = '0';
				 }
				 if(Dictionary_Data[Start_Bit_Location+1] == '0'){
				 	Dictionary_Data[Start_Bit_Location+1] = '1';
				 }
				 else{
				 	Dictionary_Data[Start_Bit_Location+1] = '0';
				 }
				 DictionaryData_temp = Dictionary_Data;
				 Decompressed.push_back(Dictionary_Data);
				break;
		case 4 : 
				 Dictionary_Match[0]= data[16];
				 Dictionary_Match[1]= data[17];
				 Dictionary_Match[2]= data[18];
				 Dictionary_Data = MapDictionary(Dictionary_Match);
				 First_Bit[0] = data[4];
				 First_Bit[1] = data[5];
				 First_Bit[2] = data[6];
				 First_Bit[3] = data[7];
				 First_Bit[4] = data[8];
				 First_Bit_Location = strtol(First_Bit,nullptr,2);
				 if(Dictionary_Data[First_Bit_Location] == '0'){
				 	Dictionary_Data[First_Bit_Location] = '1';
				 }
				 else{
				 	Dictionary_Data[First_Bit_Location] = '0';
				 }
				 Second_Bit[0] = data[10];
				 Second_Bit[1] = data[11];
				 Second_Bit[2] = data[12];
				 Second_Bit[3] = data[13];
				 Second_Bit[4] = data[14];
				 Second_Bit_Location = strtol(Second_Bit,nullptr,2);
				 if(Dictionary_Data[Second_Bit_Location] == '0'){
				 	Dictionary_Data[Second_Bit_Location] = '1';
				 }
				 else{
				 	Dictionary_Data[Second_Bit_Location] = '0';
				 }
				 DictionaryData_temp = Dictionary_Data;
				 Decompressed.push_back(Dictionary_Data);
		break;
		case 5 : //Direct match
				 Dictionary_Match[0]= data[4];
				 Dictionary_Match[1]= data[5];
				 Dictionary_Match[2]= data[6];
				 Dictionary_Data = MapDictionary(Dictionary_Match);
				 DictionaryData_temp = Dictionary_Data;
				 Decompressed.push_back(Dictionary_Data);
		break;
		case 7 : //no match
				DictionaryData_temp = data.substr(4);
				Decompressed.push_back(data.substr(4));
		break;
		default : cout<<"\n Some Error in Final Decompress";
	}
}
string MapDictionary(string data){
	if(data == "000"){
		return Dictionary[0];
	}
	if(data == "001"){
		return Dictionary[1];
	}
	if(data == "010"){
		return Dictionary[2];
	}
	if(data == "011"){
		return Dictionary[3];
	}
	if(data == "100"){
		return Dictionary[4];
	}
	if(data == "101"){
		return Dictionary[5];
	}
	if(data == "110"){
		return Dictionary[6];
	}
	if(data == "111"){
		return Dictionary[7];
	}
}
void Write_Decompressed_To_File(){
	ofstream Output_Stream;
	Output_Stream.open("dout.txt");

	for (auto i = Decompressed.begin() ; i!=Decompressed.end();i++)
	{
		Output_Stream<<*i<<"\n";
	}
	Output_Stream.close();
}
void Decompress(){
	Get_Compressed_Input();
	Separate_Data_And_Dictionary();
	Merge_Reference();
	Write_Decompressed_To_File();

}


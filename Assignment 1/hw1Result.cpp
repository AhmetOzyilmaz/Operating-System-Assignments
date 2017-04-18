
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>
//you may need to add more functions

using namespace std;

// CPU GTU_C312



class mmry
{
public:
	mmry(int i,int v){
		index=i;
		value=v;
	}
	~mmry() {}
	int index;
	int value;
	
};


class instruction
{
public:
	instruction(int i,string &s,int a=0,int b=0){
		index=i;
		command=s;
		A=a;
		B=b;
	}
	~instruction(){}
	int index;
	string command;                  //like "ADD" ,"SET" ...
	int A;
	int B;

	
};




class Simulate
{
public:

	Simulate(string &filename,int md){
		parseFile(filename);
		mode=md;

	}
	virtual ~Simulate() {}
	vector<mmry> memory;
	vector<instruction> instructions;
	int mode;
	void parseFile(string &filename){
		//initialize memory and instructions after parsing file
		/* Memory i doldurup ondan sonra  sonrada komutları alacak*/
		string line;
		string  LineEnd;
		char temp[7];
		string beginRow = "Begin Data Section";
		string endRow = "End Data Section";
		strcpy(temp,filename.c_str());
			//cout<<beginRow<<endl;
		int i = 0;
	 	ifstream myfile(temp) ;
	 	int memoryFlag =0,InstructionFlagStart=0,InstructionFlagEnd = 0 ,DataSectionFlagStart= 0,DataSectionFlagEnd = 0;


	 	while(myfile >> line) 
		{
			//cout<<line<<endl;
			if(line[0]=='#'){
				//cout << "DEBUG "<<endl;
					//cout << line<<"  ";

					getline(myfile,LineEnd);/* yorum kısımların sting e alındığı kısım */
					//cout << LineEnd<< endl;
			}
			else if (line == "Begin"){
				myfile >> line;
				if(line =="Data"){
					myfile >> line;
					
					if(line=="Section"){
					//	cout << "START Memory Adrees"<<endl;/* begin data section satırı kısmı */
						DataSectionFlagStart = 1;
						DataSectionFlagEnd = 0;
					}
					
				}
				else if (line == "Instruction"){
					myfile >> line;
					if (line == "Section"){
						
					//	cout << "START Instruction  line"<<endl;
						InstructionFlagStart = 1;
					}

				}
			}
			else if (line == "End"){
				myfile >> line;

				if(line =="Data")
				{
					myfile >> line;

					if(line=="Section")
					{
					//	cout << "END OF Memory Adrees"<<endl;/* begin data section satırı kısmı */
						DataSectionFlagStart = 1;
						DataSectionFlagEnd = 1;
					}

				}
				else if (line == "Instruction")
				{

					myfile >> line;
					if(line == "Section"){
						myfile >> line;
						InstructionFlagEnd = 1;
					//	cout << "END OF Instruction Section"<<endl;/* begin data section satırı kısmı */

					}
					else
						cout << "ERROR INPUT "<<endl;
					 
				}
			}
			else if (isdigit(line[0]) && DataSectionFlagStart == 1 && DataSectionFlagEnd == 0 && InstructionFlagStart == 0  && InstructionFlagEnd == 0  )  
			{
				char temp[sizeof(line)];
				strcpy(temp,line.c_str());

				int Adress =atoi(temp);
				myfile >> line;
				strcpy(temp,line.c_str());

				int value =atoi(temp);

				mmry mem(Adress,value);
				memory.push_back(mem);
				//cout <<Adress <<"  " << value<< endl;

			}
			else if (isdigit(line[0]) && InstructionFlagStart == 1 && InstructionFlagEnd == 0 && DataSectionFlagStart == 1 && DataSectionFlagEnd == 1 ){
				//cout <<"AAAAAAAAAAAAAAAAAAAA"<<endl;
				//cout<<line<<endl;
				char temp1[sizeof(line)];
				strcpy(temp1,line.c_str());
				int index = atoi(temp1);
				string s;
				myfile >> s;
				if(s!="HLT"){
					//cout<< "  " << s;
					myfile >> line;
					strcpy(temp1,line.c_str());
					int a = atoi(temp1);
					//cout <<"  "<<a ;
					myfile >> line;
					strcpy(temp1,line.c_str());		
					int b = atoi(temp1);
					//cout <<endl<< "b ->"<<b <<"<-"<<endl;

					instruction temp(index,s,a,b);	
					instructions.push_back(temp);
					//cout<<"  " << b <<"  "<<endl;
				}
				else
					{
						instruction temp(index,s,0,0);	
						instructions.push_back(temp);
					}
			}
			
		}
		    myfile.close();

		}

	void printMemory(){
		for (int i = 0; i < memory.size(); ++i)
		{
			cout<<memory[i].index<<"  "<<memory[i].value<<" , ";
			if(i%10==0){
				cout<<"\n";
			}
		}
	}
	void printInstructionLine(int i){
            cout <<"\n"<< instructions[i].index<<"   " << instructions[i].command << "   " 
                    << instructions[i].A<< "   " << instructions[i].B <<"\n\n"<< endl;
	 }
	int StoreInstruction(int next){
            instruction temp = instructions[next];
            string com = temp.command;
            if( com == "SET" )
            {
                 return SET(temp.A,temp.B);
   
            }
            else if (com =="CPY"){
                return CPY(temp.A,temp.B);
            }
            else if (com =="CPYI"){
            	return CPYI(temp.A,temp.B);
            }
            else if (com =="ADD")
            {
                return ADD(temp.A,temp.B);
            }
            else if (com == "ADDI"){
                return ADDI(temp.A,temp.B);
            }
            else if (com == "SUBI"){
            	return SUBI(temp.A,temp.B);
            }
            else if ( com == "JIF"){
           		return JIF(temp.A,temp.B);
            
            }
            else if(com == "CPYI2"){
            	return CPYI2(temp.A,temp.B);
            }
            else if ( com == "HLT"){
            	return -1;
            }           
        }
 
	int  SET(int par1, int par2){
		memory[par2].value = par1;
		return -2;
	}

	int CPY(int par1, int par2){
		memory[par2].value = memory[par1].value;
		return -2;	
	}
	
	int CPYI(int par1,int par2){
		memory[par2].value = memory [memory[par1].value].value;
		return -2;	
	}
	
	int ADD(int par1,int par2)	{ 
		memory[par2].value += par1;
		return -2;	
	}

	int ADDI(int par1,int par2){ 
		memory[par2].value  = memory[par2].value + memory[par1].value;
		return -2;	
	}

	int SUBI(int par1,int par2){
		memory[par2].value  = memory[par1].value - memory[par2].value;
		return -2;	
	}
	int CPYI2(int par1,int par2){
		memory[memory[par2].value].value  = memory[par1].value ;
		return -2;
	}

	int JIF(int par1,int par2){
		
		if(memory[par1].value<=0 ){
			memory[0].value  = 	par2;
			return par2 ; 
		}
		else
			return -2;
	}


	void cpuRun(){
		int i = 0 ;
        int ReturnValue = 0;
        char temp;
	 	if(mode == 0)
	 	{
			while(1){

	        	//printInstructionLine(i);
	        	ReturnValue = StoreInstruction(i);
	        	if(ReturnValue == -2)
	        	{
	        		i++;	        		
	        	}
	        	else if(ReturnValue == -1 )
	        	{
	        		printMemory();	        		
	        		break;
	        	}
	        	else /* JIFSE*/
	        	{
	        		i = ReturnValue;
	        	}	
			}
	 	}
	 	else if(mode == 1){

            while(1)
            {
            	//printInstructionLine(i);
            	ReturnValue = StoreInstruction(i);

            	if(ReturnValue == -2)
            	{
            		i++;
            		printMemory();
            	}
            	else if(ReturnValue == -1 )
            	{
            		printMemory();
            		cout <<endl;
            		break;
            	}
            	else /* JIFSE*/
            	{
            		i = ReturnValue;
            		printMemory();
            	}	
            	cout <<"\n" << endl;
	        	cout << memory[28].value<<endl;

			}
	 	}
	 	else if (mode == 2){
	 		while(1)
            {
            	printInstructionLine(i);
            	ReturnValue = StoreInstruction(i);

            	if(ReturnValue == -2)
            	{
            		i++;
            		printMemory();
            	}
            	else if(ReturnValue == -1 )
            	{
            		printMemory();
            		cout <<endl;
            		break;
            	}
            	else /* JIFSE*/
            	{
            		i = ReturnValue;
            		printMemory();
            	}	

            	cout<<"\nPLEASE ENTER ONE LETTER \n";
            	cin>>temp;
			}
	 	}
	 }
};



int main(int argc, char *argv[]){

// here you should get the command line arguments and pass to the sim(string filename, int mode)

string filename;	
int mode;

filename = argv[1];/*pr.txt = argv[1]*/
mode = atoi(argv[3]);
Simulate sim(filename,mode);
sim.cpuRun();

return 0;
}


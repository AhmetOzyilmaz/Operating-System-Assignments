/*****************************************************************************************/
/*AHMET ÖZYILMAZ 111044014  HW02 */
/*	Memory de tutulan dosya isimleri ascii karşılıkları ile tutulmaktadır				*/
/* Tırnak işareti sorun olmasından dolayı  ^ işareti kullanılmıştır memoryde 		*/
/*****************************************************************************************/
/* SET fonksiyonu base register artı verilen index değerine yazmakta verilen ilk değeri */
/* exec ile eklenen yeni memory ve instructionlar vektör olarak tutulunca vektörün sonuna eklenir*/
/*****************************************************************************************/
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>
//you may need to add more functions
#define SIZE 1024
using namespace std;

// CPU GTU_C312



class mmry
{
public:
	mmry(){

	}
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
	instruction(){;}
	instruction(int i,string &s,int a,int b){
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

class call /* Her hangibir sistem call objesi */
{
public:
	call(){;}
	call(int i,string &s,string &a,int b=0){
		index=i;
		command=s;
		A=a;
		B=b;
	}
	~call(){}
	int index;
	string command;                  //like "ADD" ,"SET" ...
	string A;
	int B;

	
};

int NumberOfProcess=0;

class ProcessTable
{
	public:
	ProcessTable(string &s,int P=0,int PI=0,int PIP=0,int Strt=0,int U=0,int STat=0,int Mem=0,int TPMSA=0,int TPISA = 0){
		ProcessID=P;
		nameOfProcess = s;                  //like "ADD" ,"SET" ...
		PID = PI;
		parentPID = PIP;
		StartTheTickNumOfProcess = Strt;
		UsedTheTickNumOfProcess = U; 
		StateOfProcess =STat;/* for ready = 1 for bloced = 2  for running = 3*/
		MemoryLocationOnPhysical = Mem;/*The physical address of the memory location of the process*/

		ThisProMemStartAdress = TPMSA;
		ThisProInsStartAdress = TPISA;	
	}
	~ProcessTable(){}
	int ProcessID;
	string nameOfProcess;                  //like "ADD" ,"SET" ...
	int PID;
	int parentPID;
	int StartTheTickNumOfProcess;
	int UsedTheTickNumOfProcess;
	int StateOfProcess;/* for ready = 1 for bloced = 2  for running = 3*/
	int MemoryLocationOnPhysical;/*The physical address of the memory location of the process*/
	int ThisProMemStartAdress ;
	int ThisProInsStartAdress ;	
};

class Simulate
{
public:

	Simulate(string &filename,int md){
		BASEPID = 0;

		ParseMemoryCounter = 0 ;
		ParseInstructionCounter=0;
			
		FillVector();
	    parseFile(filename);
		mode=md;

	}
	~Simulate() {}

	vector<mmry> memory ;
	vector<mmry> CopyMemoryVector;
	vector<instruction> instructions;
	vector<instruction> CopyInstructionVector;

	string inputFileName;
	int BASEPID ;
	vector<call> CALLS;
	vector<call> CopyCALLSVector;
	vector<ProcessTable> ProcessTables;

	int ParseMemoryCounter ;
	int ParseInstructionCounter;

	int BaseRegister;
	int LimitRegister;
	int mode;
	void FillVector(){

		string nll ="NULL";

		for (int i = 0; i < SIZE; ++i)
		{
			mmry Cop1(i,0);
			CopyMemoryVector.push_back(Cop1);
		}

		for (int i = 0; i < SIZE/2; ++i)
		{
			instruction Cop2(i,nll,0,0);
			CopyInstructionVector.push_back(Cop2);
		}
	
		for (int i = 0; i < SIZE/10; ++i)
		{
			call Cop3(i,nll,nll,0);
			CopyCALLSVector.push_back(Cop3);
		}


	}
	void parseFile(string &filename)
	{

		string line;
		string  LineEnd;
	
		string beginRow = "Begin Data Section";
		string endRow = "End Data Section";
		char CHARfileName[10];

		BaseRegister = ParseMemoryCounter;
		LimitRegister=0;

 		
		memset(CHARfileName, 0, sizeof CHARfileName);
		strcpy(CHARfileName,filename.c_str());
		
	 	ifstream myfile(CHARfileName) ;

	 	int InstructionFlagStart=0,InstructionFlagEnd = 0 ,DataSectionFlagStart= 0,DataSectionFlagEnd = 0,LimitInsRegister = 0;

		CopyMemoryVector[ParseMemoryCounter].index=2;
		CopyMemoryVector[ParseMemoryCounter].value=ParseMemoryCounter;


 			//cout << "BaseRegister For memmory -> " << BaseRegister << endl;
		    //cout << "Instruction Start this process-> " << ParseInstructionCounter-LimitInsRegister << endl;

	 	while(myfile >> line) 
		{

			//cout << "Line ->"<<line  << endl;

			if(line[0]=='#')
			{
				getline(myfile,LineEnd);/* yorum kısımların sting e alındığı kısım */
				//cout << LineEnd << endl;
			}
			else if (line == "Begin")
			{

				myfile >> line;
 				//cout << line  << endl;

				if(line =="Data"){
					myfile >> line;
				 	//cout << line  << endl;
	
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
				int value  = 0;
				memset(temp, 0, sizeof temp);

				strcpy(temp,line.c_str());

				LimitRegister ++;
				myfile >> line;

				//	cout <<line << endl;
					if(line[0]== '^')
					{
						value = line[1];
						CopyMemoryVector[ParseMemoryCounter].index=ParseMemoryCounter;
						CopyMemoryVector[ParseMemoryCounter].value=value;

					}
					else
					{
						memset(temp, 0, sizeof temp);
						strcpy(temp,line.c_str());

						value =atoi(temp);			

						CopyMemoryVector[ParseMemoryCounter].index=ParseMemoryCounter;
						CopyMemoryVector[ParseMemoryCounter].value=value;
					}

				ParseMemoryCounter++;

			}
			else if (isdigit(line[0]) && InstructionFlagStart == 1 && InstructionFlagEnd == 0 && DataSectionFlagStart == 1 && DataSectionFlagEnd == 1 ){
				//cout <<"AAAAAAAAAAAAAAAAAAAA"<<endl;
				int VALUE = 0;	
				char temp1[sizeof(line)];
				strcpy(temp1,line.c_str());
				string s,CallType;
				myfile >> s;
				
				if(s == "HLT")/* This is HALT instuction */
				{
					CopyInstructionVector[ParseInstructionCounter].index=ParseInstructionCounter;
					CopyInstructionVector[ParseInstructionCounter].command=s;
					CopyInstructionVector[ParseInstructionCounter].A=0;
					CopyInstructionVector[ParseInstructionCounter].B=0;
				}
				else if(s == "CALL")
				{/* This is system call*/
					
		
					myfile >> CallType;			
					if(CallType !="FORK"){
						myfile >> line;
						strcpy(temp1,line.c_str());
						VALUE = atoi(temp1);
					}

					if(CallType == "PRN"){
						CopyInstructionVector[ParseInstructionCounter].index=ParseInstructionCounter;
						CopyInstructionVector[ParseInstructionCounter].command=s;
						CopyInstructionVector[ParseInstructionCounter].A=1;
						CopyInstructionVector[ParseInstructionCounter].B=VALUE;
						
						/*prn fonksiyonunun çağır */
						CopyCALLSVector[ParseInstructionCounter].index = ParseInstructionCounter ;
						CopyCALLSVector[ParseInstructionCounter].command = s;
						CopyCALLSVector[ParseInstructionCounter].A = CallType ;
						CopyCALLSVector[ParseInstructionCounter].B = VALUE ;

						//cout << CpyCall.index <<" " <<CpyCall.command<<" "<< CpyCall.A <<" "<<CpyCall.B << endl;

					}
					else if(CallType == "EXEC"){

						CopyInstructionVector[ParseInstructionCounter].index=ParseInstructionCounter;
						CopyInstructionVector[ParseInstructionCounter].command=s;
						CopyInstructionVector[ParseInstructionCounter].A=2;
						CopyInstructionVector[ParseInstructionCounter].B=VALUE;
						/*exec fonksiyonunu çağır*/

						CopyCALLSVector[ParseInstructionCounter].index = ParseInstructionCounter ;
						CopyCALLSVector[ParseInstructionCounter].command = s;
						CopyCALLSVector[ParseInstructionCounter].A = CallType ;
						CopyCALLSVector[ParseInstructionCounter].B = VALUE ;
						//cout << CpyCall.index <<" " <<CpyCall.command<<" "<< CpyCall.A <<" "<<CpyCall.B << endl;

					}
					else if (CallType=="FORK")	{
						/*fork fonksiyonunu çağır*/

						CopyInstructionVector[ParseInstructionCounter].index=ParseInstructionCounter;
						CopyInstructionVector[ParseInstructionCounter].command=s;
						CopyInstructionVector[ParseInstructionCounter].A=3;
						CopyInstructionVector[ParseInstructionCounter].B=0;
					
						CopyCALLSVector[ParseInstructionCounter].index = ParseInstructionCounter ;
						CopyCALLSVector[ParseInstructionCounter].command = s;
						CopyCALLSVector[ParseInstructionCounter].A = CallType ;
						CopyCALLSVector[ParseInstructionCounter].B = 0 ;
						//cout << CpyCall.index <<" " <<CpyCall.command<<" "<< CpyCall.A <<" "<<CpyCall.B << endl;

					}
				
				}
				else if(s!="HLT"){/* This is other instuction except  hlt*/
					//cout<< "  " << s;
					myfile >> line;
					strcpy(temp1,line.c_str());
					int a = atoi(temp1);
					//cout <<"  "<<a ;
					myfile >> line;
					strcpy(temp1,line.c_str());		
					int b = atoi(temp1);
					//cout <<endl<< "b ->"<<b <<"<-"<<endl;

					CopyInstructionVector[ParseInstructionCounter].index=ParseInstructionCounter;
					CopyInstructionVector[ParseInstructionCounter].command=s;
					CopyInstructionVector[ParseInstructionCounter].A=a;
					CopyInstructionVector[ParseInstructionCounter].B=b;
					//cout<<"  " << b <<"  "<<endl;
				}
			/*cout <<"CopyInstructionVector   ->"<<	CopyInstructionVector[ParseInstructionCounter].index<<
							CopyInstructionVector[ParseInstructionCounter].command<<
							CopyInstructionVector[ParseInstructionCounter].A<<
							CopyInstructionVector[ParseInstructionCounter].B<<endl;*/
					ParseInstructionCounter++;
					LimitInsRegister++;

			}
		}
		//cout << "BaseRegister " << BaseRegister << endl;
		//cout << "LimitRegister " << LimitRegister << endl;

		CopyMemoryVector[ParseMemoryCounter-LimitRegister+3].index =ParseMemoryCounter-LimitRegister+3  ;
		CopyMemoryVector[ParseMemoryCounter-LimitRegister+3].value=LimitRegister-1;

		//cout << " Limit CopyMemoryVector[ParseMemoryCounter-LimitRegister+3].index " << CopyMemoryVector[ParseMemoryCounter-LimitRegister+3].index << endl;
		//cout << "CopyMemoryVector[ParseMemoryCounter-LimitRegister+3].value " << CopyMemoryVector[ParseMemoryCounter-LimitRegister+3].value << endl;


		CopyMemoryVector[ParseMemoryCounter-LimitRegister+2].index = BaseRegister +2  ;
		CopyMemoryVector[ParseMemoryCounter-LimitRegister+2].value =  BaseRegister;
		//cout << " BASE CopyMemoryVector[ParseMemoryCounter-LimitRegister+2].index " <<CopyMemoryVector[ParseMemoryCounter-LimitRegister+2].index << endl;
		//cout << "CopyMemoryVector[ParseMemoryCounter-LimitRegister+2].value " <<CopyMemoryVector[ParseMemoryCounter-LimitRegister+2].value << endl;
		int check = FindEmthyMemoryInMemory(ParseMemoryCounter);
		//printMemory();

	    if(check!=-1)
	    {
			memory= CopyMemoryVector; 
			instructions=CopyInstructionVector; 
			CALLS = CopyCALLSVector; 
			NumberOfProcess++;
		    AddFirstProcessTable(filename,BaseRegister,ParseInstructionCounter-LimitInsRegister);
	    
		  

	    }

	    else
	    {
	    	cout << " NOT available Memory Space" << endl;
	    	exit(1);
	    }
 
 	    myfile.close();

  
		    //cout <<"NumberOfMemoryAdress  " << NumberOfMemoryAdress << endl; 
}
	int AddFirstProcessTable(string &s,int num1,int num2){
		BASEPID++;
		ProcessTable TempProcessTable(s,NumberOfProcess,BASEPID+1024,0,0,0,1,60,num1,num2);


		/*ThisProMemStartAdress = TPMSA;
		ThisProInsStartAdress = TPISA;*/


		ProcessTables.push_back(TempProcessTable);
		return 0;
	}
	void printInstructions(){
		for (int i = 0; i < 20; ++i)
		{
			cout<<instructions[i].index  <<"  "<<instructions[i].command<<" , " << instructions[i].A<<" , " <<  instructions[i].B<<" , " << endl;
		}
	}
	void printMemory(){
		//cout << memory.size() << endl;
		for (int i = 0; i < memory.size(); ++i)
		{
			cout<<memory[i].index  <<"  "<<memory[i].value<<" , ";
			if(i%10==0){
				cout<<"\n";
			}
		}
	}
	void printInstructionLine(int i){
            cout <<"\n"<< instructions[i].index<<"   " << instructions[i].command << "   " 
                    << instructions[i].A<< "   " << instructions[i].B <<"\n\n"<< endl;
	 }
	 void printProcessTable(){

	 		cout << "\t\t\t\t\tPROCESS INFORMATION TABLE " << endl;
	 			cout << " ProcessID" <<"\t" << "nameOfProcess\t\t" << "PID\t";
	 			cout <<  " parentPID" <<"\t" << "StartTheTickNumOfProcess";
	 			cout <<  " UsedTheTickNumOfProcess" <<"\t" <<" StateOfProcess" ;
	 			cout <<  " MemoryLocationOnPhysical"<<"\t" <<  " ThisProMemStartAdress"<<"\t" <<"\t" <<  " ThisProInsStartAdress" << endl;

	 		for (int i = 0; i < ProcessTables.size(); ++i)
	 		{
	 			cout <<" \t"<< ProcessTables[i].ProcessID <<"\t" <<ProcessTables[i].nameOfProcess << "\t\t\t"<<ProcessTables[i].PID ;
	 			cout <<  "\t"<< ProcessTables[i].parentPID <<"\t" << ProcessTables[i].StartTheTickNumOfProcess <<"\t\t\t\t\t";
	 			cout <<   ProcessTables[i].UsedTheTickNumOfProcess <<"\t\t" << ProcessTables[i].StateOfProcess<<"\t\t\t"  ;
	 			cout <<   ProcessTables[i].MemoryLocationOnPhysical <<"\t\t" <<  ProcessTables[i].ThisProMemStartAdress <<"\t\t" <<   ProcessTables[i].ThisProInsStartAdress << endl;
			}
	 }
	int StoreInstruction(int ProId){
			/* n ci process table çağrılacak */

			int next = ProcessTables[ProId].ThisProInsStartAdress +ProcessTables[ProId].UsedTheTickNumOfProcess;
			int returnValue = 0;
            instruction temp = instructions[next];
            string com = temp.command;

            if( com == "SET" )
            {
                returnValue =SET(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
   
            }
            else if (com =="CPY"){

               returnValue = CPY(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            }
            else if (com =="CPYI"){

            	returnValue =CPYI(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            }
            else if (com =="ADD")
            {

               returnValue =ADD(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            }
            else if (com == "ADDI"){
               returnValue = ADDI(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            }
            else if (com == "SUBI"){
            	returnValue =SUBI(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            }
            else if ( com == "JIF"){
           		returnValue =JIF(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            
            }
            else if(com == "CPYI2"){
            	returnValue =CPYI2(temp.A+ProcessTables[ProId].ThisProMemStartAdress,temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            }
            else if(com == "CALL"){

            	if(temp.A==1)
            	{
            		returnValue =PRN(temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            	}
            	else if(temp.A==3)
            	{
            		returnValue =FORK();
            	}
            	else if(temp.A==2){
            		returnValue =EXEC(temp.B+ProcessTables[ProId].ThisProMemStartAdress);
            	}

            }
            else if ( com == "HLT"){
            	returnValue = -1;
            } 


            ProcessTables[ProId].UsedTheTickNumOfProcess++;

            return returnValue;        
      }
    /* CALL FONKSİYONLARININ OLDUĞU KISIM*/

 	int FORK()
 	{





 		return -2;
 	}
 	int PRN(int Address)
 	{
 		cout<<endl<<"Contents of memory address " <<Address<< "Value Of -> "<<memory[Address].value<< endl;
 		return -2;
 	}
 	int EXEC(int Address)
 	{
 		int i=0;
 		int End = Address;
 		char NewFileName[50];
 		while(1)
 		{
 			//cout <<" EXEC -> " <<End<< endl;
 			if(memory[End].value==0){
 				break;
 			}
 			NewFileName[i]=(char)memory[End].value;
 			++i;
 			End++;
 		}
 		NewFileName[i]='\0';
 		string File;
 		inputFileName = NewFileName ;

 		cout << "EXEC komutu ile dosyadan  memory Alınmış program ismi  ->" <<NewFileName <<endl;/*Process in yükleneceği dosya ismi bu arrayde tutulu*/

		parseFile(inputFileName);

		//FindEmthyMemoryInMemory(10) ;
 		
 		return -2;
 	}
 	int FindEmthyMemoryInMemory(int MinSizeMemory)
 	{
 		int counter = 0;

		for (int j = 60; j < CopyMemoryVector.size(); ++j)
		{
			//cout <<" 1 j "<< j <<" 1 CopyMemoryVector  "<< CopyMemoryVector.size() << endl;
			if(CopyMemoryVector[j].value == 0)
			{
				counter++;
			}
			else { counter = 0;	}

			if(counter == MinSizeMemory){
				return j-counter+1;
			}
		}
		return -1 ; /* memoryde yer yoksa program çalısmaz*/
	}

 	/* İNSTURCTİON SETLERİNİN OLDUĞU KISIM */
	int  SET(int par1, int par2){
		memory[par2+memory[2].value].value = par1;/*Base register eklenmiş hali ile yapılır.*/
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
		
		if(memory[par1].value <=0 ){
			memory[0].value  = 	par2;
			return par2 ; 
		}
		else
			return -2;
	}

	int Schedular()/* For round robin algorithm*/
	{
		int i=0, flag = 0 , ReturnValue = 0 , temp;
		int StartProcess = 0,AddThickCounter = 0;

		while(1){
		 	//printInstructionLine(i);
        	ReturnValue = StoreInstruction(StartProcess);
			AddThickCounter++;
			 	if(mode == 0)
	 			{

					if(ReturnValue == -2)
            		{
            		}
				    else if(ReturnValue == -1  || ReturnValue == 0)
					{
						printMemory();	
						printProcessTable();		
					    exit(0);
					}

					else{ i = ReturnValue; }
	 			}
				else if(mode == 1){

					if(ReturnValue == -2)
            		{
	            		printMemory();
            		}
            		 if(ReturnValue == -1  || ReturnValue == 0)
            		{
	            		printMemory();
	            		printProcessTable();
	            		exit(0);
            		}

	        		else{ 	i = ReturnValue;  	}		
				}
			else if (mode == 2){

				if(ReturnValue == -2)
            	{
            		printMemory();
            	}
            	else if(ReturnValue == -1  || ReturnValue == 0)
            	{
            		printMemory();
            		cout <<endl;
            		printProcessTable();

            		exit(0);
            		
            	}

	        	else{
	        		i = ReturnValue;
	        	}	
            	cout<<"\nPLEASE ENTER ONE LETTER \n";
            	cin>>temp;

			}

		 	else if (mode == 3) {  printProcessTable(); }

			if(i % 5 )/*Her 5 tıkta bir  */
			{
				StartProcess++;/*bir sonraki process geçecek */


			}

			if(StartProcess>=ProcessTables.size()){
				StartProcess-=ProcessTables.size();
			}
		}
		return -4;
	}
	void cpuRun(){
		Schedular();
	 }
};



int main(int argc, char *argv[]){

// here you should get the command line arguments and pass to the sim(string filename, int mode)

string filename;	
int mode;

if(argc !=4)
{
	cout << "ERROR USAGE Ex : ./Simulate pr.txt -D 0"<<endl;
	return -1;
}

filename = argv[1];/*pr.txt = argv[1]*/
char choise;
cout << "\t\t\tPROGRAMIMIZ EXEC comuutunun alacağı program  ismini ^S^ ^ e^   ... olarak kaydetmiştir çalıştırmak için \n  Bir giriş yapın" << endl;
cout << "\t\t\t100  Den geriye sayma programı prnNumbers.txt içindedir ." << endl;
cout <<"\t\t\t 1-10 Arasındaki sayıların toplarmı addNumbers.txt içindedir " << endl;
cout <<"\t\t\t 800 -100 kadar olan sayıların ekrana basımı prnBigNumbers.txt içindedir " << endl;

cin >> choise;
mode = atoi(argv[3]);
Simulate sim(filename,mode);
sim.cpuRun();

return 0;
}


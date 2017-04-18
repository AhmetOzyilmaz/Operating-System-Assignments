/*********************************************************************/
/*						AHMET ÖZYILMAZ 111044014 HW3	             */
/*********************************************************************/

/* Harddisk Sort edilecek dosyadaki bütün memory elemanlarını kendine kaydeder*/
/*

• If an instruction does not find its operand in memory, it causes a page fault
• Your programs will use virtual addresses, so each address needs to be translated



You will use second chance FIFO method for the page replacement algoritm
*/


#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <math.h>
#include <time.h>       /* time */

using namespace std;

// CPU GTU_C312
#define RAM 512  //	It has 2^9 integers of physical memory to store the data
#define DISKSPACE 1048576       //It has 2^20 integers of disk space for pages.
#define PAGESIZE 64 //The page size will be 2^6 integers
#define VIRTUALMEMORYSIZE 32768  // The virtual memory size is 2^15
#define PAGES 16384
#define NumberOfElement 2000
#define StartMEmoryADress 60
#define SizeRamFrame 8
class mmry
{
public:
	mmry(int i,int v){
		index=i;
		value=v;
	}
	mmry(){
		index=0;
		value=0;
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
class VirtualMemory
{
	public:
		VirtualMemory(int F ,int P){
			FrameNumber = F ;
			PageIndex = P ;
		}
		
		~VirtualMemory(){}
	
	int FrameNumber;/* Kaçıncı page in kaçıncı elemanı onu tutan bir class */
	int PageIndex;
};

/*Her page 64 integer tutabilir */
class Page{
	public:
		Page(){
			 ModifiedBit=0;
			 ReferencedBit=0;
			 PresentAbsentBit=0;
			 PageFrameNumber=0;
			  for (int i = 0; i < PAGESIZE; ++i){ Numbers[i] = 0 ;}
		}
		Page(int m,int r,int Pa,int pgn,int num[PAGESIZE]){
			 ModifiedBit=m;
			 ReferencedBit=r;
			 PresentAbsentBit=Pa;
			 PageFrameNumber=pgn;

			 for (int i = 0; i < PAGESIZE; ++i){  Numbers[i] = num[i];  }
		}

		int ModifiedBit;
		int ReferencedBit;/*R=0 ise bu sayfa hem eski hemde kullanılmayan bir sayfa
								olduğu için çıkartılabilir. Eğer R biti 1 ise bu bit 0 lanır ve
								bu sayfa listenin sonuna konulur ve yüklenme zamanı
								yeniymiş gibi düşünülerek güncellenir.*/
		int PresentAbsentBit;
		int PageFrameNumber;
		int Numbers[PAGESIZE];
};

/*• The page table will hold the following information for each page
◦ Modified bit
◦ Referenced bit
◦ Present/absent
◦ Page frame number
◦ Any other information needed by your system*/

class Simulate
{
	public:
	Simulate(string &filename,int md){
		
		HardDisk = (mmry*)malloc(sizeof(mmry)* DISKSPACE);
		mode=md;
		parseFile(filename);
		AllPages = (Page*)malloc(sizeof(Page)* PAGES);
		OurRam = (Page*)malloc(sizeof(Page)* SizeRamFrame);
		OurVirtualMemory = (VirtualMemory*)malloc(sizeof(VirtualMemory)* NumberOfElement);/*program için kullanılan sanal memory*/
		counter = 0;
	}
	virtual ~Simulate() {}

	VirtualMemory* OurVirtualMemory; /*Dosyamız 2000 lik integer içerdiği için 2000 lik bir virtual memory oluştu */
	Page* OurRam;/*512 integer alabilen 8 framelik bir rammımız var */
	Page* AllPages;
	//vector<mmry> HardDisk;
	mmry* HardDisk; /*memory objelerinden oluşan harddisk*/
	int counter ;
	int HardDiskIndex ;/*size of out all data s*/

	vector<instruction> instructions;/*keep all instructiopnss */
	int mode;/*Debug mode */
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

	 	HardDiskIndex = 0;

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
			//	HardDisk.push_back(mem);
				//cout <<Adress <<"  " << value<< endl;
				HardDisk[HardDiskIndex] = mem ;
				HardDiskIndex++;
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

				//printInstructionLine(0);
				//cout << index << "  " << s << endl;
			}
			
		}
		    myfile.close();

		}


	 /*Bu fonksiyon 2000 tane sayıyı pagelere ayırır yaklaşık 2 ^ 14 page imiz var */
	void CutAndFillPages(){
		int PageNumber = 0;
		int Temp[PAGESIZE];

		int counter = NumberOfElement;
		int i = 0;

		i+=StartMEmoryADress;
		//cout << NumberOfElement;

		for (int i = 60; i < NumberOfElement + 60 ; ++i)
		{
			//cout << i << "  " << HardDisk[i].value << endl;
			
			for (int j = 0; j < PAGESIZE; ++j)
			{
				/* code */
				//cout << j << endl;
				Temp[j] = HardDisk[i].value;
				i++;

			}

			Page pg(0,0,0,PageNumber,Temp);
			AllPages[PageNumber] =pg;
			PageNumber++;

		}

	 }

	 /*Eğer page fault varsa return değeri olarak -1 döndürüd yoksa 0  döndürür*/
	int  ControlPageFaultInRam(int inp1)
	{

		for (int i = 0; i < SizeRamFrame; ++i)
		{
			/* code */
				if(OurRam[i].PageFrameNumber == inp1  )
				{
					return 0 ;
					break;
				}
		}

		//OurRam
		return -1;
	}

	void FillOurVirtualMemory()
	{
		int i ;
		int temp = 0 ;
		
		for (i = 0; i < NumberOfElement  ;)
		{
			//cout << i << "  " << HardDisk[i].value << endl;
			
			for (int j = 0; j < PAGESIZE; ++j)
			{
				/* code */
				//cout << temp << endl;
				OurVirtualMemory[i].FrameNumber = temp;
				OurVirtualMemory[i].PageIndex = j;
				i++;
				
				if(i == NumberOfElement)
					break;
			}
			//cout <<"DEBUG"<<  endl;

			temp++;
			
		}

		//cout <<  i << endl;


/*
	   	for (int i = 0; i < NumberOfElement; ++i)
		{
			cout <<  "OurVirtualMemory  " << OurVirtualMemory[i].FrameNumber << "   "	<<OurVirtualMemory[i].PageIndex   << endl ;
		}*/
	 }

	void FillRam(){

		for (int i = 0; i < SizeRamFrame; ++i)
		{

			OurRam[i] = AllPages[i];
		
		}
	}
	void PrintRam(){
			for (int i = 0; i <SizeRamFrame; ++i)
			{
				for (int j = 0; j < PAGESIZE; ++j)
				{
					cout <<"i =>"<< i << "  Frame Number " << j << "Value "<<  OurRam[i].Numbers[j]<< endl;	
				}

			}
	}
	void printMemory(){
		for (int i = 0; i < HardDiskIndex; ++i)
		{
			cout<<HardDisk[i].index<<"  "<<HardDisk[i].value<<" , ";
			if(i%10==0){
				cout<<"\n";
			}
		}

	}
	void printPagetable()
	{
		int temp = 0;

		/*ramde yapılmış değişiklikler diske tekrar yazılır ve ekrana basılır */

		for (int i = 0; i < SizeRamFrame; ++i)
		{
			AllPages[OurRam[i].PageFrameNumber]  = OurRam[i];
		}

		cout <<"\n*********************************************************************************************************************"<< endl;
		cout << "\t\t\t\t\t\t\tPAGE TABLE " << endl;
		cout <<"***********************************************************************************************************************"<< endl;
		for (int i = 0; i < NumberOfElement; ++i)
		{
			for (int j = 0; j < PAGESIZE; ++j)
			{
				cout << "\tAdress of "  <<(OurVirtualMemory[i].FrameNumber * 64 ) + OurVirtualMemory[i].PageIndex   << "\tReferenced bit " << AllPages[i%64].ReferencedBit 
				<< "\tModifiedBit "  << AllPages[i%64].ModifiedBit  << "\tPresentAbsentBit "   << AllPages[temp].PresentAbsentBit  <<"\tAdress Value -> "<< AllPages[temp].Numbers[j]<< endl;
				i++;
				if(i== NumberOfElement )
					break;
			}
			temp++;
		}
	}
	void printInstructionLine(int i){
            cout <<"\n"<< instructions[i].index<<"   " << instructions[i].command << "   " 
                    << instructions[i].A<< "   " << instructions[i].B <<"\n\n"<< endl;
	 }
	int StoreInstruction(int next){

		/*önce page fault varmı yokmu onun kontrolu yapılacak */
           /*page fault varsa second chance page replacement algoritmasına göre page ram e alınacak ve */
		/*ondan sonra yer değiştirme işlemi yapılacak */

			/*30 31 adresler sayıları indexlerini tutan adres eğer index ramde yoksa page fault alması gerek
			*/


            instruction temp = instructions[next];
            string com = temp.command;
            int Num = HardDisk[30].value;
            int frmNum = 0;

        	while(1)
            {
           		frmNum++;
           		Num-= 64 ;
           		if(Num < 64 )
            		break;
            }
				
            int control1 = ControlPageFaultInRam(frmNum);

           if(control1 == -1 )
            {
            	//cout << " This Page Not in the Ram ->  " << frmNum+1 << endl;
          			 	
          		Page CopyPage ;
	            int flag =0 ;
	            CopyPage = AllPages[frmNum];

	            for (int i = 0; i < SizeRamFrame; ++i)
	            {
	            	if(OurRam[i].ReferencedBit == 0)/*reference biti 0  ramden kaldırılıp yerine yenisi konur ve r biti 1 yapılır*/
	            	{
	            		flag = 1;
	 					AllPages[frmNum] = OurRam[i];
	 					OurRam[i] = CopyPage;
	 					OurRam[i].ReferencedBit = 1;/*diskten yazıldığı için ram e */
	 			//		cout << "CHANCGEd  -> i " << i  << endl;
	 					if(mode == 3){
		 					cout << "\tThis page replacem with -> " << i  <<"\tThis Page ->" << frmNum << endl;
		 					printPagetable();
	 					}

	 				break;
	            	}	            	
	            }
	            if(flag == 0){/*remdeki bütün veriler reference edilmiş demek rasgele bir tanesi yer d
	          		  eğiştirilip olması gereken page ile değiştirilcek ve diske yazılan page 0 yapılıcak */
	        		   
					 int iSecret, iGuess;

					  /* initialize random seed: */
					  srand (time(NULL));

					  /* generate secret number between 1 and 10: */
					  iSecret = rand() % 8;

	        		    AllPages[frmNum] = OurRam[iSecret];
	 					OurRam[iSecret] = CopyPage;
	 					OurRam[iSecret].ReferencedBit = 0;/*diske tekrar yazıldıgı kısım */

	 					if(mode == 3){
		 					cout << "\tThis page replacem with -> " << iSecret  <<"\tThis Page ->" << frmNum << endl;
		 					printPagetable();
	 					}
	            }

	            flag == 0;
            }

            if ( com == "HLT"){
            	return -1;
            }           
            else if( com == "SET" )
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
            	/*sayıların replace edildiği kısım */
            	OurRam[frmNum].ModifiedBit = 1;
            	return CPYI2(temp.A,temp.B);
            }
       
        }
 
	int  SET(int par1, int par2){
		HardDisk[par2].value = par1;
		return -2;
	}

	int CPY(int par1, int par2){

		//cout << "  par1 " << HardDisk[par1].value <<  endl;

		HardDisk[par2].value = HardDisk[par1].value;
		return -2;	
	}
	
	int CPYI(int par1,int par2){
		HardDisk[par2].value = HardDisk [HardDisk[par1].value].value;
		return -2;	
	}
	
	int ADD(int par1,int par2)	{ 
		HardDisk[par2].value += par1;
		return -2;	
	}

	int ADDI(int par1,int par2){ 
		HardDisk[par2].value  = HardDisk[par2].value + HardDisk[par1].value;
		return -2;	
	}

	int SUBI(int par1,int par2){
		HardDisk[par2].value  = HardDisk[par1].value - HardDisk[par2].value;
		return -2;	
	}
	int CPYI2(int par1,int par2){
		HardDisk[HardDisk[par2].value].value  = HardDisk[par1].value ;
		return -2;
	}

	int JIF(int par1,int par2){
		
		if(HardDisk[par1].value<=0 ){
			HardDisk[0].value  = par2;
			return par2 ; 
		}
		else
			return -2;
	}


	void cpuRun(){
//		printMemory();
	CutAndFillPages();

	FillRam();
	//PrintRam();
	FillOurVirtualMemory();
		int i = 0 ;
        int ReturnValue = 0;
        char temp;

		if(mode == 1)
	 	{
				 	/*	will be run and the contents of the memory will be
						displayed after the CPU halts.*/

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
	        	    printPagetable();
	        		exit(0);
	        	}
	        	else /* JIFSE*/
	        	{
	        		i = ReturnValue;
	        	}	
			}

	 	}
	 	else if(mode == 0)
	 	{
				/*
				the
				contents of the memory and the page table will be printed to the screen after each CPU
				tick (memory address and the content for each adress).
				*/
            while(1)
            {
            	//printInstructionLine(i);
            	ReturnValue = StoreInstruction(i);
	        	//cout <<"ReturnValue " <<ReturnValue << endl;

            	if(ReturnValue == -2)
            	{
            		i++;

            		//printMemory();
	        	   // printPagetable();

            	}
            	else if(ReturnValue == -1 )
            	{
            		printMemory();
            		cout <<endl;
					exit(0);
            	}
            	else /* JIFSE*/
            	{
            		i = ReturnValue;
            		//printMemory();
            	}	
            	//cout <<"\n" << endl;

			}
	 	}
	 	else if (mode == 2){

				 	/*	after each CPU tick, the contents of the memory and the page table
			will be displayed. Your simulation will wait for an entry from the keyboard and it will
			continue for the next tick*/
	 		while(1)
            {
            	printInstructionLine(i);
            	ReturnValue = StoreInstruction(i);

            	if(ReturnValue == -2)
            	{
            		i++;
            		printMemory();
	        	   printPagetable();

            	}
            	else if(ReturnValue == -1 )
            	{
            		printMemory();
            		cout <<endl;
					exit(0);
            	}
            	else /* JIFSE*/
            	{
            		i = ReturnValue;
            		printMemory();
            		printPagetable();

            	}	

            	cout<<"\nPLEASE ENTER ONE LETTER \n";
            	cin>>temp;
			}
	 	}
	 	else if(mode == 3)
	 	{

	 		while(1)
            {
            	//printInstructionLine(i);
            	ReturnValue = StoreInstruction(i);

            	if(ReturnValue == -2)
            	{
            		i++;
            		//printMemory();
            	}
            	else if(ReturnValue == -1 )
            	{
            		//printMemory();
            		//cout <<endl;
            		break;
            	}
            	else /* JIFSE*/
            	{
            		i = ReturnValue;
            		//printMemory();
            	}	
			}

	 	}

		free(AllPages);
		free(HardDisk);
		//free(OurRam);	
		//free(OurVirtualMemory);
		exit(0);
	}
};



int main(int argc, char *argv[]){

// here you should get the command line arguments and pass to the sim(string filename, int mode)

	int mode;
	char con;

	if(argc != 3) 
	{
		cout << "Usage ./Simulate NewSort.txt 1" << endl ;
		cout <<"Enter a character  for countinue" << endl;
		cin >>con;
		cout << endl;
		exit(0);
	}
	else if(argc == 3)
	{
		cout << "Program  Uzun sürüyor Sonsuz döngüye girmedi ./Simulate NewSort.txt 1 >> Output.txt ile dosyadan inceleyebilirsiniz " << endl;
		string filename(argv[1]);
		mode = atoi(argv[2]);

		Simulate sim(filename,mode);
		sim.cpuRun();
	}
	else
		cout << "ERROR "<< endl;


return 0;
}


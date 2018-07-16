#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <deque>

using namespace std;

// Global Variables Declaration 
// Output Variables
deque<string> INM;
string INB;
string AIB;
string SIB;
string PRB;
string ADB;
deque<string> REB;
ofstream Sim;
int Step = 0;
int RMap[16];         //mapping presence of Registers
int MMap[16];		//   "      "        "  Memory values
int GPRs[16];
int Memory[16];
string opcode;
string op;
// Processing Variables

deque<string> Deb;
deque<int> Debu;

// Functions Declaration

int Splitter(std::string x)
{
	int f = x.find(",");
	return f;
}
void Read()
{
	stringstream temp;
	int src1, src2;
	if (Deb[0] != "ST")
	{
		if (RMap[Debu[1]] == 1 && RMap[Debu[2]] == 1)
		{
			src1 = GPRs[Debu[1]];
			src2 = GPRs[Debu[2]];
		}
	}
	else
	{
		src1 = GPRs[Debu[1]];
		src2 = Debu[2];
	}
	temp << "<" << Deb[0] << ",R" << Debu[0] << "," << src1 << "," << src2 << ">";
	Debu.clear();
	INB = temp.str();
}
void PrintFunction()
{
	stringstream temp1;
	string temp2;
	Sim << "STEP " << Step << ":" << endl;
	Sim << "INM:";
	for (int i = 0; i < INM.size(); i++)
	{
		Sim << INM[i];
		if (i < (INM.size() - 2))
		{
			Sim << ",";
		}
	}
	Sim << endl;
	Sim << "INB:" << INB << endl;
	Sim << "AIB:" << AIB << endl;
	Sim << "SIB:" << SIB << endl;
	Sim << "PRB:" << PRB << endl;
	Sim << "ADB:" << ADB << endl;
	Sim << "REB:";
	for (int i = 0; i < REB.size(); i++)
	{
		Sim << REB[i];
		if (i < (REB.size() - 2))
		{
			Sim << ",";
		}
	}
	Sim << endl;
	Sim << "RGF:";
	for (int i = 0; i < 16; i++)
	{
		if (RMap[i] == 1)
		{
			temp1 << "<R" << i << "," << GPRs[i] << ">,";
		}
	}
	temp2 = temp1.str();
	temp1.str("");
	temp2.erase(temp2.end() - 1);
	Sim << temp2 << endl;
	temp2.clear();
	Sim << "DAM:";
	for (int i = 0; i < 16; i++)
	{
		if (MMap[i] == 1)
		{
			temp1 << "<" << i << "," << Memory[i] << ">,";
		}
	}
	temp2 = temp1.str();
	temp1.str("");
	temp2.erase(temp2.end() - 1);
	Sim << temp2;
	if (!INM.empty() || !REB.empty())
	{
		Sim << endl << endl;
	}
	temp2.clear();
	Step++;
}
void Initialization()
{
	ifstream Instruct;
	ifstream Registers;
	ifstream Datamem;
	string Buffer;
	string temp;
	int MemAddr = 0;
	int	Memvalue = 0;
	for (int i = 0; i < 16; i++)
	{
		RMap[i] = 0;
		MMap[i] = 0;
		GPRs[i] = 0;
		Memory[i] = 0;

	}
	Instruct.open("instructions.txt", std::ifstream::in);
	while (!Instruct.eof())
	{
		getline(Instruct, Buffer);
		if(!Buffer.empty())
		{
			INM.push_back(Buffer);
		}
	}
	Instruct.close();
	Registers.open("registers.txt", std::ifstream::in);
	while (!Registers.eof())
	{
		getline(Registers, Buffer);
		if (!Buffer.empty())
		{
			int found = Buffer.find(",");
			temp = Buffer.substr(2, found - 2);
			MemAddr = std::stoi(temp);
			temp = Buffer.substr(found + 1);
			Memvalue = std::stoi(temp);
			GPRs[MemAddr] = Memvalue;
			RMap[MemAddr] = 1;
		}
	}
	Registers.close();
	Datamem.open("datamemory.txt", std::ifstream::in);
	while (!Datamem.eof())
	{
		getline(Datamem, Buffer);
		if (!Buffer.empty())
		{
			int found = Buffer.find(",");
			temp = Buffer.substr(1, found - 1);
			MemAddr = stoi(temp);
			temp = Buffer.substr(found + 1);
			Memvalue = stoi(temp);
			Memory[MemAddr] = Memvalue;
			MMap[MemAddr] = 1;
		}
	}
	Datamem.close();
	PrintFunction();
}
void Decode()
{
	string temp;
	int dest;
	string buff;
	int src1;
	int src2;
	int find;
	buff = INM.front();
	INM.pop_front();
	find = Splitter(buff);
	opcode = buff.substr(1,find-1);
	buff = buff.substr(find + 1);
	find = Splitter(buff);
	dest = stoi(buff.substr(1, find-1));
	temp = buff.substr(find + 1);
	find = Splitter(temp);
	buff = temp.substr(1, find - 1);
	src1 = stoi(buff);
	temp = temp.substr(find + 1);
	if (opcode != "ST")
	{
		src2 = stoi(temp.substr(1));
	}
	else
	{
		src2 = stoi(temp);
	}
	Deb.push_back(opcode);
	Debu.push_back(dest);
	Debu.push_back(src1);
	Debu.push_back(src2);
	Read();
}
void Issue1()
{
	op = Deb[0];
	AIB = INB;
	INB.clear();
	Deb.pop_front();
}
void Issue2()
{
	SIB = INB;
	INB.clear();
	Deb.pop_front();
}
void ASU()
{ 
	int find;
	int result;
	stringstream temp;
	string func = op;
	op.clear();
	find = Splitter(AIB);
	AIB= AIB.substr(find+1);
	find = Splitter(AIB);
	string dest = AIB.substr(0, find);
	AIB = AIB.substr(find+1);
	find = Splitter(AIB);
	int src1 = stoi(AIB.substr(0,find));
	int src2 = stoi(AIB.substr(find + 1));
	if (func == "ADD")
	{
		result = src1 + src2;
	}
	else
	{
		result = src1 - src2;
	}
	temp << "<" << dest << "," << result << ">";
	REB.push_back(temp.str());
	temp.str("");
	AIB.clear();
}
void MLU1()
{
	PRB = AIB;
	AIB.clear();
	op.clear();
}
void MLU2()
{
	int find;
	int result;
	stringstream temp;
	find = Splitter(PRB);
	string func = PRB.substr(1,find-1);
	PRB = PRB.substr(find + 1);
	find = Splitter(PRB);
	string dest = PRB.substr(0, find);
	PRB = PRB.substr(find + 1);
	find = Splitter(PRB);
	int src1 = stoi(PRB.substr(0, find));
	int src2 = stoi(PRB.substr(find + 1));
	result = src1*src2;
	temp << "<" << dest << "," << result << ">";
	REB.push_back(temp.str());
	temp.str("");
	PRB.clear();
}
void ADDR()
{
	int find;
	int result;
	stringstream temp;
	find = Splitter(SIB);
	SIB = SIB.substr(find + 1);
	find = Splitter(SIB);
	string dest = SIB.substr(0, find);
	SIB = SIB.substr(find + 1);
	find = Splitter(SIB);
	int src1 = stoi(SIB.substr(0, find));
	int src2 = stoi(SIB.substr(find + 1));
	result = src1 + src2;
	temp << "<" << dest << "," << result << ">";
	ADB=temp.str();
	temp.str("");
	SIB.clear();
}
void Store()
{	
	int find = Splitter(ADB);
	int src = stoi(ADB.substr(2, find - 2));
	int dest =stoi(ADB.substr(find + 1));
	MMap[dest] = 1;
	Memory[dest] = GPRs[src];
	ADB.clear();
}
void Write()
{
	int find = Splitter(REB[0]);
	int dest = stoi(REB[0].substr(2, find - 2));
	int result = stoi(REB[0].substr(find + 1));
	RMap[dest] = 1;
	GPRs[dest] = result;
	REB.pop_front();
}

// Main Function
int main()
{
	Sim.open("simulation.txt", std::ofstream::out);
	Initialization();
	while (!INM.empty() || !INB.empty() ||!AIB.empty() ||!SIB.empty() ||!PRB.empty() ||!ADB.empty() ||!REB.empty())
	{
		if (!REB.empty())
		{
			Write();
		}
		if (!ADB.empty())
		{
			Store();
		}
		if (!SIB.empty())
		{
			ADDR();
		}
		if (!PRB.empty())
		{
			MLU2();
		}
		if (!AIB.empty())
		{
			if (op != "MUL")
			{
				ASU();
			}
			else
			{
				MLU1();
			}
		}
		if (!INB.empty())
		{
			if (Deb[0] != "ST")
			{
				Issue1();
			}
			else
			{
				Issue2();
			}
		}
		if (!INM.empty())
		{
			Decode();
		}
		PrintFunction();
	}
	return 0;
}

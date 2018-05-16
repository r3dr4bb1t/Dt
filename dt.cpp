#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>	
#include <fstream>
#include <string>
#include <sstream>

#pragma warning(disable:4996)

using namespace std;

//struct definition for tree

typedef struct Node {
	vector<struct Child> children;
	 int attrnum;
} Node;


typedef struct Child {
	string classvalue;
	string value;
	struct Node* p;
} Child;

typedef struct {
	vector<vector<string>> tuple;
	vector<string> attribute;
	int labelnum;
} Table;


typedef struct {
	string classvalue;
	int count;
} freq;

typedef struct {
	string attributevalue;
	vector<freq> freqcount;
} ResultOfAttr;


typedef struct {
	vector<vector<string>*> tupleleft;
	vector<freq> freqcount;
} ResultOfChild;


double CalInfoD(const vector<int>* i_buycomputer) 
{
	double samples = 0.0;
	double Info_D = 0.0;
	//Calculate num of sample
	for (int i = 0; i < i_buycomputer->size(); i++)
	{
		samples= samples + i_buycomputer->at(i);
	}
	//means "age < = 30 " has 5 out of samples with 2yess and 3 nos
	for (int i = 0; i < i_buycomputer->size(); i++) 
    {  
		if (i_buycomputer->at(i) == 0) 
        {
			continue;
		}
			Info_D += (-1 * i_buycomputer->at(i) / samples) * log2(i_buycomputer->at(i) / samples);
	}

	return Info_D;
}

//calculate gain by Info(D) - Info(A)
double Calgain(const int attrnum, const int classLableIndex, const vector<vector<string>*>* tupleleft) {
	
	vector<freq> labeledclass;	
	vector<ResultOfAttr> attributeData;	

	int a = 0;
	int b = 0;

	for (int i = 0; i < tupleleft->size(); i++) 
	{
		a = 0;
		string classvalue = tupleleft->at(i)->at(classLableIndex);
		string attributevalue = tupleleft->at(i)->at(attrnum);

		for (int j = 0; j < attributeData.size(); j++) {
			if (attributeData[j].attributevalue.compare(attributevalue) == 0)
			{
				b= 0;
				for (int k = 0; k < attributeData[j].freqcount.size(); k++) 
				{
					if (attributeData[j].freqcount[k].classvalue.compare(classvalue) == 0) 
					{
						attributeData[j].freqcount[k].count++;
						b = 1;
						break;
					}
				}

				if (b==0) 
				{
					freq classValueCountPair;

					classValueCountPair.classvalue = classvalue;
					classValueCountPair.count = 1;

					attributeData[j].freqcount.push_back(classValueCountPair);
				}

				a = 1;
				break;
			}
		}

		if (a==0)
		{
			ResultOfAttr attributeDatum;
			attributeDatum.attributevalue = attributevalue;

			freq classValueCountPair;
			classValueCountPair.classvalue = classvalue;
			classValueCountPair.count = 1;
			attributeDatum.freqcount.push_back(classValueCountPair);

			attributeData.push_back(attributeDatum);
		}
		b= 0;
		for (int j = 0; j < labeledclass.size(); j++) 
		{
			if (labeledclass[j].classvalue.compare(classvalue) == 0)
			{
				b = 1;
				labeledclass[j].count++;
				break;
			}
		}

		if (b==0) {
			freq classValueCountPair;

			classValueCountPair.classvalue = classvalue;
			classValueCountPair.count = 1;

			labeledclass.push_back(classValueCountPair);
		}
	}

	
	double infoD;
	{
		vector<int> pi;
		for (int i = 0; i < labeledclass.size(); i++) {
			pi.push_back(labeledclass[i].count);
		}
		infoD = CalInfoD(&pi);
	}

	
	double infoADSum = 0.0;
	for (int i = 0; i < attributeData.size(); i++)
	{
		vector<int> yes;
		double sum = 0.0;
		for (int j = 0; j < attributeData[i].freqcount.size(); j++)
		{
			yes.push_back(attributeData[i].freqcount[j].count);
			sum += attributeData[i].freqcount[j].count;
		}
		infoADSum += (sum / tupleleft->size()) * CalInfoD(&yes);
	}

	

	return infoD - infoADSum;
}


void Decide(vector<string>* tuple, const Node* node) 
{
	const string attributevalue = tuple->at(node->attrnum);

	
	for (int i = 0; i < node->children.size(); i++)
	{
		if (node->children[i].value.compare(attributevalue) == 0) {
			if (node->children[i].p == NULL) 
			{
				tuple->push_back(node->children[i].classvalue);
			}
			else
			{
				Decide(tuple, node->children[i].p);
			}
			break;
		}
	}
}


Node* GenTree(const vector<vector<string>*> tupleleft, vector<int> leftattributenum, const int classLableIndex) {

	int selected = 0;
	double maximumgain = Calgain(leftattributenum[selected], classLableIndex, &tupleleft);
	for (int i = 1; i < leftattributenum.size(); i++)
	{
		double gain = Calgain(leftattributenum[i], classLableIndex, &tupleleft);
		if (gain > maximumgain)
		{
			maximumgain = gain;
			selected = i;
		}
	}
	const int selectedAttributeIndex = leftattributenum[selected];


	leftattributenum.erase(leftattributenum.begin() + selected);

	Node* node = new Node;
	node->attrnum = selectedAttributeIndex;

	vector<ResultOfChild> reslutofchild;
	for (int i = 0; i < tupleleft.size(); i++) {
		string childValue = tupleleft[i]->at(selectedAttributeIndex);
		string classvalue = tupleleft[i]->at(classLableIndex);

		bool childValueFound = false;
		for (int j = 0; j < node->children.size(); j++) {

			if (node->children[j].value.compare(childValue) == 0) {
				reslutofchild[j].tupleleft.push_back(tupleleft[i]);


				bool classValueFound = false;
				for (int k = 0; k < reslutofchild[j].freqcount.size(); k++) {
					if (reslutofchild[j].freqcount[k].classvalue.compare(classvalue) == 0) {
						reslutofchild[j].freqcount[k].count++;
						classValueFound = true;
						break;
					}
				}
				if (!classValueFound) {
					freq classValueCountPair;

					classValueCountPair.classvalue = classvalue;
					classValueCountPair.count = 1;

					reslutofchild[j].freqcount.push_back(classValueCountPair);
				}

				childValueFound = true;
				break;
			}
		}
		if (!childValueFound)
		{
			Child child;
			child.value = childValue;
			node->children.push_back(child);

			ResultOfChild childDatum;
			childDatum.tupleleft.push_back(tupleleft[i]);

			freq classValueCountPair;
			classValueCountPair.classvalue = classvalue;
			classValueCountPair.count = 1;
			childDatum.freqcount.push_back(classValueCountPair);

			reslutofchild.push_back(childDatum);
		}
	}

	for (int i = 0; i < node->children.size(); i++)
	{
		if (reslutofchild[i].freqcount.size() == 1)
		{
			node->children[i].classvalue = reslutofchild[i].freqcount[0].classvalue;
			node->children[i].p = NULL;
		}

		else if (leftattributenum.empty())
		{
			int maxClassValueIndex = 0;
			for (int j = 1; j < reslutofchild[i].freqcount.size(); j++)
			{
				if (reslutofchild[i].freqcount[j].count > reslutofchild[i].freqcount[maxClassValueIndex].count)
				{
					maxClassValueIndex = j;
				}
			}

			node->children[i].classvalue = reslutofchild[i].freqcount[maxClassValueIndex].classvalue;
			node->children[i].p = NULL;
		}

		else {
			node->children[i].p = GenTree(reslutofchild[i].tupleleft, leftattributenum, classLableIndex);
		}
	}

	return node;
}
int main(int argc, char** argv) {
	if (argc != 3) {
		cout <<  "put enough file." << endl;
		return 0;
	}
	Table TableForTraining;
	char* f1 = argv[1];
	char* f2 = argv[2];
	string str;
	ifstream Training(f1);	
	vector<string> v;

	if (Training.is_open()) {

		// first row of the file includes kinds of attribute
		if (getline(Training, str)) {
			stringstream line(str);
			string token;

			while (line >> token) {
				TableForTraining.attribute.push_back(token);
			}
		}

		// from second row, extract real values
		while (getline(Training, str)) {
			v.clear();
			stringstream line(str);
			string token;

			while (line >> token) {
				v.push_back(token);
			}

			TableForTraining.tuple.push_back(v);
		}

		Training.close();
	}

	
	// taking care of leftover data to decide stop partioning
	vector<vector<string>*> tupleleft;
	for (int i = 0; i < TableForTraining.tuple.size(); i++) 
	{
		tupleleft.push_back(&TableForTraining.tuple[i]);
	}
	TableForTraining.labelnum = TableForTraining.attribute.size() - 1;
	vector<int> leftattributenum;
	for (int i = 0; i < TableForTraining.attribute.size(); i++) 
	{		//if not reached whole number of label, 
		if (i != TableForTraining.labelnum)
		{
			leftattributenum.push_back(i);
		}
	}


	// make tuple with curent left tuple and left attribute .
		Node* start = GenTree(tupleleft, leftattributenum, TableForTraining.labelnum);

	// get test sets
	vector<vector<string>> testtuple;

	ifstream testfile(f2);
	if (testfile.is_open()) {
		string str;

		// start from next line
		getline(testfile, str);
		

		while (getline(testfile, str)) 
		{

			v.clear();
			stringstream line(str);
			string token;

			while (line >> token)
			{
				v.push_back(token);
			}

			//decide with test tuples and root
			Decide(&v, start);
		
			testtuple.push_back(v);
		}

		testfile.close();
	}
	

	// output
	const string OUTPUT_FILE_NAME = "dt_result.txt";
	ofstream outputFile(OUTPUT_FILE_NAME);
	if (outputFile.is_open()) {
		// print attribute names
		for (int i = 0; i < TableForTraining.attribute.size(); i++) {
			outputFile << TableForTraining.attribute[i];
			if (i < TableForTraining.attribute.size() - 1) {
				outputFile << "\t";
			}
			else {
				outputFile << endl;
			}
		}

			for (int i = 0; i < testtuple.size(); i++) {
			for (int j = 0; j < testtuple[i].size(); j++) {
				outputFile << testtuple[i][j];
				if (j < testtuple[i].size() - 1) {
					outputFile << "\t";
				}
				else {
					outputFile << endl;
				}
			}
		}

		outputFile.close();
	}
	
	return 0;
}
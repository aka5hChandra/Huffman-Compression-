

//******************************************************************************************************************// 
//																                									//
//																                									//
//		This contains the Code for Huffman compression of ASCII text file 	                						//
//		And all Required Helper Functions.						                									//
//																													//
//																													//
//	The Main Compress function																						//
//			void compress(char* fileName)																			//
//																													//
//	Helper Function to read through the input file and count the frequencies of chars								//
//		vector<byteFrequency> findFrequency(char* memblock,streampos fileSize)										//
//																													//
//	Helper function to construct Binary Tree for found frequencies of chars											//
//		Node* constructBinaryTree(vector<byteFrequency> frequencyMap)												//
//																													//
//	Helper Function to find the smaller node to be used in Binary Tree construction at given time					//
//		int findSmaller ( Node *treeArray[],	int differentFrom,int numOfSubRoots)								//
//																													//
//	Helper Function to print the binary tree in post order															//
//		void printPostorder( Node *tree)																			//
//																													//
//	Helper Function to find binary prefix codes for input file by traversing constructed Binary tree				//
//		void getPrefixCode( Node *tree, int Code, int height=0,int count=0,int type=-1)								//
//																													//
//	Helper Function to write binary tree to header for decompression												//
//		void writeTreeToheader( Node *tree,ofstream& ofile,int side)												//
//																													//
//  Helper Function to prompt user to proceed																		//
//		void prompt()																								//
//																                									//
//******************************************************************************************************************//            						

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <map>
#include <stack>
#include "datatypes.h"
using namespace std;


//Map To hold unique character and there prefix code
map<short,PrefixAndHeights> byteToPrefix;

//Function declarations.
void compress(char* fileName);
vector<byteFrequency> findFrequency(char* memblock,streampos fileSize);
void printPostorder( Node *tree);
int findSmaller ( Node *treeArray[],	int differentFrom,int numOfSubRoots);
Node* constructBinaryTree(vector<byteFrequency> frequencyMap);
void getPrefixCode( Node *tree, int Code, int height=0,int count=0,int type=-1);
void writeTreeToheader( Node *tree,ofstream& ofile,int side);
void prompt();	


void compress(char* fileName)
{
	//This the main function for ASCII text file compression.

	Node* tree;
	char * memblock=NULL;
	streampos fileSize;

	//Load input file to be compressed into memmory
	ifstream file (fileName, ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		fileSize = file.tellg();
		memblock = new char [fileSize];
		file.seekg (0, ios::beg);
		file.read (memblock, fileSize);
		file.close();

		printf("File has been loaded into memory successfuly");
	}
	else 
	{
		printf( "Error in Compression : Unable to open file\n ");
		return;
	}
	//Vector to store unique chars and there corresponding frequencies.
	vector<byteFrequency> frequencyMap=findFrequency(memblock,fileSize);

	//print characters and there frequenices in asscending order.
	printf("Characters and there frequecies are stored in a Vector, Size of Vector is  %d",frequencyMap.size());


	std::sort(frequencyMap.begin(), frequencyMap.end());

	std::vector<byteFrequency>::iterator vectorPos;
	printf("------------------------------------------------------------------------\n");

	for (vectorPos = frequencyMap.begin(); vectorPos != frequencyMap.end(); ++vectorPos) {
		printf("Byte in Hex, %x in Char, %c and its Frequency is %d\n",vectorPos->byte,vectorPos->byte,vectorPos->frequency );
	}
	printf("------------------------------------------------------------------------\n");

	//prompt();

	int numOfSubRoots=frequencyMap.size();
	if(numOfSubRoots<0)
	{
		//return if there is no entries in frequency map
		printf("ERROR in Compression : No Frequency Map is constructed");
		return;
	}
	//Construct tree for found frequencies of characters.
	printf("Constructing binary tree\n");
	tree= constructBinaryTree(frequencyMap);
	if(tree==NULL)
	{
		//return if tree is null
		printf("Error in Compression : Unable to compress the file\n ");
		return;
	}
	printf("Binary tree constructed successfully\n");



	printf("Finding Prefix codes for characters\n");

	//Get prefix code for each unique characters using binary tree.
	getPrefixCode( tree, -1);

	//Print constructed tree in post order
	//	printPostorder(tree);
	//prompt();


	//count of bits written to compressed file
	int totalNumOfBits=0;
	//bits to be written to be compressed file
	char* bits=NULL;
	//temp char varible to store bits		 
	char* tempByte=new char[8];

	printf("Compressing file....\n");

	//Process each byte in input file
	for (int k = 0; k < fileSize; k++)
	{
		char curChar=memblock[k];
		//prefix and height in binary tree of curent input character
		PrefixAndHeights curPrefix=byteToPrefix[curChar];

		unsigned int prefixCode=curPrefix.prefixCode;
		int height=curPrefix.height;
		///to get bits 
		unsigned int size = sizeof(unsigned char);
		unsigned int maxPow = 1<<(size*8-1);
		int i=0,j;

		//get 8 bits for each byte 
		for(;i<size;++i){
			for(j=0;j<8;++j){
				// get last bit and shift left.
				tempByte[j]=(prefixCode&maxPow? 1 : 0);

				prefixCode = prefixCode<<1;
			}

			//store prefix code according to height of char in binary tree
			//this is to make sure 0001(three zeors) and 01(one zero) are stored as per ther heights
			//not has 1(without any zeors)
			for(int k=8-height;k<8;k++)
			{

				const char a=tempByte[k];
				if(bits==NULL) 
				{
					//allocate memory for first time
					bits=(char*)malloc(fileSize*8);
					bits[totalNumOfBits]=a;
				}
				else
				{
					bits[totalNumOfBits]=a;
				}
				//increament total number of bits
				totalNumOfBits+=1;

			}
		}


	}
	char* outFileName=strcat(fileName,".bin");

	//file for compressed data
	//since bits are appended to file, clear the file using trunt option before using if same file is
	//already exist
	ofstream ofile (outFileName, std::fstream::out | std::fstream::trunc);
	ofile.close();
	//Open same file agian but this time with append mode
	ofile.open (outFileName, ios::out|ios::binary|ios::app);
	//clear the file before writing
	ofile.clear();
	//Write tree date header of file, which is to reconstructed for decompression
	writeTreeToheader(tree,ofile,-1);

	//write -3 to indicate end of tree data
	int minusTree=-3;
	ofile.write((char*)&minusTree,(1));



	//count to make sure bytes (8 bits) are wirritend at once to file 
	int bitCount=0;

	//compresed byte to be writted to file
	int bytei=0;


	int compresedCount=0;
	//count of number of zeros
	int numOfzeros=0;
	//process every bit
	for(int i=0;i<totalNumOfBits;i++)
	{
		bool zeroFlag=false;
		//writing zerros
		//write for every 8 bits
		if(bitCount==0)
		{int pos=i;
		for(int k=0;k<8;k++)
		{

			//loop till first one is found i.e loop to 6th pos. for  00000111
			if(bits[pos+k]==1)
				break;
			//toggle zero flag and count number of zeros so far ie.5 for 00000111
			zeroFlag=true;
			unsigned char zero=0;
			i++;
			numOfzeros++;


		}
		}
		//if zero was found ,write zero and number of zeros found.
		//ie for 00000111 write 05, this saves memory by write only 0 and 5 instead of five zeros.
		if(zeroFlag)
		{

			int zero=0;
			ofile.write((char*)&zero,(1));

			ofile.write((char*)&numOfzeros,(1));
			//toggle zero flag and set number of zeros to 0
			numOfzeros=0;
			zeroFlag=false;
		}
		if(bits[i]==0)
		{
			//if current bit is 0
			if(bitCount==0)bytei=0;
			//left shift partially formed prefix code and OR 0
			else bytei=bytei<<1 | 0;
		}
		else
		{
			//if current bit is 1
			if(bitCount==0)bytei=1;
			//left shift partially formed prefix code and OR 1
			else bytei=bytei<<1 | 1;

		}
		bitCount++;	
		if(bitCount==8)
		{
			//once 8 bits are collect write it to file


			ofile.write((char*)&bytei,1);
			//reset bit count to 0
			bitCount=0;

		}


	}

	if(bitCount!=8)
	{//padding for last byte , if number bits left are less then 8
		//temp=temp>>(byteCount-8);
		//ofile.write((char*)&temp,(1));

	}

	//Memory clean up.
	delete(memblock);
	memblock=NULL;
	delete(tree);
	tree=NULL;
	delete(bits);
	bits=NULL;
	ofile.close();


	printf("File compressed successfully.\n");
	printf("Compressed binary file is %s",outFileName);

}

vector<byteFrequency> findFrequency(char* memblock,streampos fileSize)
{
	//This is the helper function to count the frequencies of each characters in input file.
	vector<byteFrequency> frequencyMap;
	for (int i = 0; i < fileSize; i++)
	{
		char temp=memblock[i];
		//Find if the char is already found
		byteFrequency searchFor(temp,0);

		auto posInVector= std::find(frequencyMap.begin(), frequencyMap.end(), searchFor);
		if(posInVector==frequencyMap.end())
		{
			//if not push new entry to vector with its frequency as 1
			byteFrequency tempByteFrequency(temp,1);
			frequencyMap.push_back(tempByteFrequency);


		}else{
			//if present increment its frequency by 1.
			posInVector->frequency++;

		}
	}
	return frequencyMap;
}
void printPostorder( Node *tree)
{

	//This is the helper function to travres and print tree in post order
	if (tree == NULL)
		return;

	// first recur on left subtree
	printPostorder(tree->left);

	// then recur on right subtree
	printPostorder(tree->right);

	//Print details
	printf("%x %c %d %d %d\n",tree->byte,tree->byte, tree->frequencySum,tree->prefixCode,tree->height);


}
Node* constructBinaryTree(vector<byteFrequency> frequencyMap)
{
	//This is the helper function to consturct Binary tree for found frequenies of characters.
	int numOfSubRoots=frequencyMap.size();
	Node *temp;
	Node **treeArray=(Node**)malloc(numOfSubRoots*sizeof(Node*));

	//Constructs nodes for each entry in frequency map,with left and right pointer set to NULL
	for(int i=0;i<numOfSubRoots ;i++)
	{
		treeArray[i]=(Node*)malloc(sizeof(Node));
		treeArray[i]->byte=frequencyMap[i].byte;
		treeArray[i]->frequencySum=frequencyMap[i].frequency;
		treeArray[i]->left = NULL;
		treeArray[i]->right = NULL;		
	}


	int subTrees=numOfSubRoots;
	int smallNode,nextSmallNode;
	//construct bianry tree itterativly by finding least two frequency sums in array 
	while(subTrees>1)
	{
		//Find least two frequency sums using helper method findSmaller.
		smallNode=findSmaller(treeArray,-1,numOfSubRoots);
		nextSmallNode=findSmaller(treeArray,smallNode,numOfSubRoots);
		temp=treeArray[smallNode];
		//construct new root node for found least frequency sum nodes.
		treeArray[smallNode]=(Node*)malloc(sizeof(Node));
		//Frequeny of new node is the sum of frequencies of found two nodes.
		treeArray[smallNode]->frequencySum= temp->frequencySum+treeArray[nextSmallNode]->frequencySum;
		//least frequency is the right node of new node
		treeArray[smallNode]->right=temp;	
		//next least frequency is the left node of new node
		treeArray[smallNode]->left=treeArray[nextSmallNode];
		//set byte of root node to -1 (unsigned char),need for reconstruction of tree for decompression
		treeArray[smallNode]->byte=255;
		//toggle frequeny in used nodes so that same is not used in next ittreation.
		treeArray[nextSmallNode]->frequencySum=-1;
		temp->frequencySum=-1;
		//decrement node of sub trees to be processed yet by 1.
		subTrees--;
	}


	return treeArray[smallNode];


}
int findSmaller ( Node *treeArray[],int differentFrom,int numOfSubRoots){

	//This is the helper function called by constructBinaryTree fucntion to find two least frequencies .
	int smaller;
	int i = 0;
	//skip through alrady proccessed nodes
	while (treeArray[i]->frequencySum==-1)
		i++;
	smaller=i;
	//skip if looking for second least frequency
	if (i==differentFrom){
		i++;
		while (treeArray[i]->frequencySum==-1)
			i++;
		smaller=i;
	}
	//Make sure found frequency is smaller one 
	for (i=1;i<numOfSubRoots;i++){
		if (treeArray[i]->frequencySum==-1)
			continue;
		if (i==differentFrom)
			continue;
		if (treeArray[i]->frequencySum<treeArray[smaller]->frequencySum)
			smaller = i;
	}

	return smaller;
}

void getPrefixCode( Node *tree, int Code, int height,int count,int type)
{
	//This is the recursive helper method to find prefix code for each unique characters by traversing bianry tree.

	//for each leaf node,(root is unisigned -1),assign prefix code and height of that node in tree 
	if (tree->byte<255){
		tree->prefixCode=Code;
		tree->height=height;

		//store the prefix code and height in vector for further refrence.
		PrefixAndHeights prefixHeight;
		prefixHeight.prefixCode=Code;
		prefixHeight.height=height;
		byteToPrefix[tree->byte]=prefixHeight;
	}
	else{
		//continue till leaf node is found
		//increment height
		height++;
		//append 0 to prefix for left node recurrions.
		//this done by using left shift and OR operation.
		if(Code==-1)
			Code=0;
		getPrefixCode( tree->left, Code<<1 |0,height,count,0); 

		//append 1 to prefix for right node recurrions.
		//this done by using left shift and OR operation.
		if(Code==-1)
			Code=1;
		getPrefixCode( tree->right, Code<<1 |1,height,count,1);
	}

	return;
}


void writeTreeToheader( Node *tree,ofstream& ofile,int side)
{

	//This is the helper method to write tree data to header file in post order 

	if (tree == NULL)
		return;


	// first recur on left subtree, 0 to indicate left
	writeTreeToheader(tree->left,ofile,0);
	// then recur on right subtree,1 to indicate rigth
	writeTreeToheader(tree->right,ofile,1);

	if(tree->byte==255)
	{
		//write -2 indicate root node
		side=-2;
		ofile.write((char*)&side,(1));

	}else
	{
		//write 0 to indicate left and 1 ot indicate right
		//ofile.write((char*)&side,(1));
		//write char at leaf node
		ofile.write((char*)&tree->byte,(1));

	}
	//printf("%u %c %d %d %d %d\n",tree->byte,tree->byte, tree->frequencySum,tree->prefixCode,tree->height,side);//,(tree->prefixCode==NULL)?"not yet ":tree->prefixCode);





}


void prompt(){
	std::cout<<"Done, Continue? y/n.\n";

	char input=cin.get();
	if(input=='n')
		return;
	else if(input=='y');
	else prompt();
}
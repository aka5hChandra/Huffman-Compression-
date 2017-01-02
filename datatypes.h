/////////////////////////////////////////////////////////////////
//															   //
//	This file contains the requried data types				   //
//															   //
/////////////////////////////////////////////////////////////////

#ifndef _DATA_TYPE
#define _DATA_TYPE

typedef struct node Node;
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <map>
#include <stack>
using namespace std;


//Node for tree stucture
//frequency sum will have frequency of repetition of each unique character in input file.
//prefix code will have huffman prifix code for the leaf node.
//height will have height of the node in binary tree.
//byte will have actual character read from input file
//*left and * right will have pointers to left and right child nodes.
//type will have type of node 0 for left 1 for right -1 for root.
//construct will have flag to indicate if node is consturcted.
struct node{
	int frequencySum;
	int prefixCode;
	int height;
	unsigned char byte;
	struct node *left,*right;
	int type;
	int construted;
};

//Data type to store Huffaman prefix code and height in binary tree
//This is to help differentiate between prefix code with diffrent heights
//i.e differentiat between codes like 00000010 and 00010.
struct PrefixAndHeights
{
	int prefixCode;
	int height;
};

//Data structure to store character read from input file and there frequency
struct byteFrequency
{


public:
	byteFrequency(char byte,int frequency):byte(byte),frequency(frequency)
	{

	}

	byteFrequency(const byteFrequency& bf):byte(bf.byte),frequency(bf.frequency)
	{

	}
	unsigned char byte;
	int frequency;
	friend bool operator== ( const byteFrequency &n1, const byteFrequency &n2);
	friend bool operator< ( const byteFrequency &n1, const byteFrequency &n2);

	friend bool operator> ( const byteFrequency &n1, const byteFrequency &n2);


};

bool operator==(const byteFrequency& left,
	const byteFrequency& right)
{
	return left.byte == right.byte;
}

bool operator<(const byteFrequency& left,
	const byteFrequency& right)
{
	return left.frequency < right.frequency;
}


bool operator>(const byteFrequency& left,
	const byteFrequency& right)
{
	return left.frequency > right.frequency;
}

#endif
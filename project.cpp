//Use only the following three libraries:
#include "parserClasses.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <string>
using namespace std;

//outs all the tokens with details for each token
void outputList (ostream& out, Token* t);
//outs all the tokens without details for each token
void outputListWithoutDetails (ostream& out, Token* t);

//Example Test code for interacting with your Token, TokenList, and Tokenizer classes
//Add your own code to further test the operation of your Token, TokenList, and Tokenizer classes
int main() {
	ifstream sourceFile;
	ofstream catagorized;
	ofstream removed; //outputs catagorized code with a specific token type removed
	ofstream conditionalStatements;
	TokenList tokens;
	TokenList tokens2; //for testing if/then elsif/then endif pairing
    bool verboseMode = true;


  //Lists for types of tokens
  TokenList operatorTokens;
  TokenList identifierTokens;
  TokenList literalTokens;
  TokenList commentBodyTokens;
  TokenList otherTokens;
  TokenList* condition;

	Tokenizer tokenizer;



	//Read in a file line-by-line and tokenize each line
	sourceFile.open("test1.vhd");
	if (!sourceFile.is_open()) {
		cout << "Failed to open file" << endl;
		return 1;
	}

	while(!sourceFile.eof()) {
		string line;
		getline(sourceFile, line);

		tokenizer.setString(&line);
		while(!tokenizer.isComplete()) {
			tokens.append(tokenizer.getNextToken());
		}
	}

	sourceFile.close();
	sourceFile.clear();

	sourceFile.open("test1.vhd");
	if (!sourceFile.is_open()) {
		cout << "Failed to open file" << endl;
		return 1;
	}

	while(!sourceFile.eof())
    {
		string line;
		getline(sourceFile, line);

		tokenizer.setString(&line);
		while(!tokenizer.isComplete()) {
			tokens2.append(tokenizer.getNextToken());
		}
		tokens2.append("\n");
	}


    catagorized.open("catagorized.txt");
    removed.open("removed.txt");
    conditionalStatements.open("conditional.txt");

    if (catagorized.fail())
    {
        cout << "cannot open catagorized.txt" << endl;
        return 1;
    }
    if (removed.fail())
    {
        cout << "cannot open removed.txt" << endl;
        return 1;
    }
    if (conditionalStatements.fail())
    {
        cout << "cannot open conditional.txt" << endl;
        return 1;
    }

    //------------ Set token details
    Token *t = tokens.getFirst();

    //set details of every token
    while(t)
	{
	    tokens.findAndSetTokenDetails(t);
	    t = t->getNext();
	}

    outputList(catagorized, tokens.getFirst());

    condition = findAllConditionalExpressions(tokens);
    outputListWithoutDetails(cout, condition->getFirst());
    findConditionalError(tokens2, verboseMode);
    checkTypeMismatch(tokens);

    //-------------------


    // Part 2



    //find all the confitional statements


    //remove all tokens of a specific type
    //tokensRemoved = removeTokensOfType(tokens, T_Identifier);
    //outputList(removed, tokens.getFirst());



	return 0;
}


void outputList (ostream& out, Token* t)
{
    tokenDetails* info;

    while(t)
	{
		out << t->getStringRep() << endl;
        if (t->isKeyword())
        {
            out << "keyword" << endl;
        }
        else
        {
            out << "not keyword" << endl;
        }
        if (t->isOperator())
        {
            out << "operator" << endl;
        }
        else if (t->isIdentifier())
        {
            out << "identifier" << endl;
        }
        else if (t->isLiteral())
        {
            out << "literal" << endl;
        }
        else if (t->isComment())
        {
            out << "comment" << endl;
        }
        else if (t->isOther())
        {
            out << "other" << endl;
        }
        else
        {
            out << "something terrible happened" << endl;
        }

		info = t->getTokenDetails();
		if (info != nullptr)
        {
            out << "type: " << info->type << endl;
            out << "width: " << info->width << endl;
        }
        out << "---------------------------" << endl << endl;
        t = t->getNext();
	}
    return;
}

void outputListWithoutDetails (ostream& out, Token* t)
{
    while(t)
	{
	    out << t->getStringRep();
	    if (t->getStringRep() != "\n")
        {
            out << " ";
        }
	    t = t->getNext();
	}
	return;
}

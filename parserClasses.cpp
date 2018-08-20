//Use only the following libraries:
#include "parserClasses.h"
#include <string>
#include <cctype>
#include <cstdlib>
#include <iostream>

bool isInteger(string testString);  //tests to see if the string holds an integer
void printTokens(Token* start, Token* end); //prints out all the tokens, starting from start until end inclusive
string lowerCase(string uppercase);  //converts a string to lowercase
bool endingThenFound (Token* currentToken); //Determines if there is a then statement after an if or elsif. Accepts the token after the if or elsif
bool endIfFound (Token* &beginSearch);  //Begins at the token after "if", and searches for "end if".  If an "end if" is found, a pointer to the token "end" is returned, so that the search can begin after this "end if"
void checkTypeMismatch(TokenList& tokens); //precondition: token details have been set

//****Token class function definitions******
Token::Token() : next(nullptr), prev(nullptr), stringRep(""), _isKeyword(false), type(T_Other), details(nullptr)
{

}

Token::Token(const string &stringRep) : next(nullptr), prev(nullptr), stringRep(stringRep), _isKeyword(false), type(T_Other), details(nullptr)
{

}

Token::Token(const Token &token) : next(nullptr), prev(nullptr), stringRep(token.stringRep), _isKeyword(token._isKeyword), type(token.type), details(token.details)
{
   details = new tokenDetails;
   details->type = token.details->type;
   details->width = token.details->width;
}

Token::~Token()
{
    delete details;
}

void Token::operator =(const Token& token)
{
    stringRep = token.stringRep;
    _isKeyword = token._isKeyword;
    type = token.type;

    if (details == nullptr && token.getTokenDetails() != nullptr)
    {
        details = new tokenDetails;
        details->type = token.details->type;
        details->width = token.details->width;
    }


}

void Token::setTokenDetails(const string& type, int _width)
{
    string typeDetail = type;


    if (details == nullptr)
    {
        details = new tokenDetails;
    }

    details->width = _width;
    details->type = typeDetail;
}
//****TokenList class function definitions******
//           function implementations for append have been provided and do not need to be modified

//Creates a new token for the string input, str
//Appends this new token to the TokenList
//On return from the function, it will be the last token in the list
void TokenList::append(const string &str)
{
	Token *token = new Token(str);
	append(token);
}

//Appends the token to the TokenList if not null
//On return from the function, it will be the last token in the list
void TokenList::append(Token *token)
{
	if (!head)
	{
		head = token;
		tail = token;
	}
	else
	{
		tail->setNext(token);
		token->setPrev(tail);
		tail = token;
	}
}

void TokenList::findAndSetTokenDetails(Token *token)
{
    string copy = lowerCase(token->stringRep);
    string nextCopy = ""; //used for searching for to and downto
    //reserved can seem to be cut off (cannot scroll further). Click on the end and use arrow keys to allow scrolling further
    string reserved[97] = {"abs", "access", "after", "alias", "all", "and", "architecture", "array", "assert", "attribute", "begin", "block", "body", "buffer", "bus", "case", "component", "configuration", "constant", "disconnect", "downto", "else", "elsif", "end", "entity", "exit", "file", "for", "function", "generate", "generic", "group", "guarded", "if", "impure", "in", "inertial", "inout", "is", "label", "library", "linkage", "literal", "loop", "map", "mod", "nand", "new", "next", "nor", "not", "null", "of", "on", "open", "or", "others", "out", "package", "port", "postponed", "procedure", "process", "pure", "range", "record", "register", "reject", "rem", "report", "return", "rol", "ror", "select", "severity", "signal", "shared", "sla", "sll", "sra", "srl", "subtype", "then", "to", "transport", "type", "unaffected", "units", "until", "use", "variable", "wait", "when", "while", "with", "xnor", "xor"};
    string numbers = "0123456789";
    string literal = "\"\'";
    string operatorList[30] =  {"**", "abs", "not", "*", "/", "mod", "rem", "+", "-", "&", "sll", "srl", "sla", "sra", "rol", "ror", "=", "/=", "<", "<=", ">", ">=", "and", "or", "nand", "nor", "xor", "xnor"};  //to be finished later
    string alphebet = "abcdefghijklmnopqrstuvwxyz";
    int startIndex = 0; //if the data is a multibit signal, this is the starting bit
    int endIndex = 0;

    if (token == nullptr)
    {
        return;
    }

    if (copy.find_first_of(numbers, 0) < 0 || copy.find_first_of(numbers, 0) >= (copy.length() ) )  //if the token has no numbers, then it may be a keyword or operator.
    {

        //go through the array of reserved words and determine if it matches any
        for (int i = 0; i < 97; i++)
        {
            if (copy == reserved[i])
            {
                token->_isKeyword = true;
                break;
            }
        }
        //test for operator

        for (int i = 0; i < 30; i++)
        {
            if (copy == operatorList[i])
            {
                token->type = T_Operator;
                break;
            }
        }

    }


    //single and double quotes mark literals
    if (copy.find_first_of(literal, 0) >= 0 && copy.find_first_of(literal, 0) < copy.length() )  //if quotation marks are found, then it is literal
    {
        token->type = T_Literal;
    }
    if (copy == "true" || copy == "false")
    {
        token->type = T_Literal;
    }
    //pure number
    if (copy.find_first_not_of(numbers, 0) < 0 || copy.find_first_not_of(numbers, 0) > copy.length())
    {
        token->type = T_Literal;
    }
    //comment bodies
    if (token->prev != nullptr && token->prev->stringRep == "--")
    {
        token->type = T_CommentBody;
    }
    if (isalpha(copy[0]) && token->type != T_Literal && token->type != T_CommentBody && token->type != T_Operator)
    {
        //if token has starts with a character letters, is not a reserved word, is not operator, is not literal, and is not comment, it is an identifier
        token->type = T_Identifier;
    }
    //default is set to T_other. If it is not any of these, it is T_Other.

    //The token following signal or variable must be the name of the signal or variable
    //set details
    if (token->prev != nullptr && token->type == T_Identifier)
    {
        //identifier details for signal declaration
        if ( lowerCase(token->prev->stringRep) == "signal" || lowerCase(token->prev->stringRep) == "variable")
        {
            if (token->details == NULL)
            {
                token->details = new tokenDetails;
            }

            token->details->type = token->next->next->stringRep;
            if (lowerCase(token->details->type) == "std_logic")
            {
                token->details->width = 1;
            }

            //iterate through the rest of the line to search for to and downto
            for (Token* iter = token->next; iter->stringRep != ";"; iter = iter->next)
            {
                nextCopy = lowerCase(iter->stringRep);

                 if ( nextCopy == "to" || nextCopy == "downto")
                 {
                    //data type is multibits
                    //Check that the vector start index and end index are intergers
                    if (isInteger(iter->prev->stringRep) && isInteger(iter->next->stringRep))
                    {
                     startIndex = stoi(iter->prev->stringRep);
                     endIndex = stoi(iter->next->stringRep);
                     token->details->width = abs(startIndex - endIndex) + 1; //default is 0 if no startIndex and endIndex is found, since both are initialized to 0
                     break;

                    }
                 }
            }

        }
        //setting details for identifiers that were delcared earlier
        else
        {
            //search through all previous tokens to find where the signal or variable was declared in order to copy the details
            Token* reverseIter = token->prev;
            while (reverseIter != nullptr)
            {

                if (copy == lowerCase(reverseIter->stringRep) && reverseIter->details != NULL)
                {
                    // signal/variable declaration is found
                    if (token->details == NULL)
                    {
                        token->details = new tokenDetails;
                    }
                    token->details->type = reverseIter->details->type;
                    token->details->width = reverseIter->details->width;
                }
                reverseIter = reverseIter->prev;
            }
        }

    }
    //set details for literals
    if (token->type == T_Literal)
    {
        if (token->details == NULL)
        {
            token->details = new tokenDetails;
            token->details->width = 0;
        }

        if (copy == "true" || copy == "false")
        {
            token->details->type = "boolean";
        }
        else if (isInteger(copy))
        {
             token->details->type = "integer";
        }
        else if (copy[0] == '\'')
        {
            token->details->type = "std_logic";
            token->details->width = 1;
        }
        else
        {
             token->details->type = "std_logic_vector";
            if (copy[0] == '\"')
            {
                token->details->width = copy.length() - 2;
            }
            if (tolower(copy[0]) == 'b')
            {
                token->details->width = copy.length() - 3;
            }
             if (tolower(copy[0]) == 'o')
            {
                token->details->width = (copy.length() - 3) * 3;
            }
            if (tolower(copy[0]) == 'x')
            {
                token->details->width = (copy.length() - 3) * 4;
            }
        }
    }

    return;
}

//Complete the implementation of the following member functions:
//****Tokenizer class function definitions******

Tokenizer::Tokenizer() : complete(false), offset(0), tokenLength(0), isComment(false), commentPos(0)
{


}

//Computes a new tokenLength for the next token
//Modifies: size_t tokenLength, and bool complete
//(Optionally): may modify offset
//Does NOT modify any other member variable of Tokenizer
void Tokenizer::prepareNextToken()
{

    int currentLocation = offset;


    offset = str->find_first_not_of(" \t", currentLocation);  //skip over any spaces

    if (offset >= str->length() || offset < 0) //if nothing is found aside from whitespace
    {
        complete = true;
    }
    else if(isComment)  //if the rest of the line is a comment
    {
        tokenLength = str->length() - commentPos;
        isComment = false; //comment has been tokenized
    }
    else
    {
    //Offset points to the next character after the end of the previous token. So the search beings at index str[offset]
    // if (the next alphanumeric character found is the next character)
        if (str->find_first_of(keywords, offset) == offset)  //if the character in the current position is alphanumeric
        {
            //First token is a variable name or VHDL keyword
            tokenLength = str->find_first_not_of(keywords, offset) - offset;
        }
        else if (str->find(comment, offset) == offset)  //if the line is a comment. Using find() instead of find_first_of() will prevent '-' being classified as a comment
        {
            tokenLength = 2;
            isComment = true;  //Tells the next run of prepareNextToken that the rest of the line is a comment
            commentPos = offset + tokenLength;
        }
        else if (str->find_first_of(operators, offset) == offset)  //if the character in the current position is an operator
        {
            //find the end of the operator
            tokenLength = str->find_first_not_of(operators, offset) - offset;

            //Operators are 1 or 2 characters long.
            //If operator is 1 character long, then str->find_first_not_of will find the next operator elsewhere in the code, and tokenLength will be > 2
           //If operator is 2 characters long, then str->find_first_not_of will find the second character of the operator and return 2
            if (tokenLength > 2 || tokenLength < 0)
            {
                tokenLength = 1;
            }

        }
        else
        {
            tokenLength = 1;
        }

    }
    if (tokenLength == 0)
    {
        complete = true;
    }

    return;
}

//Sets the current string to be tokenized
//Resets all Tokenizer state variables
//Calls Tokenizer::prepareNextToken() as the last statement before returning.
void Tokenizer::setString(string *str)
{
    offset = 0;
    tokenLength = 0;
    isComment = false;
    commentPos = 0;
    complete = false;

    this->str = str;
    prepareNextToken();

    return;
}


//Returns the next token. Hint: consider the substr function
//Updates the tokenizer state
//Updates offset, resets tokenLength, updates processingABC member variables
//Calls Tokenizer::prepareNextToken() as the last statement before returning.
string Tokenizer::getNextToken()
{
    string temp;

    temp = str->substr(offset, tokenLength);

    offset = offset + tokenLength;  //offset now points to the character that is one position after the last character in the token
    tokenLength = 0; //reset tokenLength
    prepareNextToken();
    return temp;
}

void TokenList::deleteToken(Token *token)
{
    if (token == nullptr)
    {
        return;
    }
    else if (token == head && token == tail)
    {
        head = nullptr;
        tail = nullptr;
        delete token;
        token = nullptr;
    }

    else if (token == head)
    {
        token->next->setPrev(nullptr);
        head = token->next;
        delete token;
        token = nullptr;
    }

    else if (token == tail)
    {
        token->prev->setNext(nullptr);
        tail = token->prev;
        delete token;
        token = nullptr;
    }

    else
    {
        token->next->setPrev(token->prev);
        token->prev->setNext(token->next);
        delete token;
        token = nullptr;
    }

    return;
}

//Removes all comments from the tokenList including the -- marker
//Returns the number of comments removed
int removeComments(TokenList &tokenList)
{
    Token *tokenPtr = nullptr;
    Token *tempPtr = nullptr;
    int tokenNumber = 0;  //number of tokens removed

    tokenPtr = tokenList.getFirst();

    if (tokenPtr == nullptr)
    {
        return 0;
    }

    while( tokenPtr != nullptr)
    {
        if (tokenPtr->getStringRep() == "--")
        {
            tempPtr = tokenPtr->getNext();
            tokenList.deleteToken(tokenPtr);
            tokenPtr = tempPtr->getNext();
            tokenList.deleteToken(tempPtr);

            tokenNumber++;
            continue;
        }

        else
        {
            tokenPtr = tokenPtr->getNext();
            continue;
        }
    }
    return tokenNumber;
}

int removeTokensOfType(TokenList &tokenList, tokenType type)
{
    int numRemoved = 0;
    Token* destroy = nullptr;  //points to the token being deleted
    Token* iter = tokenList.getFirst();

    while ( iter != nullptr )
    {
        if (iter->getTokenType() == type)
        {
            //if token will be deleted, iter will have to be be moved to next node in the list before deletion of the node
            destroy = iter;
            iter = iter->getNext();
            tokenList.deleteToken(destroy);
            numRemoved++;
        }
        else
        {
            iter = iter->getNext();
        }

    }
    return numRemoved;
}

TokenList* findAllConditionalExpressions(const TokenList &tokenList)
{
    TokenList conditional;
    TokenList* conditionalptr = &conditional;
    Token* lastNode;
    Token* iter;
    string copy = "";  //copy of stringRep, in lowercase
    string prevCopy = " "; //copy of stringRep in previous node, in lower case

    bool flag = false; //flag to signal that the next token will be the start of the conditional expression
    bool copyToken = false;   //signals that tokens should be copied


    for (iter = tokenList.getFirst(); iter != nullptr; iter = iter->getNext())
    {
        if (flag == true)
        {
            copyToken = true;
        }

        copy = iter->getStringRep();

        //convert to lower case
        for (int i = 0; i < copy.length(); i++)
        {
            if (isalpha(copy[i]))
            {
                copy[i] = tolower(copy[i]);
            }

        }

        if ( (copy == "if" && prevCopy != "end") || copy == "elsif" || copy == "when")
        {
            flag = true;
        }
        if (copy == "then" || copy == "else")
        {
            copyToken = false;
            flag = false;
            conditional.append("\n");
        }

        if(copyToken)
        {
            conditional.append("");  //create a new empty token for information to be copied over
            lastNode = conditional.getLast();
            *lastNode = *iter; //copy the node over using the overloaded assignment operator

        }
        prevCopy = copy;
    }
    return conditionalptr;
}

bool isInteger(string testString)
{

    if (!isdigit(testString[0]) && testString[0] != '-')
    {
        return false;
    }

    for (int i = 1; i < testString.length(); i++)
    {
        if (!isdigit(testString[i]))
        {
            return false;
        }
    }
    return true;
}

string lowerCase(string uppercase)
{
    string copy = uppercase;

     for (int i = 0; i < copy.length(); i++)
     {
            if (isalpha(copy[i]))
            {
                copy[i] = tolower(copy[i]);
            }

     }

     return copy;
}

void findConditionalError (const TokenList &tokenList, bool verboseMode)
{
    Token* iter = tokenList.getFirst();
    Token* checkNearbyTokens = nullptr;  //pointer to check the tokens around iter
    int numberTokens = 0; //number of tokens in the list
    int numberConditional = 0; //number of conditional statements
    int missingThen = 0; //number of missing then's
    int missingEndif = 0;  //number of missing end if's
    bool foundThen = false;  //Used to signal whether a corresponding THEN is found
    string currentToken = "";  //the string stored in the current token
    string prevToken = "";      //the string stored in the previous token
    Token* endif = tokenList.getFirst();  //points to the "if" in "end if" in tokenList



    //loop through the entire list of tokens to find each conditional statement and determine if they are terminated properly
    while (iter != nullptr)
    {
        foundThen = false; //reset foundThen to be reused
        currentToken = lowerCase(iter->getStringRep());

        if ( (currentToken == "if" && prevToken != "end") || currentToken == "elsif")
        {

            //Loop through next few tokens until a THEN is found or until "\n" if found. If "\n" is found first, there is a missing THEN
            for (checkNearbyTokens = iter; checkNearbyTokens->getStringRep()!= "\n"; checkNearbyTokens = checkNearbyTokens->getNext())
            {
                if (lowerCase(checkNearbyTokens->getStringRep()) == "then")
                {
                    numberConditional++;
                    foundThen = true;
                    break;
                }
            }
            if (!foundThen)
            {
                if (verboseMode)
                {
                    cout << "Error in lines: " << endl;
                    printTokens(iter, checkNearbyTokens); //print the line of code

                    //CheckNearbyTokens stops at THEN or \n. If it stops at THEN, no new line is printed, whereas if it stops at \n, a new line is printed
                    if (checkNearbyTokens->getStringRep() != "\n")
                    {
                        cout << endl;
                    }
                    cout << "Missing \"then\"" << endl << endl;
                }

                missingThen++;
            }

            // Search for a corresponding end if.  Each end if is associated with the first if statement that doesn't have an end if
            if (currentToken == "if")
            {
                if (!endIfFound(endif))
                {
                    if (verboseMode)
                    {
                        cout << "Error in lines: " << endl;
                        printTokens(iter, checkNearbyTokens);  //Print the line of code

                    //CheckNearbyTokens stops at THEN or \n. If it stops at THEN, no new line is printed, whereas if it stops at \n, a new line is printed
                    if (checkNearbyTokens->getStringRep() != "\n")
                        {
                            cout << endl;
                        }
                        cout << " No \"end if\" found" << endl << endl;
                    }
                    missingEndif++;
                }
            }
        }
        numberTokens++;
        prevToken = currentToken;
        iter = iter->getNext();
    }

    cout << "There are " << numberTokens << " tokens" << endl;
    cout << "There are " << numberConditional << " valid conditional expressions" << endl;
    cout << "There are " << missingEndif << " missing \"end if\"s" << endl;
    cout << "There are " << missingThen << " missing \"then\"s" << endl;

    return;
}

void printTokens(Token* start, Token* end)
{
    if (start != nullptr)
    {
        while (start != end)
        {
            cout << start->getStringRep();
            if (start->getStringRep() != "\n")
            {
                cout << " ";
            }
            start = start ->getNext();
        }
    }
    //loop does not print out string in end
    //in some cases, if the expression is at the end of file, end may receive the value nullptr, so a check will prevent possible crashes
     if (end != nullptr)
     {
        cout << end->getStringRep();
     }
    return;
}

bool endIfFound (Token* &beginSearch)
{
   while (beginSearch != nullptr)
   {
        if (lowerCase(beginSearch->getStringRep()) == "end" && beginSearch->getNext() != nullptr && lowerCase(beginSearch->getNext()->getStringRep()) == "if")
        {
            beginSearch = beginSearch->getNext();  //This endif is now used. Next search starts at next token
            return true;
        }
        beginSearch = beginSearch->getNext();
   }
   return false;
}

void checkTypeMismatch(const TokenList &tokens)
{
    int typeMismatch = 0;
    int widthMismatch = 0;
    Token* checkNearbyTokens = nullptr;
    string comparison;


    for (Token* iter = tokens.getFirst(); iter != nullptr; iter = iter->getNext())
    {
        if (iter->getStringRep() == "if" || iter->getStringRep() == "elsif")
        {
            //for (checkNearbyTokens = iter; checkNearbyTokens->getNext() != nullptr; checkNearbyTokens = checkNearbyTokens->getNext())
            for (checkNearbyTokens = iter; checkNearbyTokens->getNext() != nullptr; checkNearbyTokens = checkNearbyTokens->getNext())
            {
                comparison = checkNearbyTokens->getStringRep();
                if (comparison == "<=" || comparison == ">=" || comparison == "/="|| comparison == "="
                || comparison == ">" || comparison == "<")
                {
                    if (checkNearbyTokens->getPrev()->getTokenDetails() != nullptr && checkNearbyTokens->getNext()->getTokenDetails() != nullptr)
                    {
                        if (checkNearbyTokens->getPrev()->getTokenDetails()->type != checkNearbyTokens->getNext()->getTokenDetails()->type)
                        {
                            typeMismatch++;
                            cout << endl << "Type mismatch at:" << endl;
                            printTokens(checkNearbyTokens->getPrev(), checkNearbyTokens->getNext());
                        }
                        else if (checkNearbyTokens->getPrev()->getTokenDetails()->width != checkNearbyTokens->getNext()->getTokenDetails()->width)
                        {
                            widthMismatch++;
                            cout << endl << "Width mismatch at:" << endl;
                            printTokens(checkNearbyTokens->getPrev(), checkNearbyTokens->getNext()->getNext());
                        }
                    }
                    else
                    {
                        if (checkNearbyTokens->getNext()->getStringRep() == "not")
                        {
                            Token* test = checkNearbyTokens->getPrev();
                            Token* test2 = checkNearbyTokens->getNext()->getNext();
                            if (checkNearbyTokens->getPrev()->getTokenDetails()->type != checkNearbyTokens->getNext()->getNext()->getTokenDetails()->type)
                            {
                                typeMismatch++;
                                cout << endl << "Type mismatch at:" << endl;
                                printTokens(checkNearbyTokens->getPrev(), checkNearbyTokens->getNext()->getNext());
                            }
                            else if (checkNearbyTokens->getPrev()->getTokenDetails()->width != checkNearbyTokens->getNext()->getNext()->getTokenDetails()->width)
                            {
                                widthMismatch++;
                                cout << endl << "Width mismatch at:" << endl;
                                printTokens(checkNearbyTokens->getPrev(), checkNearbyTokens->getNext()->getNext());
                            }
                        }
                        else
                        {
                            cout << endl << "No details exist for the line:" << endl;
                            printTokens(checkNearbyTokens->getPrev(), checkNearbyTokens->getNext());
                        }
                    }
                }

                else
                {
                    continue;
                }
            }
        }
    }

    cout << endl << "There are " << typeMismatch << " type mismatches" << endl;
    cout << "There are " << widthMismatch << " width mismatches" << endl;
    return;
}

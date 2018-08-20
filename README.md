Simple Compiler 2015

Date: Spring 2015

Authors:  
Greyson Wang  
Dayton Pukanich  

This code parser does some of the tasks that a compiler needs to do, including tokenizing the code and checking the syntax.
This code parser will parse VHDL code.

Tokens are categorized into several broad categories along with storing the type for identifiers
and literals.  The parser will identify all conditional expressions within the code and then check for certain
kinds of errors in the input files.

Tokens are catagorized into:
- Keywords  
- Identifiers  
- Operators  
- Literals  
- Comment bodies   
- Other

For identifiers (that are for variables and signals) and literals the parser will identify the
type of the variable and the width (if it is a vector type).

The parser will identify all conditional expressions inside of when/else
structures and if/elsif/else structures, and it will check that they follow the correct syntax rules:
Every if statement must end with an end if. It may have multiple elsifs in between, and it may
also have an else case. For every “if” or “elsif” there must be a corresponding “then”.  

The code parser will check that when two identifiers are compared, they are the same type and, if bit vectors,
the same width. When comparing against a literal against an identifier it will check to see if the
literal's properties match the identifiers.

Missing brackets and semicolons will be caught by the parser.

For each error, the error, as well as the line number will be printed.






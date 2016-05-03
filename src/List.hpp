//
//  List.hpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//  Copyright © 2016 Joachim Wester, Starcounter AB.
//

#ifndef List_hpp
#define List_hpp

#include "Value.hpp"
#include "Isolate.hpp"

class List;
class Cons;

// This value represents all kinds of lists (including strings and maps). If the list is empty,
// there will only be this value. If the list is non-empty, it will have a representation
// on the heap. The representation depends on the predicted use-case. I.e. if we suspect new
// lists will be derived, we will create a persistent vector. If not, we will create a simple
// array.
class LIST : public VALUE {
public:
    LIST() {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        Integer = 0;
    };
    
    // Create a list that points to a Cons (a classical lisp linked list pair node)
    LIST( VALUE _first, VALUE _rest) {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        MaterializeAsCons( _first, _rest );
    }
    
    
    LIST( List* list ) {
        Integer = (uint64_t)list;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
    }
    
    
    // Append to the end of this list. As this is slow in persistent linked lists,
    // the new list will probably have another type of materialization.
    LIST Append( VALUE elem );
    
    
    // Empty lists are not allocated on the heap.
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    std::string Print();
    
    Cons* MaterializeAsCons( VALUE first, VALUE rest );
    
    LIST ReplaceAt( int i, VALUE v );
    VALUE GetAt( int i );
    
    List* GetList() {
        return (List*)Integer;
    }
    
    
};


class List : public Object {
public:
    virtual VALUE First() = 0;
    virtual VALUE Rest() = 0;
    
    virtual List* Append( VALUE v ) = 0;
    virtual List* Prepend( VALUE v ) = 0;
    
    virtual VALUE GetAt( int i ) = 0;
    virtual List* ReplaceAt( int i, VALUE v ) = 0;
    
    virtual int Count() = 0;

    virtual List* RemoveAt( int i ) = 0;
    virtual List* InsertAt( int i, VALUE v ) = 0;
    virtual List* Concatenate( VALUE v ) = 0;
    virtual List* Reverse() = 0;
    virtual List* Replace( VALUE v1, VALUE v2 ) = 0;
    virtual List* Sort( VALUE fun ) = 0;
    virtual List* Map( VALUE fun ) = 0;
    virtual List* First( int i ) = 0;
    virtual List* Last( int i ) = 0;
    virtual List* Skip( int i ) = 0;
};



#endif /* List_hpp */

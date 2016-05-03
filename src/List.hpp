//
//  List.hpp
//  Addie
//
//  Created by Joachim Wester on 03/05/16.
//
//

#ifndef List_hpp
#define List_hpp


#include "Value.hpp"
#include "Isolate.hpp"

class List;



class LIST : public VALUE {
public:
    LIST() {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        Integer = 0;
    };
    
    LIST( VALUE _first, VALUE _rest) {
        Type = PList;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
        MaterializeAsCons( _first, _rest );
    }
    
    
    LIST( List* cons ) {
        Integer = (uint64_t)cons;
        Style = QParenthesis; // See VALUE::IsClassicalParenthesis
    }
    
        
    LIST Append( VALUE elem );
    
    
    bool IsEmptyList() {
        return Integer == 0;
    }
    
    std::string Print();
    
    Cons* MaterializeAsCons( VALUE first, VALUE rest );

    
    LIST SetAt( int i, VALUE v );    
    VALUE GetAt( int i );
    
    List* GetList() {
        return (List*)Integer;
    }
    
    
};


class List : public Object {
public:
    virtual VALUE First() = 0;
    virtual VALUE Rest() = 0;
    
    virtual LIST Append( VALUE v ) = 0;
    virtual LIST Prepend( VALUE v ) = 0;
    
    virtual VALUE GetAt( int i ) = 0;
    virtual LIST ReplaceAt( int i, VALUE v ) = 0;
    
    virtual int Count() = 0;

    virtual VALUE RemoveAt( int i ) = 0;
    virtual VALUE InsertAt( int i, VALUE v ) = 0;
    virtual VALUE Concatenate( VALUE v ) = 0;
    virtual VALUE Reverse() = 0;
    virtual VALUE Replace( VALUE v1, VALUE v2 ) = 0;
    virtual VALUE Sort( VALUE fun ) = 0;
    virtual VALUE Map( VALUE fun ) = 0;
    virtual VALUE First( int i ) = 0;
    virtual VALUE Last( int i ) = 0;
    virtual VALUE Skip( int i ) = 0;
};



#endif /* List_hpp */

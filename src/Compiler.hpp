//
//  Header.h
//  Addie
//
//  Created by Joachim Wester on 05/05/16.
//
//

#ifndef Compier_hpp
#define Compier_hpp

#include <sstream>


using namespace Addie::Internals;

namespace Addie {

    namespace Internals {

        
        struct Capture {
            uint8_t ParentRegister;
            uint8_t ChildRegister;
            Capture( uint8_t p, uint8_t c ) : ParentRegister(p), ChildRegister(c) {
            }
        };
        
        enum RegisterType {
            Argument,
            Closure,
            Intermediate,
            Local,
            Return,
            RuntimeReference
        };
        
        struct Explanation {
            Symbol symbol;
            RegisterType type;
            Explanation( Symbol sym, RegisterType t ) : symbol(sym), type(t) {};
            std::string Print() {
                
                std::ostringstream res;

                
                switch (type) {
                    case Closure:
                        res << "clo";
                        break;
                    case Argument:
                        res << "arg";
                        break;
                    case Local:
                        res << "loc";
                        break;
                    case RuntimeReference:
                        res << "ext";
                        break;
                    case Intermediate:
                        res << "tmp";
                        break;
                    case Return:
                        res << "ret";
                        break;
                    default:
                        res << "???";
                        break;
                }
                if (symbol != 0) {
                    res << " ";
                    res << SYMBOL(symbol).Print();
                }
                return res.str();
            }
        };
        
        class Metaframe;
        
        class Binding {
        public:
            Binding() {
                
            }
            Binding( Metaframe* frame, uint8_t reg ) : Frame(frame), Register(reg) {
            }
            
            Metaframe* Frame;
            uint8_t Register;
        };
        
        
        //struct Variable {
        //public:
        //    Variable( Symbol sym, VALUE val ) : symbol(sym), defaultValue(val) {
        //    }
        //    Symbol symbol;
        //    VALUE defaultValue;
        //};
        
        struct RegisterUse {
            bool InUse : 1;
            bool IsClosure : 1;
            bool IsArgument: 1;
            bool IsConstant: 1;
            RegisterUse() {
                InUse = false;
                IsClosure = false;
                IsArgument = false;
                IsConstant = false;
            }
        };
        
        class VariableScope {
        private:
            std::map<Symbol,Binding> Bindings;
        public:
            Metaframe* metaframe;
            VariableScope* parent = NULL;
            
            int AllocateInitializedRegister( Isolate* isolate, VALUE value, Symbol symbol, RegisterType type );
            

            int ExtendedFindRegisterForSymbol( Isolate* isolate, Symbol id, bool scanForeignParentFrames, Metaframe* &foundInMetaframe ) {
                auto x = Bindings.find(id);
                if ( x != Bindings.end()) {
                    foundInMetaframe = metaframe;
                    return x->second.Register;
                }
                if (parent != NULL) {
                    Metaframe* f = NULL;
                    int i = parent->ExtendedFindRegisterForSymbol(isolate,id,scanForeignParentFrames,f);
                    if ( i != -1 ) {
                        assert( f != NULL);
                        foundInMetaframe = f;
                        if (f == metaframe || scanForeignParentFrames) {
                            return i;
                        }
                    }
                }
                return -1;
            }
            
            int FindRegisterForSymbol( Isolate* isolate, Symbol id ) {
                Metaframe* foundInMetaframe = NULL;
                return ExtendedFindRegisterForSymbol(isolate, id, false, foundInMetaframe );
            }

            
            void BindSymbolToRegister( Symbol id, int regNo, RegisterType type );
            
        };
        
        class MetaCompilation : Object {
        public:
            std::vector<Metaframe*> metaframes;
            Compilation* compilation;
            Metaframe* currentMetaframe;
        };
        

        
        class Metaframe  {
        public:
            std::vector<Capture> enclosedVariables;
            bool IsFlushed = false;
            RegisterUse RegUsage[256];
            std::vector<Explanation> Registers;

            
            Metaframe( Isolate* isolate, VariableScope* parent, Compilation* comp ) :Parent(parent) {
                if (parent != NULL ) {
                    Metaframe* parentMetaframe = parent->metaframe;
                    compilation  = parentMetaframe->compilation;
                    codeFrame = parentMetaframe->codeFrame;
                    rootScope.parent = parent;
                }
                rootScope.metaframe = this;
                currentScope = &rootScope;
                //writeHead = ((byte*)unit) + sizeof(CodeFrame);
                
                compilation = comp;
                //codeFrame = unit;
                //unit->metaframe = this;
                
                currentScope->AllocateInitializedRegister(isolate,NIL(),RET,Return); // Return register
                RegUsage[0].InUse = false;

                
                
            }
//            Metaframe( Metaframe* parent, CodeFrame cu, Compilation c ) :Parent(parent),
//                                            codeFrame(cu), compilation(c) {}
            VariableScope* Parent = NULL;
            
            VariableScope rootScope;
            VariableScope* currentScope;
            
//            std::vector<Symbol> Registers;
//            std::map<Symbol,Binding> Bindings;
            
            VALUE Body;
//            int constants = 0;
            CodeFrame* codeFrame = NULL;
            int sizeOfCodeFrame;
            Compilation* compilation = NULL;
            //byte* writeHead;
//            int intermediatesUsed = 0;
            int maxIntermediatesUsed = 0;
            int maxInitializedRegisters = 0;
            
            Instruction* tempCodeWriteHead;
            Instruction* tempCodeBuffer = NULL; // Will point to a temporary stack allocation during compilation
            VALUE* tempRegisterWriteHead;
            VALUE* tempRegisterBuffer = NULL; // Will point to a temporary stack allocation during compilation
            
            
            int AllocateRegister( Isolate* isolate, RegisterAllocationMethod mtd, int existingRegNo ) {
                if (mtd == UseReturnRegister) {
                    return 0;
                }
                if (existingRegNo == 0) {
                    return AllocateIntermediateRegister(isolate);
                }
                return existingRegNo;
            }
            
            VariableScope* TopScopeInSameFrame() {
                auto s = currentScope;
                while (s->parent != NULL && s->parent->metaframe == this ) {
                    s = s->parent;
                }
                return s;
            }
            
            Instruction* BeginCodeWrite( Isolate* isolate ) {
                if (tempCodeBuffer == NULL) {
                    tempCodeWriteHead = tempCodeBuffer = (Instruction*)(isolate->NextOnStack);
                }
                return tempCodeWriteHead;
            }
            
            VALUE* BeginRegisterWrite( Isolate* isolate ) {
                if (tempRegisterBuffer == NULL) {
                    tempRegisterWriteHead = tempRegisterBuffer = (VALUE*)(isolate->NextOnStack2);
                }
                return tempRegisterWriteHead;
            }
            
            size_t GetSizeOfCode() {
                if (!IsFlushed) {
                    if (tempCodeWriteHead == NULL) {
                        return 0;
                    }
                   return ((byte*)tempCodeWriteHead - (byte*)tempCodeBuffer);
                }
                return sizeOfCodeFrame - codeFrame->sizeOfInitializedRegisters - sizeof(CodeFrame);
            }
            
            void SetReturnRegister( VALUE v ) {
                assert( tempRegisterBuffer != NULL );
                tempRegisterBuffer[0] = v;
            }
            
            void EndRegisterWrite( Isolate* isolate, VALUE* addr ) {
                size_t size = (byte*)addr - (byte*)tempRegisterWriteHead;
                tempRegisterWriteHead = addr;
                isolate->AdvanceStack2(size);
            }
            
            void EndCodeWrite( Isolate* isolate, Instruction* addr ) {
                size_t size = (byte*)addr - (byte*)tempCodeWriteHead;
                tempCodeWriteHead = addr;
                isolate->AdvanceStack(size);
            }
            
//            void FoundConstant( VALUE form ) {
//                std::cout << "Found constant " << form.Print() << "\n";
//                constants++;
//            }

            
            // Finds an available register that is not associated with
            // a variable, a constant or an argument. Marks the register
            // as being in use and returns the register number.
            // The function assumes that we don't know how many registers will be
            // used for variables, constants and arguments, so it allocates
            // registers from the other end (beginning with 255). A second pass
            // is needed to avoid big registers gaps. This is done by moving the
            // registers used down to the registers immediatelly following the
            // initialized registers (the lower numbered registers).
            int AllocateIntermediateRegister(Isolate* isolate) {
                if (!RegUsage[0].InUse) {
                    // The return register is available
                    RegUsage[0].InUse = true;
                    return 0;
                }
                int regNo = 255; // 6 - intermediatesUsed; // - 1;
                while (true) {
                    if (!RegUsage[regNo].InUse) {
                        RegUsage[regNo].InUse = true;

                        int used = -regNo + 255;
                        if (used > maxIntermediatesUsed)
                            maxIntermediatesUsed = used;

                        return regNo;
                    }
                    regNo--;
                }
                throw std::runtime_error("No free registers");
            }
            
            // Called when a register is no longer in use. In this way,
            // registers can be reused.
            void FreeIntermediateRegister( int regNo ) {
                int highestFixedRegisterNo = maxInitializedRegisters-1;
                if (regNo == 0 || regNo > highestFixedRegisterNo ) {
                   RegUsage[regNo].InUse = false;
                }
            }
            
            int __allocateConstant( Isolate* isolate, VALUE value ) {
                
                int regNo;
                
                if (value.IsSymbol()) {
                    
                    Symbol id = value.SymbolId;
                    int regNo = currentScope->FindRegisterForSymbol(isolate,id);
                    
                    if ( regNo != -1 ) {
                        RegUsage[regNo].InUse = true;
                        return regNo;
                    }
                }
                
                VALUE* reg = BeginRegisterWrite(isolate); //(VALUE*)writeHead;
                (*reg++) = value;
                EndRegisterWrite(isolate,reg);
                
//                writeHead = (byte*)reg;
                regNo = maxInitializedRegisters; // codeFrame->AddInitializedRegister();
                maxInitializedRegisters++;
                RegUsage[regNo].InUse = true;
                
                //if (value.IsSymbol()) {
                //    currentScope->BindSymbolToRegister(value.SymbolId,regNo);
                //}
                
                //std::cout << "Default value r[" << (int)regNo << "]=" << value.Print() << "\n";
                return regNo;
            }
            
            void Flush(Isolate* isolate);

            
            Explanation ExplainRegister( int regNo ) {
                return Registers[regNo];
            }

        };

    }
    
    class Compiler {
    public:
        static MetaCompilation* Compile( Isolate* isolate, VALUE form );
//        static Compilation* CompilePrototype( Isolate* isolate, VALUE form );
        static STRINGOLD Disassemble(  Isolate* isolate, Compilation* code, MetaCompilation* meta );
    };
    

}


#endif /* Compier_hpp */

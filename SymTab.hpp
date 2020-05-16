#ifndef EXPRINTER_SYMTAB_HPP
#define EXPRINTER_SYMTAB_HPP

#include <string>
#include <stack>
#include <map>
// #include <vector>

#include "Descriptor.hpp"
#include "DescriptorFunctions.hpp"

class FunctionDefinition;


// This is a flat and integer-based symbol table. It allows for variables to be
// initialized, determines if a give variable has been defined or not, and if
// a variable has been defined, it returns its value.

class SymTab {
public:

    SymTab() = default;
    ~SymTab() = default;

    bool isDefined(std::string vName);
    bool erase(std::string vName);
    void createEntryFor(std::string, int);
    void createEntryFor(std::string, double);
    void createEntryFor(std::string, bool);
    void createEntryFor(std::string, std::string);
    void createEntryFor(std::string, const char*);

    void setValueFor(std::string, std::shared_ptr<TypeDescriptor>);
    // int getValueFor(std::string vName);

    TypeDescriptor *getValueFor(std::string);

    void openScope();
    void activateScope();
    void closeScope();

    std::shared_ptr<TypeDescriptor> getReturnValue() { return _returnValue; }
    void setReturnValue(std::shared_ptr<TypeDescriptor> rv) { _returnValue = rv; }

    void setFunction(std::string fName, std::shared_ptr<FunctionDefinition> fDef) { _functionTable[fName] = fDef; }
    std::shared_ptr<FunctionDefinition> getFunction(std::string fName) {
      if (_functionTable[fName] == nullptr) {
          std::cout << fName << " does not exist!\n";
      }
      return _functionTable[fName];
     }

     void printIndividualTable(std::map<std::string, std::shared_ptr<TypeDescriptor>>&, std::string);
     void dumpTable();

private:

    void addDescriptor(std::string,  std::shared_ptr<NumberDescriptor>);


    std::map<
        std::string,
        std::shared_ptr<TypeDescriptor>
    > globalSymTab;

    std::map<
        std::string,
        std::shared_ptr<FunctionDefinition>
    > _functionTable;

    std::stack<
        std::map< std::string, std::shared_ptr<TypeDescriptor>
	>> symTab;

    std::map<std::string, std::shared_ptr<TypeDescriptor>> &previousScope{
        globalSymTab
    };
    bool readFromPreviousScope{false};
    // std::vector<
    //     std::map<std::string, std::shared_ptr<TypeDescriptor>>
    // > symTab;

    std::shared_ptr<TypeDescriptor> _returnValue;
};

#endif //EXPRINTER_SYMTAB_HPP

// STD MOVE
// template<class _Ty> inline
//   constexpr typename remove_reference<_Ty>::type&&
//     move(_Ty&& _Arg) _NOEXCEPT
//   {
//     return (static_cast<typename remove_reference<_Ty>::type&&>(_Arg));
//   }

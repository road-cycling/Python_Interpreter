#include <stack>
#include <iostream>
#include <iomanip>
#include "SymTab.hpp"

// https://stackoverflow.com/questions/41871115/why-would-i-stdmove-an-stdshared-ptr

/* void SymTab::addDescriptor(std::string vName, TypeDescriptor descriptor) {
    if (symTab.size() > 0)
        (stack.top())[vName] = std::move(descriptor);
    else
        globalSymTab[vName] = std::move(descriptor);
}*/


void SymTab::addDescriptor(std::string vName, std::shared_ptr<NumberDescriptor> descriptor) {
    if ( symTab.size() > 0 )
        (symTab.top())[vName] = std::move(descriptor);
    else
        globalSymTab[vName] = std::move(descriptor);
}

void SymTab::createEntryFor(std::string vName, int value) {
    if (debug)
        std::cout << "SymTab::createEntryFor(INT) ->" << value << "<-" << std::endl;
    auto descriptor = std::make_shared<NumberDescriptor>(TypeDescriptor::INTEGER);
    descriptor->_value.intValue = value;
    addDescriptor(vName, descriptor);
    //globalSymTab[vName] = std::move(descriptor);
}

void SymTab::createEntryFor(std::string vName, double value) {

    if (debug)
        std::cout << "SymTab::createEntryFor(DOUBLE) ->" << value << "<-" << std::endl;

    auto descriptor = std::make_shared<NumberDescriptor>(TypeDescriptor::DOUBLE);
    descriptor->_value.doubleValue = value;
    addDescriptor(vName, descriptor);
}

void SymTab::createEntryFor(std::string vName, bool value) {

    if (debug)
        std::cout << "SymTab::createEntryFor(BOOL) ->" << value << "<-" << std::endl;

    auto descriptor = std::make_shared<NumberDescriptor>(TypeDescriptor::BOOL);
    descriptor->_value.boolValue = (int) value;
    addDescriptor(vName, descriptor);
}

void SymTab::createEntryFor(std::string vName, std::string value) {
    if (debug)
        std::cout << "SymTab::createEntryFor(STRING) ->" << value << "<-" << std::endl;

    auto descriptor = std::make_shared<StringDescriptor>(TypeDescriptor::STRING);
    descriptor->_stringValue = value;
    if ( symTab.size() > 0 )
        (symTab.top())[vName] = std::move(descriptor);
    else
        globalSymTab[vName] = std::move(descriptor);
}

void SymTab::createEntryFor(std::string vName, const char* value) {
    createEntryFor(vName, std::string(value));
}

void SymTab::setValueFor(std::string vName, std::shared_ptr<TypeDescriptor> sp) {
    
    if ( symTab.size() > 0 )
        (symTab.top())[vName] = /*std::move(*/sp/*)*/;
    else
        globalSymTab[vName] = /*std::move(*/sp/*)*/;

    
    
    // globalSymTab[vName] = std::move(sp);
}

bool SymTab::isDefined(std::string vName) {
    
    // std::cout << "isDefined" << std::endl;

    if ( readFromPreviousScope ) {
        return previousScope.find(vName) != previousScope.end();
    }

    if ( symTab.size() > 0 )
        return (symTab.top()).find(vName) != (symTab.top()).end();
    return globalSymTab.find(vName) != globalSymTab.end();
}

bool SymTab::erase(std::string vName) {
    if (isDefined(vName)) {

        if ( symTab.size() > 0 ) {
            auto iterator = (symTab.top()).find(vName);
            (symTab.top()).erase(iterator);
        } else {
            auto iterator = globalSymTab.find(vName);
            globalSymTab.erase(iterator);
        }
        return true;
    }
    return false;
}

TypeDescriptor *SymTab::getValueFor(std::string vName) {

    if ( !isDefined(vName) ) {
        std::cout << "SymTab::getValueFor: " << vName << " has not been defined.\n";
        exit(1);
    } /*else {
        std::cout << "Is defined" << std::endl;
    }*/

    if (debug)
        std::cout << "SymTab::getValueFor: " << vName << "\n";
    
    if ( readFromPreviousScope ) {
        // std::cout << "Previous Scope Access" << std::endl;
        return previousScope.find(vName)->second.get();
    }

    if ( symTab.size() > 0 )
        return (symTab.top()).find(vName)->second.get();

    return globalSymTab.find(vName)->second.get();

}

void SymTab::openScope() {

    // std::cout << "open scope "<< std::endl;

    if ( symTab.size() == 0 ) {
        previousScope = globalSymTab;
    } else {
        previousScope = symTab.top();
    }
    // previousScope = (symTab.top());
    // std::cout << "still open scope" << std::endl;
    readFromPreviousScope = true;

    std::map<std::string, std::shared_ptr<TypeDescriptor>> newScope;
    symTab.push(newScope);
}

void SymTab::activateScope() {
    readFromPreviousScope = false;
}

void SymTab::closeScope() {

    if ( symTab.size() == 0 ) {
        std::cout << "SymTab::closeScope() -> can't close scope - stack size is zero" << std::endl;
        exit(1);
    }
    symTab.pop();

    readFromPreviousScope = true;
}

void SymTab::printIndividualTable(std::map<std::string, std::shared_ptr<TypeDescriptor>>& table, std::string scope) {

    // std::cout << scope << "PrintIndividualTable" << std::endl;
    // for( auto const& [key, val] : table ) {
    //     std::cout << scope << std::setw(15) << std::left << key << '\t';
    //     Descriptor::printValue(val.get());
    //     std::cout << std::endl;
    // }
}

void SymTab::dumpTable() {

    // std::string scope = "void SymTab::dumpTable()";
    // std::string spacing = "\t";
    // std::cout << scope << std::endl;

    // printIndividualTable(globalSymTab, "");
    // std::cout << std::endl;
    // // We nuke the stack for debugging purposes
    // // Stacks don't expose iterators
    // while ( symTab.size() > 0 ) {
    //     std::cout << spacing << "SCOPE START" << std::endl;
    //     spacing += '\t';
    //     printIndividualTable(symTab.top(), spacing);
    //     symTab.pop();
    //     std::cout << spacing << "Scope END" << std::endl;
    //     std::cout << std::endl;
    // }



}

// template<class _Ty> inline
//   constexpr typename remove_reference<_Ty>::type&&
//     move(_Ty&& _Arg) _NOEXCEPT
//   {
//     return (static_cast<typename remove_reference<_Ty>::type&&>(_Arg));
//   }

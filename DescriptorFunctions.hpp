#ifndef _DESCRIPTOR__FUNCTION__HPP
#define _DESCRIPTOR__FUNCTION__HPP
#include "Descriptor.hpp"

// #include <type_traits>

// Looks like SFINAE is just tag dispatch under the covers.

// template <class _Tp> struct __libcpp_is_integral                     : public false_type {};
// template <>          struct __libcpp_is_integral<bool>               : public true_type {};
// template <>          struct __libcpp_is_integral<char>               : public true_type {};
// template <>          struct __libcpp_is_integral<signed char>        : public true_type {};
// template <>          struct __libcpp_is_integral<unsigned char>      : public true_type {};
// template <>          struct __libcpp_is_integral<wchar_t>            : public true_type {};
// #ifndef _LIBCPP_HAS_NO_UNICODE_CHARS
// template <>          struct __libcpp_is_integral<char16_t>           : public true_type {};
// template <>          struct __libcpp_is_integral<char32_t>           : public true_type {};
// #endif  // _LIBCPP_HAS_NO_UNICODE_CHARS
// template <>          struct __libcpp_is_integral<short>              : public true_type {};
// template <>          struct __libcpp_is_integral<unsigned short>     : public true_type {};
// template <>          struct __libcpp_is_integral<int>                : public true_type {};
// template <>          struct __libcpp_is_integral<unsigned int>       : public true_type {};
// template <>          struct __libcpp_is_integral<long>               : public true_type {};
// template <>          struct __libcpp_is_integral<unsigned long>      : public true_type {};
// template <>          struct __libcpp_is_integral<long long>          : public true_type {};
// template <>          struct __libcpp_is_integral<unsigned long long> : public true_type {};
// #ifndef _LIBCPP_HAS_NO_INT128
// template <>          struct __libcpp_is_integral<__int128_t>         : public true_type {};
// template <>          struct __libcpp_is_integral<__uint128_t>        : public true_type {};
// #endif

// template <class _Tp> struct _LIBCPP_TEMPLATE_VIS is_integral
//     : public __libcpp_is_integral<typename remove_cv<_Tp>::type> {};



// template <class T, typename
//     std::enable_if<std::__and_<std::is_integral<T>::value, std::is_floating_point<T>::value>::type* = nullptr>
// inline bool compAll(T lhsVar, T rhsVar, const std::shared_ptr<Token> &t) {
//      if ( t->isRelGT() )
//         return lhsVar > rhsVar;
//     else if ( t->isRelLT() )
//         return lhsVar < rhsVar;
//     else if ( t->isRelGTE() )
//         return lhsVar >= rhsVar;
//     else if ( t->isRelLTE() )
//         return lhsVar <= rhsVar;
//     else if ( t->isRelEQ() )
//         return lhsVar == rhsVar;
//     else if ( t->isRelNotEQ() || t->isRelEQML() )
//         return lhsVar != rhsVar;

//     std::cout << "Condition Not Met -- Comparison::String::compString - no condition for token ";
//     t->print();
//     std::cout << std::endl;

//     return false;
// }


namespace Compare {

    namespace String {
        // Should use Template /w SFINAE
        inline bool compString(std::string lhsVar, std::string rhsVar, const std::shared_ptr<Token> &t) {

            if ( t->isRelGT() )
                return lhsVar > rhsVar;
            else if ( t->isRelLT() )
                return lhsVar < rhsVar;
            else if ( t->isRelGTE() )
                return lhsVar >= rhsVar;
            else if ( t->isRelLTE() )
                return lhsVar <= rhsVar;
            else if ( t->isRelEQ() )
                return lhsVar == rhsVar;
            else if ( t->isRelNotEQ() || t->isRelEQML() )
                return lhsVar != rhsVar;

            std::cout << "Condition Not Met -- Comparison::String::compString - no condition for token ";
            t->print();
            std::cout << std::endl;

            return false;
        }

    }

}


//Stack overflow is down so i am inlining everything as a quick fix
namespace Descriptor {

     namespace Int {

        inline NumberDescriptor *dieIfNotInt(TypeDescriptor *t) {

            if ( t->type() != TypeDescriptor::INTEGER ) {
                std::cout << "Fatal Error Descriptor::Int::dieIfNotInt" << std::endl;
                exit(1);
            }

            auto desc = dynamic_cast<NumberDescriptor *>(t);

            if ( desc == nullptr ) {
                std::cout << "Fatal Error .. Dynamic Cast Failed dieIfNotInt" << std::endl;
                exit(1);
            }

            return desc;
        }

        inline std::unique_ptr<TypeDescriptor> createIntDescriptor(int value) {
            auto desc = std::make_unique<NumberDescriptor>(TypeDescriptor::INTEGER);
            desc->_value.intValue = value;

            return desc;
        }

        inline void incrementByN(int n, TypeDescriptor *t) {

            auto desc = dieIfNotInt(t);
            desc->_value.intValue += n;
        }

        inline int getIntValue(TypeDescriptor *t) {

            auto desc = dieIfNotInt(t);
            return desc->_value.intValue;
        }

        inline void flipSignBit(TypeDescriptor *t) {

            auto desc = dieIfNotInt(t);
            desc->_value.intValue *= -1;
        }
    };

    namespace Bool {

        inline NumberDescriptor *dieIfNotBool(TypeDescriptor *t) {

            if ( t->type() != TypeDescriptor::BOOL ) {
                std::cout << "Fatal Error Descriptor::Bool::dieIfNotBool" << std::endl;
                exit(1);
            }

            auto desc = dynamic_cast<NumberDescriptor *>(t);

            if ( desc == nullptr ) {
                std::cout << "Fatal Error .. Dynamic Cast Failed dieIfNotBool" << std::endl;
                exit(1);
            }

            return desc;
        }

        inline std::unique_ptr<TypeDescriptor> createBooleanDescriptor(bool value) {

            auto desc = std::make_unique<NumberDescriptor>(TypeDescriptor::BOOL);
            desc->_value.boolValue = (int) value;
            return desc;
        }

        inline bool getBoolValue(TypeDescriptor *t) {

            auto desc = dieIfNotBool(t);
            return (bool) desc->_value.boolValue;
        }

    };

    namespace String {

        inline StringDescriptor *dieIfNotString(TypeDescriptor *t) {

            if ( t->type() != TypeDescriptor::STRING ) {
                std::cout << "Fatal Error Descriptor::Int::dieIfNotString" << std::endl;
                exit(1);
            }

            auto desc = dynamic_cast<StringDescriptor *>(t);

            if ( desc == nullptr ) {
                std::cout << "Fatal Error .. Dynamic Cast Failed dieIfNotBool" << std::endl;
                exit(1);
            }

            return desc;
        }

        inline std::unique_ptr<TypeDescriptor> createStringDescriptor(std::string value) {
            auto desc = std::make_unique<StringDescriptor>(TypeDescriptor::STRING);
            desc->_stringValue = value;
            return desc;
        }

        inline std::string getStringValue(TypeDescriptor *t) {

            auto desc = dieIfNotString(t);
            return desc->_stringValue;

        }

    };

    namespace Double {

        inline NumberDescriptor *dieIfNotDouble(TypeDescriptor *t) {

            if ( t->type() != TypeDescriptor::DOUBLE ) {
                std::cout << "Fatal Error Descriptor::Double::dieIfNotDouble.." << t->type() << std::endl;
                exit(1);
            }

            auto desc = dynamic_cast<NumberDescriptor *>(t);

            if ( desc == nullptr ) {
                std::cout << "Fatal Error .. Dynamic Cast Failed dieIfNotDouble" << std::endl;
                exit(1);
            }

            return desc;

        }

        inline double getDoubleValue(TypeDescriptor *t) {
            auto desc = dieIfNotDouble(t);
            return desc->_value.doubleValue;
        }

        inline std::unique_ptr<TypeDescriptor> createDoubleDescriptor(double value) {
            auto desc = std::make_unique<NumberDescriptor>(TypeDescriptor::DOUBLE);
            desc->_value.doubleValue = value;
            return desc;
        }

        inline void flipSignBit(TypeDescriptor *t) {

            auto desc = dieIfNotDouble(t);
            desc->_value.doubleValue *= -1;
        }
    };



    inline void printValue(TypeDescriptor *desc) {

        // std::cout << "printValue" << std::endl;

        if ( desc->type() == TypeDescriptor::INTEGER
        ||   desc->type() == TypeDescriptor::DOUBLE
        ||   desc->type() == TypeDescriptor::BOOL ) {
            // std::cout <<"int / double / bool" << std::endl;
            NumberDescriptor *nDesc = dynamic_cast<NumberDescriptor *>(desc);
            if( nDesc->type() == TypeDescriptor::INTEGER )
                std::cout << nDesc->_value.intValue;
            else if( nDesc->type() == TypeDescriptor::DOUBLE )
                std::cout << nDesc->_value.doubleValue;
            else if( nDesc->type() == TypeDescriptor::BOOL )
                std::cout << nDesc->_value.boolValue;
            else
                std::cout << "Misconfigured union type." << std::endl;

            return;
        }
        else if ( desc->type() == TypeDescriptor::STRING ) {
            // std::cout << "Stringtype" << std::endl;
            StringDescriptor *sDesc = dynamic_cast<StringDescriptor *>(desc);
            if ( sDesc != nullptr ) {
                std::cout << sDesc->_stringValue;
            }

            return;
        } else if ( 
            desc->type() == TypeDescriptor::ARRAY_INT
        ||  desc->type() == TypeDescriptor::ARRAY_DOUBLE
        ||  desc->type() == TypeDescriptor::ARRAY_BOOL
        ||  desc->type() == TypeDescriptor::ARRAY_STRING) { 

            // std::cout << "At array" << std::endl;
            ArrayDescriptor *aDesc = dynamic_cast<ArrayDescriptor *>(desc);
            if ( aDesc != nullptr ) {
                aDesc->print();
            }
            return;
        } else {
            std::cout << "NOTHING: " << desc->type() << std::endl;
        }
    }

    inline std::unique_ptr<TypeDescriptor> copyReferencePtr(TypeDescriptor *ref) {

        if (ref->type() == TypeDescriptor::INTEGER) {
            auto desc = std::make_unique<NumberDescriptor>(TypeDescriptor::INTEGER);
            desc->_value.intValue = dynamic_cast<NumberDescriptor *>(ref)->_value.intValue; //risky

            return desc;

        } else if (ref->type() == TypeDescriptor::DOUBLE) {
            auto desc = std::make_unique<NumberDescriptor>(TypeDescriptor::DOUBLE);
            desc->_value.doubleValue = dynamic_cast<NumberDescriptor *>(ref)->_value.doubleValue;

            return desc;

        } else if (ref->type() == TypeDescriptor::BOOL) {
            auto desc = std::make_unique<NumberDescriptor>(TypeDescriptor::BOOL);
            desc->_value.boolValue = dynamic_cast<NumberDescriptor *>(ref)->_value.boolValue;

            return desc;

        } else if (ref->type() == TypeDescriptor::STRING) {
            auto desc = std::make_unique<StringDescriptor>(TypeDescriptor::STRING);
            desc->_stringValue = dynamic_cast<StringDescriptor *>(ref)->_stringValue;

            return desc;

        } else if (ref->type() == TypeDescriptor::ARRAY_INT || ref->type() == TypeDescriptor::ARRAY_DOUBLE ||
                   ref->type() == TypeDescriptor::ARRAY_BOOL || ref->type() == TypeDescriptor::ARRAY_STRING ||
                   ref->type() == TypeDescriptor::NOTY_ARRAY) {
            auto desc = std::make_unique<ArrayDescriptor>(ref->type());
            auto casted = dynamic_cast<ArrayDescriptor *>(ref);
            desc->_integerArray = casted->_integerArray;
            desc->_stringArray =  casted->_stringArray;
            desc->_doubleArray = casted->_doubleArray;
            desc->_boolArray = casted->_boolArray;
            return desc;

        } else {
            std::cout << "Error in copyReferencePtr...ref type is " << ref->type() << std::endl;
            exit(1);
            // Silence Warnings
            return nullptr;
        }
    }


    inline bool validTypeOp(TypeDescriptor *t1, TypeDescriptor *t2) {

        auto lhsType = t1->type();
        auto rhsType = t2->type();

        if ( lhsType == rhsType )
            return true;
        else if ( lhsType == TypeDescriptor::BOOL && ( rhsType == TypeDescriptor::DOUBLE || rhsType == TypeDescriptor::INTEGER ))
            return true;
        else if ( rhsType == TypeDescriptor::BOOL && ( lhsType == TypeDescriptor::DOUBLE || lhsType == TypeDescriptor::INTEGER ))
            return true;
        else if ( (rhsType == TypeDescriptor::INTEGER && lhsType == TypeDescriptor::DOUBLE) || (lhsType == TypeDescriptor::INTEGER && rhsType == TypeDescriptor::DOUBLE) )
            return true;
        else return false;
    }

    inline std::unique_ptr<TypeDescriptor> negateDescriptor(TypeDescriptor *t) {

        if (t->type() != TypeDescriptor::BOOL) {

            if ( t->type() == TypeDescriptor::DOUBLE || t->type() == TypeDescriptor::INTEGER ) {
                auto ptr = dynamic_cast<NumberDescriptor *>(t);
                return Bool::createBooleanDescriptor( ptr->_value.intValue == 0 ? true : false );
            }
            return Bool::createBooleanDescriptor(false);
        }

        auto casted = dynamic_cast<NumberDescriptor *>(t);
        return Bool::createBooleanDescriptor( !casted->_value.boolValue );
    }

    inline std::unique_ptr<TypeDescriptor> andDescriptor(TypeDescriptor *lhs, TypeDescriptor *rhs) {

        if ( (lhs->type() == TypeDescriptor::BOOL || lhs->type() == TypeDescriptor::INTEGER || lhs->type() == TypeDescriptor::DOUBLE) &&
             (rhs->type() == TypeDescriptor::BOOL || rhs->type() == TypeDescriptor::INTEGER || rhs->type() == TypeDescriptor::DOUBLE) ) {

            auto lhsPtr = dynamic_cast<NumberDescriptor *>(lhs);
            auto rhsPtr = dynamic_cast<NumberDescriptor *>(rhs);

            // Don't care if its int double -- we just want to see bits.
            int lhsValue = lhsPtr->_value.intValue;
            int rhsValue = rhsPtr->_value.intValue;


            return (( lhsValue > 0 && rhsValue > 0)/* || ( lhsValue < 0 && rhsValue < 0 ) || ( lhsValue == rhsValue )*/)
                ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);
        } else {
            std::cout << "andDescriptor bad types - quitting" << std::endl;
            exit(1);
            return nullptr;
        }
    }

    inline std::unique_ptr<TypeDescriptor> orDescriptor(TypeDescriptor *lhs, TypeDescriptor *rhs) {
        //Same as and - offset load 2 function later
        if ( (lhs->type() == TypeDescriptor::BOOL || lhs->type() == TypeDescriptor::INTEGER || lhs->type() == TypeDescriptor::DOUBLE) &&
             (rhs->type() == TypeDescriptor::BOOL || rhs->type() == TypeDescriptor::INTEGER || rhs->type() == TypeDescriptor::DOUBLE) ) {

            auto lhsPtr = dynamic_cast<NumberDescriptor *>(lhs);
            auto rhsPtr = dynamic_cast<NumberDescriptor *>(rhs);

            // Don't care if its int double -- we just want to see bits.
            int lhsValue = lhsPtr->_value.intValue;
            int rhsValue = rhsPtr->_value.intValue;

            return ( lhsValue > 0 || rhsValue > 0 ) ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);
        } else {
            std::cout << "andDescriptor bad types - quitting" << std::endl;
            exit(1);
            return nullptr;
        }

    }

    inline void grabValueFromNumberDescriptor(double &fill, NumberDescriptor *ptr) {

        if (ptr->type() == TypeDescriptor::INTEGER) {
            fill = (float) ptr->_value.intValue;
        } else if (ptr->type() == TypeDescriptor::DOUBLE) {
            fill = ptr->_value.doubleValue;
        } else if (ptr->type() == TypeDescriptor::BOOL) {
            fill = 1.;
        }
    }

    inline std::unique_ptr<TypeDescriptor> relOperatorDescriptor(TypeDescriptor *lhs, TypeDescriptor *rhs, const std::shared_ptr<Token> &t) {

        // Check for string concat
        if ( lhs->type() == TypeDescriptor::STRING && rhs->type() == TypeDescriptor::STRING && t->isAdditionOperator() ) {

            auto lhsPtr = dynamic_cast<StringDescriptor *>(lhs);
            auto rhsPtr = dynamic_cast<StringDescriptor *>(rhs);

            if ( lhsPtr == nullptr || rhsPtr == nullptr ) {
                std::cout << "Descriptor::relOperatorDescriptor - Error casting to string descriptor" << std::endl;
                exit(1);
            }

            return String::createStringDescriptor(lhsPtr->_stringValue + rhsPtr->_stringValue);
        }

        if ( lhs->type() == TypeDescriptor::DOUBLE && rhs->type() == TypeDescriptor::DOUBLE ) {
            return Double::createDoubleDescriptor(Double::getDoubleValue(lhs) + Double::getDoubleValue(rhs));
        }

        // Only support on integers from now on!
        if ( lhs->type() != TypeDescriptor::INTEGER || rhs->type() != TypeDescriptor::INTEGER ) {

            std::cout << "Unsupported Type Returning Garbage Value 1" << std::endl;

            return Int::createIntDescriptor(1);
        }

        auto lhsPtr = dynamic_cast<NumberDescriptor*>(lhs);
        auto rhsPtr = dynamic_cast<NumberDescriptor*>(rhs);

        int lhsVar = lhsPtr->_value.intValue;
        int rhsVar = rhsPtr->_value.intValue;

        if( t->isAdditionOperator() )
            return Int::createIntDescriptor(lhsVar + rhsVar);
        else if(t->isSubtractionOperator())
            return Int::createIntDescriptor(lhsVar - rhsVar);
        else if(t->isMultiplicationOperator())
            return Int::createIntDescriptor(lhsVar * rhsVar);
        else if(t->isDivisionOperator()) {
            if ( rhsVar == 0 ) {
                std::cout << "Warning: Division by zero is undefined" << std::endl;
                exit(1);
            }
            return Int::createIntDescriptor(lhsVar / rhsVar); // division by zero?
        }
        else if( t->isModuloOperator() ) {
            if ( lhsPtr->type() != TypeDescriptor::INTEGER || rhs->type() != TypeDescriptor::INTEGER ) {
                std::cout << "Error: invalid operands to binary expression NOTINT and NOTINT" << std::endl;
                exit(1);
            }
            return Int::createIntDescriptor(lhsVar % rhsVar);
        }
        else {
            std::cout << "ERRRRRR" << std::endl;
            exit(1);
            return nullptr;
        }
    }

    inline std::unique_ptr<TypeDescriptor> comparisonDescriptor(TypeDescriptor *lhs, TypeDescriptor *rhs, const std::shared_ptr<Token> &t) {

        if ( lhs->type() == TypeDescriptor::STRING && rhs->type() == TypeDescriptor::STRING ) {

            std::string lhsVar = String::getStringValue(lhs);
            std::string rhsVar = String::getStringValue(rhs);

            return Compare::String::compString(lhsVar, rhsVar, t)
                ?  Bool::createBooleanDescriptor(true)
                :  Bool::createBooleanDescriptor(false);

        }

        // // We don't support comparisonOp on strings yet
        // if (lhs->type() == TypeDescriptor::STRING || rhs->type() == TypeDescriptor::STRING) {
        //     std::cout << "Unsupported operator on type -> returning garbage value" << std::endl;
        //     return Bool::createBooleanDescriptor(true);
        // }

        auto lhsPtr = dynamic_cast<NumberDescriptor*>(lhs);
        auto rhsPtr = dynamic_cast<NumberDescriptor*>(rhs);

        double lhsVar;
        grabValueFromNumberDescriptor(lhsVar, lhsPtr);

        double rhsVar;
        grabValueFromNumberDescriptor(rhsVar, rhsPtr);

        if ( t->isRelGT() )
            return lhsVar > rhsVar ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);
        else if ( t->isRelLT() )
            return lhsVar < rhsVar ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);
        else if ( t->isRelGTE() )
            return lhsVar >= rhsVar ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);
        else if ( t->isRelLTE() )
            return lhsVar <= rhsVar ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);
        else if ( t->isRelEQ() )
            return lhsVar == rhsVar ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);
        else if ( t->isRelNotEQ() || t->isRelEQML() )
            return lhsVar != rhsVar ? Bool::createBooleanDescriptor(true) : Bool::createBooleanDescriptor(false);

        return nullptr;
    }
};


#endif

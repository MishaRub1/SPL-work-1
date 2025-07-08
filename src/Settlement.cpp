#include "Settlement.h"
using std::string;

//Constructor
Settlement::Settlement(const string &name, SettlementType type)
    :name(name), type(type){}

Settlement::Settlement(const Settlement &settlement)
    : name(settlement.getName()), type(settlement.getType()) {}

//Getter's
const string &Settlement::getName() const {
    return name;
}

SettlementType Settlement::getType() const {
    return type;
}

const string Settlement::toString() const {
    string stringType;
    switch (type){
        case SettlementType::VILLAGE:
            stringType = "Village";
            break;
        case SettlementType::CITY:
            stringType = "City";
            break;
        case SettlementType::METROPOLIS:
            stringType = "Metropolis";
            break;
    }
    return "Settlement " + name + " is a " + stringType;
}



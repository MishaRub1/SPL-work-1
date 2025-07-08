#include "Facility.h"
#include <sstream>
#include <iostream>
using std::string;

//FacilityType Constructor
FacilityType::FacilityType(const string &name, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    :name(name), category(category), price(price),
    lifeQuality_score(lifeQuality_score), economy_score(economy_score), environment_score(environment_score){}

FacilityType *FacilityType::clone() const {
    return new FacilityType(*this);
}

bool FacilityType::operator==(const FacilityType& other) const {
    return this->name == other.name; // Compare by name or another unique field
}

// Getters for FacilityType
const string &FacilityType::getName() const{
    return name;
}

int FacilityType::getCost() const {
    return price;
}

FacilityCategory FacilityType::getCategory() const {
    return category;
}

int FacilityType::getEconomyScore() const {
    return economy_score;
}

int FacilityType::getEnvironmentScore() const {
    return environment_score;
}

int FacilityType::getLifeQualityScore() const {
    return lifeQuality_score;
}

// Facility constructor
Facility::Facility(const FacilityType &type, const string &settlementName)
    :FacilityType(type), settlementName(settlementName), status(FacilityStatus::UNDER_CONSTRUCTIONS), timeLeft(price){}

Facility::Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price,const int lifeQuality_score, const int economy_score, const int environment_score) 
                   : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score),
                     settlementName(settlementName),
                     status(FacilityStatus::UNDER_CONSTRUCTIONS),
                     timeLeft(price) {}


//Facility getter's and setter's
const string &Facility::getSettlementName() const {
    return settlementName;
}

const FacilityStatus& Facility::getStatus() const {
    return status;
}

void Facility::setStatus(FacilityStatus newStatus){
    this->status = newStatus;
}

int Facility::getTimeLeft() const {
    return timeLeft;
}

void Facility::ReduceTimeLeft() {
    timeLeft--;
}

FacilityStatus Facility::step(){
    if (getTimeLeft() > 0) {
        ReduceTimeLeft();
    }
    if (getTimeLeft() == 0) {
        setStatus(FacilityStatus::OPERATIONAL);
        return FacilityStatus::OPERATIONAL;
    }
    else {
        return FacilityStatus::UNDER_CONSTRUCTIONS;
    }
}

const string Facility::toString() const {
    std::ostringstream oss;
    oss << "Facility Name: " << getName()
        << ", Settlement: " << settlementName
        <<", Status: " << (getStatus() == FacilityStatus::UNDER_CONSTRUCTIONS ? "Under Construction" : "Operational")
        <<", Time Left: " << getTimeLeft();
    return oss.str();
}
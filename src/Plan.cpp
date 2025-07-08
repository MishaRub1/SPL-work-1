#include "Plan.h"
#include "Facility.h"
#include <iostream>
#include <sstream>

//Plan constructor
Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId),
      settlement(settlement),
      selectionPolicy(selectionPolicy),
      status(PlanStatus::AVALIABLE),
      facilities(), underConstruction(),
      facilityOptions(facilityOptions),
      life_quality_score(0),
      economy_score(0),
      environment_score(0){}

//Plan rule of 5

//Delete:
Plan::~Plan() {
    delete selectionPolicy;

    for (Facility *facility : facilities) {
        delete facility;
    }
    facilities.clear();

    for (Facility *facility : underConstruction) {
        delete facility;
    }
    underConstruction.clear();
}

//copy constructor:
Plan::Plan(const Plan &other) 
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy->clone()),
      status(other.status),
      facilities(),
      underConstruction(),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score) {

        for (const Facility *facility : other.facilities) {
            facilities.push_back(new Facility(*facility));
        }

        for (const Facility *facility : other.underConstruction) {
            underConstruction.push_back(new Facility(*facility));
        }
      }

//Move Constractor
Plan::Plan(Plan &&other) noexcept
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy),
      status(other.status),
      facilities(std::move(other.facilities)),
      underConstruction(std::move(other.underConstruction)),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score) {
        other.selectionPolicy = nullptr;
      }

//plan getters
int Plan::getlifeQualityScore() const {
    return life_quality_score;
}

int Plan::getEconomyScore() const {
    return economy_score;
}

int Plan::getEnvironmentScore() const {
    return environment_score;
}

int Plan::getConstructionLimit(const Settlement& settlement) {
    if (settlement.getType() == SettlementType::VILLAGE){
        return 1;
    }
    else if (settlement.getType() == SettlementType::CITY){
        return 2;
    }
    else if (settlement.getType() == SettlementType::METROPOLIS){
        return 3;
    }
    return 0; //invalid settlement
    
}

int Plan::getId() const {
    return plan_id;
}

//plan methods
void Plan::step() {
    int constructionLimit = getConstructionLimit(settlement); 

    while (underConstruction.size() < static_cast<size_t>(constructionLimit)) {
        try {
            if (facilityOptions.empty()) {
                std::cerr << "No facilities left for selection" << std::endl;
                break;
            }

            const FacilityType& selectedFacilityType = selectionPolicy->selectFacility(facilityOptions);
            Facility* newFacility = new Facility(selectedFacilityType, settlement.getName());
            underConstruction.push_back(newFacility);
            BalancedSelection* b = dynamic_cast<BalancedSelection*> (selectionPolicy);
            if (b) b->updateScore(selectedFacilityType);
        }
        catch (std::exception& e) {
            std::cerr << "Error during facility selection: " << e.what() << std::endl;
            break; 
        }
    }
        
    for (auto it = underConstruction.begin(); it != underConstruction.end();) {
        Facility *facility = *it;
        facility->step();

        if (facility->getTimeLeft() == 0) {
            //moving the facility to operational list
            facility->setStatus(FacilityStatus::OPERATIONAL);
            facilities.push_back(facility);

            //updating scores
            life_quality_score += facility->getLifeQualityScore();
            economy_score += facility->getEconomyScore();
            environment_score += facility->getEnvironmentScore();

            it = underConstruction.erase(it);
        }

        else {
            ++it;
        }
    }

    //updating the plan
    if (underConstruction.size() >= static_cast<size_t>(getConstructionLimit(settlement))) {
        status = PlanStatus::BUSY;
    }
    else {
        status = PlanStatus::AVALIABLE;
    }
 }

 void Plan::setSelectionPolicy(SelectionPolicy *selectionPolicy) {
    std::cout << "Plan: " << this->plan_id <<std::endl;
    std::cout << "Current policy: " <<this->selectionPolicy->toString() << std::endl;
    delete this->selectionPolicy;
    this->selectionPolicy = selectionPolicy;
    std::cout << "Updated to: " <<this->selectionPolicy->toString() << std::endl;
 }

const string Plan::toString() const {
    std::ostringstream oss;

    oss << "PlanID: " << plan_id << "\n";
    oss << "SettlementName: " << settlement.getName() << "\n";
    oss << "PlanStatus: " << (status == PlanStatus::AVALIABLE ? "Available" : "Busy") << "\n";
    oss << "SelectionPolicy: " << selectionPolicy->toString() << "\n";
    oss << "LifeQualityScore: " << life_quality_score << "\n";
    oss << "EconomyScore: " << economy_score << "\n";
    oss << "EnvironmentScore: " << environment_score << "\n";

    oss << "Operational Facilities:\n";
    for (const auto &Facility : facilities) {
        oss << " - " << Facility->toString() << "\n";
    }

    oss << "Under Constructions facilities:\n";
    for (const auto &Facility : underConstruction) {
        oss << " - " << Facility->toString() << "\n";
    }

    return oss.str();
}

const string Plan::resultPrint() const {
    std::ostringstream oss;

    oss << "PlanID: " << plan_id << "\n";
    oss << "SettlementName: " << settlement.getName() << "\n";
    oss << "PlanStatus: " << (status == PlanStatus::AVALIABLE ? "Available" : "Busy") << "\n";
    oss << "SelectionPolicy: " << selectionPolicy->toString() << "\n";
    oss << "LifeQualityScore: " << life_quality_score << "\n";
    oss << "EconomyScore: " << economy_score << "\n";
    oss << "EnvironmentScore: " << environment_score << "\n";

    return oss.str();
}

//comparing policies
bool Plan::isSamePolicy(const SelectionPolicy *policy) const {
    //if there are no policies
    if (!selectionPolicy || !policy) {
        return false;
    }
    return selectionPolicy->toString() == policy->toString();
}

void Plan::printStatus() {
    if (status == PlanStatus::AVALIABLE) {
        std::cout << "Status: Available" << std::endl;
    }
    else {
        std::cout << "Status: Busy" << std::endl;
    }
}

void Plan::addFacility(Facility *facility) {
    facilities.push_back(facility);
}

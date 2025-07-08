#include "Action.h"
#include "Simulation.h"
#include "Settlement.h"
#include "Facility.h"
#include "Plan.h"
#include "SelectionPolicy.h"
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;
extern Simulation* backup;

BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::COMPLETED) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(std::string errorMsg) {
    status = ActionStatus::ERROR;
    this->errorMsg = errorMsg;
}

Close::Close() {}

void Close::act(Simulation &simulation) {
    std::cout << "Simulation Results: " << std::endl;

    for (const Plan &plan : simulation.getPlans()) {
        std::cout << plan.resultPrint();
        std::cout << "" << endl;
    }
    complete();
    simulation.addAction(this);
}

const std::string Close::toString() const {
    return "Close";
}

Close* Close::clone() const {
    return new Close(*this);
}

//SimulateStep constructor
SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {
    if (numOfSteps <= 0) {
        throw std::invalid_argument("Error: number of steps must be positive");
    }
}

void SimulateStep::act(Simulation &simulation) {
    for (int i = 0; i<numOfSteps; i++) {
        simulation.step();
    }
    complete();
    simulation.addAction(this);
}

const string SimulateStep::toString() const {
    return "SimulateStep: steps = " + std::to_string(numOfSteps);
}

SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
}

//addSettlement constructor
AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType)
    : settlementName(settlementName), settlementType(settlementType) {}

void AddSettlement::act(Simulation &simulation) {
    if (simulation.isSettlementExists(settlementName)) {
        error("Settlement already exists");
        return;
    }
    
    Settlement *newSettlement = new Settlement(settlementName, settlementType);
        
    if (simulation.addSettlement(newSettlement)) {
        complete();
        simulation.addAction(this);
    }
    else {
        delete newSettlement;
        error("Error adding settlement.");
    }
    
}

const string AddSettlement::toString() const {
    return "Add settlement";
}

AddSettlement *AddSettlement::clone() const {
    return new AddSettlement(*this);
}

//addPlan
AddPlan::AddPlan(const std::string &settlementName, const std::string &selectionPolicy)
    : settlementName(settlementName), selectionPolicy(selectionPolicy) {}


void AddPlan::act(Simulation &simulation) {
    try {
        if(!simulation.isSettlementExists(settlementName)) {
            error("Error: Settlement does not exist.");
            return;
        }
    
        Settlement &settlement = simulation.getSettlement(settlementName);
        SelectionPolicy *policy = simulation.createSelectionPolicy(selectionPolicy);

        if (!policy) {
            error("Error: Invalid selection policy.");
            return;
        }

        simulation.addPlan(settlement, policy);
        complete();
        simulation.addAction(this);
    }
    catch (const std::exception &e) {
        error(e.what());
    }
}

const string AddPlan::toString() const {
    return "AddPlan: settlement = " + settlementName + ", policy =  " + selectionPolicy;
}

AddPlan *AddPlan::clone() const {
    return new AddPlan(*this);
}

//Add facility
AddFacility::AddFacility(const std::string &facilityName, FacilityCategory facilityCategory, int price, int lifeQualityScore, int economyScore, int environmentScore)
    : facilityName(facilityName), facilityCategory(facilityCategory), price(price),
      lifeQualityScore(lifeQualityScore), economyScore(economyScore), environmentScore(environmentScore) {}

void AddFacility::act(Simulation &simulation) {
    try {
        //check for duplicates
        for (const FacilityType &existingFacility : simulation.getFacilitiesOptions()) {
            if (existingFacility.getName() == facilityName) {
                error("Error: Facility already exists. ");
                return;
            }
        }

        //validating values
        if (price <= 0 || lifeQualityScore < 0 || economyScore < 0 || environmentScore < 0) {
            error("Error: facility attributes can't be negative");
            return;
        }

        FacilityType newFacility(facilityName, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
        simulation.addFacility(newFacility);
        simulation.addAction(this);
        complete();
    }
    catch (const std::exception &e){
        error(e.what());
    }
}

const std::string AddFacility::toString() const {
    return "Add Facility";
}

AddFacility* AddFacility::clone() const {
    return new AddFacility(*this);
}

//change plan
ChangePlanPolicy::ChangePlanPolicy(int planId, const string &newPolicy)
    : planId(planId), newPolicy(newPolicy) {}

void ChangePlanPolicy::act(Simulation &simulation) {
    try {
        Plan &plan = simulation.getPlan(planId);
        SelectionPolicy *policy = simulation.createSelectionPolicy(newPolicy);

        if (!policy) {
            error("Error: invalid selection policy");
            return;
        }

        if (plan.isSamePolicy(policy)) {
            delete policy;
            error("Error: new policy is the same as the current");
            return;
        }

        plan.setSelectionPolicy(policy);
        simulation.addAction(this);
        complete();
    }
    catch (const std::exception &e) {
        error(e.what());
    }
}

const std::string ChangePlanPolicy::toString() const {
    return "Change Plan Policy";
}

ChangePlanPolicy *ChangePlanPolicy::clone() const {
    return new ChangePlanPolicy(*this);
}

PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation) {
    try {
        for (Plan plan : simulation.getPlans()) {
            if (planId == plan.getId()) {
                Plan &plan = simulation.getPlan(planId);
                std::cout << plan.toString() << std::endl;
                simulation.addAction(this);
                complete();
            }
        }
        
    }
    catch (const std::exception &e) {
        error(e.what());
    }
}

PrintPlanStatus* PrintPlanStatus::clone() const {
    return new PrintPlanStatus(*this);
}

const string PrintPlanStatus::toString() const {
    return "PrintPlanStatus: " + std::to_string(planId);
}

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Simulation &simulation) {
    try {
        const auto &actionlog = simulation.getActionsLog();
        std::cout << "Actions Log:\n";

        for (const auto *action : actionlog) {
            std::cout << action->toString() << " - Status: "
            << (action->getStatus() == ActionStatus::COMPLETED ? "Completed" : "Error") << "\n";
        }
        complete();
    }
    catch (const std::exception &e) {
        error(e.what());
    }
}

const string PrintActionsLog::toString() const {
    return "PrintActionsLog";
}

PrintActionsLog* PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

//Backup and Restore
BackupSimulation::BackupSimulation() {}

void BackupSimulation::act(Simulation &simulation) {

    if (backup != nullptr) {
        delete backup;
    }

    backup = new Simulation(simulation);
    complete();
    simulation.addAction(this);
}

BackupSimulation *BackupSimulation::clone() const {
    return new BackupSimulation(*this);
}

const string BackupSimulation::toString() const {
    return "Backup";
}

RestoreSimulation::RestoreSimulation() {}

void RestoreSimulation::act(Simulation &simulation) {
    if (backup == nullptr) {
        error("No backup available");
        return;
    }

    // simulation.clearPlans();
    // simulation.clearSettlements();

    // simulation = *backup;
    std::swap(simulation, *backup);

    complete();
    simulation.addAction(this);
}

RestoreSimulation *RestoreSimulation::clone() const {
    return new RestoreSimulation(*this);
}

const string RestoreSimulation::toString() const {
    return "Restore";
}
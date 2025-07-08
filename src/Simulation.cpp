#include "Simulation.h"
#include "Action.h"
#include "Auxiliary.h"
#include <fstream>
#include <iostream>
#include <vector>

//Constructor
Simulation::Simulation(const string &configFilePath) :isRunning(false), planCounter(0),
    actionsLog(), plans(), settlements(), facilitiesOptions(){
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Can't open config file: " << configFilePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        std::vector<std::string> args = Auxiliary::parseArguments(line);

        if (args.empty()) {
            continue;
        }
        if (args[0] == "#") {
            continue;
        }
        if (args[0] == "settlement") {
            SettlementType type = static_cast<SettlementType>(std::stoi(args[2]));
            Settlement *settlement = new Settlement(args[1], type);
            addSettlement(settlement);
        }
        else if (args[0] == "facility") {
            FacilityCategory category = static_cast<FacilityCategory>(std::stoi(args[2]));
            FacilityType facility(args[1], category, std::stoi(args[3]), std::stoi(args[4]), std::stoi(args[5]), std::stoi(args[6]));
            addFacility(facility);
        }
        else if (args[0] == "plan") {
            Settlement &settlement = getSettlement(args[1]);
            SelectionPolicy *policy = createSelectionPolicy(args[2]);
            if (!policy) {
                std::cerr << "Error creating selection policy for line: " << line << std::endl;
                continue;
            }
            addPlan(settlement, policy);
        }
        else {
            std::cerr << "Warning: Unknown configuration line: " << line << std::endl;
        }
    }

    configFile.close();
}

//Rule Of 5

//Delete
Simulation::~Simulation() {

    for (Settlement *settlement : settlements) {
        delete settlement;
    }
    settlements.clear();

    for (BaseAction *action : actionsLog) {
        delete action;
    }
    actionsLog.clear();
}

//Copy Constructor
Simulation::Simulation(const Simulation &other) 
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions(other.facilitiesOptions) {

        for (const Settlement *settlement : other.settlements) {
            settlements.push_back(new Settlement(*settlement));
        }

        for (const BaseAction *action : other.actionsLog) {
            actionsLog.push_back(action->clone());
        }

        for (const Plan &plan : other.plans) {
            plans.push_back(plan);
        }
      }

//Copy Assignment operator
Simulation &Simulation::operator=(const Simulation &other) {
    if (this != &other) {
        //cleaning existing resources
        for (Settlement *settlement : settlements) {
            delete settlement;
        }
        settlements.clear();

        for (BaseAction *action : actionsLog) {
            delete action;
        }
        actionsLog.clear();

        plans.clear();

        //copy itself
        isRunning = other.isRunning;
        planCounter = other.planCounter;
        facilitiesOptions.clear();
        for (const auto &facility : other.facilitiesOptions) {
            facilitiesOptions.emplace_back(facility); 
        }

        for (const Settlement *settlement : other.settlements) {
            settlements.push_back(new Settlement(*settlement));
        }

        for (const BaseAction *action : other.actionsLog) {
            actionsLog.push_back(action->clone());
        }

        for (const Plan &plan : other.plans) {
            plans.push_back(plan);
        }
    }
    return *this;
}

//Move Constructor
Simulation::Simulation(Simulation &&other) noexcept
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(std::move(other.actionsLog)),
      plans(std::move(other.plans)),
      settlements(std::move(other.settlements)),
      facilitiesOptions(std::move(other.facilitiesOptions)) {
        other.isRunning = false;
        other.planCounter = 0;
      }

//Move Assignment operator
Simulation &Simulation::operator=(Simulation &&other) noexcept {
    if (this != &other) {
        //cleaning existing resources
        for (Settlement *settlement : settlements) {
            delete settlement;
        }
        settlements.clear();

        for (BaseAction *action : actionsLog) {
            delete action;
        }
        actionsLog.clear();

        plans.clear();

        isRunning = other.isRunning;
        planCounter = other.planCounter;
        facilitiesOptions = std::move(other.facilitiesOptions);
        settlements = std::move(other.settlements);
        actionsLog = std::move(other.actionsLog);
        plans = std::move(other.plans);

        other.isRunning = false;
        other.planCounter = 0;
    }
    return *this;
}
//getter's
Settlement &Simulation::getSettlement(const std::string &name) {
    for (auto &settlement : settlements) {
        if (settlement->getName() == name) {
            return *settlement;
        }
    }
    throw std::runtime_error("Settlement" + name + " was not found");
}



const std::vector<FacilityType>& Simulation::getFacilitiesOptions() const {
    return facilitiesOptions;
}

const std::vector<BaseAction*>& Simulation::getActionsLog() const {
    return actionsLog;
}

Plan &Simulation::getPlan(const int planId) {

    for (auto &plan : plans) {
        if (plan.getId() == planId) {
            return plan;
        }
    }

    throw std::runtime_error("Plan not found");
}

const std::vector<Plan> &Simulation::getPlans() const {
    return plans;
}

//methods
void Simulation::start() {
    open();
    std::cout << "The simulation has started" << std::endl;

    while (isRunning) {
        std::cout << "Enter an action: ";
        std::string inputLine;
        std::getline(std::cin, inputLine);
        std::vector<std::string> args = Auxiliary::parseArguments(inputLine);

        if (args.empty()) {
            std::cerr << "Error: No action provided. " << std::endl;
            continue;
        }

        try {
            if (args[0] == "#") {
                continue;
            }
            if (args[0] == "step") {
                if (args.size() < 2) {
                    throw std::runtime_error("Error: Invalid step command format");
                }
                int steps = std::stoi(args[1]);
                SimulateStep stepAction(steps);
                stepAction.act(*this);
            }
            else if (args[0] == "plan") {
                if (args.size() < 3) {
                    throw std::runtime_error("Error: Invalid plan command format");
                }
                AddPlan addPlanAction(args[1], args[2]);
                addPlanAction.act(*this);
            }
            else if (args[0] == "settlement") {
                if (args.size() < 3) {
                    throw std::runtime_error("Error: invalid settlement command format");
                }
                SettlementType type = static_cast<SettlementType>(std::stoi(args[2]));
                AddSettlement addSettlementAction(args[1], type);
                addSettlementAction.act(*this);
            }
            else if (args[0] == "facility") {
                if (args.size() < 7) {
                    throw std::runtime_error("Error: invalid facility command format");
                }
                FacilityCategory category = static_cast<FacilityCategory>(std::stoi(args[2]));
                AddFacility addFacilityAction(args[1], category, std::stoi(args[3]), std::stoi(args[4]), std::stoi(args[5]), std::stoi(args[6]));
                addFacilityAction.act(*this);
            }
            else if (args[0] == "planStatus") {
                if (args.size() < 2) {
                    throw std::runtime_error("Error: invalid planstatus command format");
                }
                int planId = std::stoi(args[1]);
                PrintPlanStatus printPlanStatusAction(planId);
                printPlanStatusAction.act(*this);
            }
            else if (args[0] == "log") {
                PrintActionsLog printActionsLogAction;
                printActionsLogAction.act(*this);
            }
            else if (args[0] == "changePolicy") {
                if (args.size() < 3) {
                    throw std::runtime_error("Error: invalid changepolicy command format");
                }
                int planId = std::stoi(args[1]);
                ChangePlanPolicy changePlanPolicyAction(planId, args[2]);
                changePlanPolicyAction.act(*this);
            }
            else if (args[0] == "close") {
                Close closeAction;
                closeAction.act(*this);
                close();
            }
            else if (args[0] == "backup") {
                BackupSimulation backupAction;
                backupAction.act(*this);
            }
            else if (args[0] == "restore") {
                RestoreSimulation restoreAction;
                restoreAction.act(*this);
            }
            else {
                std::cerr << "Error: unknown action '" <<args[0] << "'" <<std::endl;
            }
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

void Simulation::close() {
    isRunning = false;
}

void Simulation::open() {
    isRunning = true;
}

void Simulation::step() {
    for (auto &plan : plans) {
        plan.step(); //going one step in each plan
    }
}

bool Simulation::addSettlement(Settlement *settlement) {
    if (!settlement){
        std::cout << "Error: nullPtr" << std::endl;
        return false;
    }
    for (const auto &s : settlements) {
        if (s->getName() == settlement->getName()) {
            std::cout << "Error: Settlement already exists" << std::endl;
            return false; //duplicate
        }
    }
    settlements.push_back(settlement);
    return true; //added succesfuly
}

bool Simulation::addFacility(FacilityType facility) {
    for (const FacilityType &f : facilitiesOptions) {
        if (f.getName() == facility.getName()) {
            std::cout << "Facility already exists" << std::endl;
            return false; //duplicate
        }
    }
    facilitiesOptions.emplace_back(facility);
    return true; //added succesfuly
}

void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy){
    if (!selectionPolicy) {
        throw std::runtime_error("Error: selection policy is null");
    }

    Plan newPlan(planCounter++, settlement, selectionPolicy, facilitiesOptions);

    plans.push_back(newPlan);

    std::cout <<"Plan created for settlement: " << settlement.getName()
              <<" with policy: " << selectionPolicy->toString() << std::endl;
}

void Simulation::addAction(BaseAction *action) {
    if (!action) {
        throw std::runtime_error("Error: Invalid action");
    }
    actionsLog.push_back(action->clone());
}

SelectionPolicy *Simulation::createSelectionPolicy(const string &policyType){
    if (policyType == "nve") {
        return new NaiveSelection();
    }
    else if (policyType == "bal") {
        return new BalancedSelection(0,0,0);
    }
    else if (policyType == "eco") {
        return new EconomySelection();
    }
    else if (policyType == "env") {
        return new SustainabilitySelection();
    }
    else {
        std::cerr << "Error: Unknown selection policy type: " << policyType << std::endl;
        return nullptr;
    }
}

bool Simulation::isSettlementExists(const std::string &name) {
    for (const auto &settlemt : settlements) {
        if (settlemt->getName() == name) {
            return true;
        }
    }
    return false;
}

void Simulation::clearPlans() {
    plans.clear();
}

void Simulation::clearSettlements() {
    for (auto* settlement : settlements) {
        delete settlement;
    }
    settlements.clear();
}


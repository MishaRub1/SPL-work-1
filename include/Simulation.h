#pragma once
#include <string>
#include <vector>
#include "Facility.h"
#include "Plan.h"
#include "Settlement.h"
using std::string;
using std::vector;


class BaseAction;
class SelectionPolicy;

class Simulation {
    public:
        Simulation(const string &configFilePath);
        ~Simulation();                                     
        Simulation(const Simulation &other);              
        Simulation &operator=(const Simulation &other);   
        Simulation(Simulation &&other) noexcept;          
        Simulation &operator=(Simulation &&other) noexcept; 

        void start();
        void addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy);
        void addAction(BaseAction *action);
        bool addSettlement(Settlement *settlement);
        bool addFacility(FacilityType facility);
        bool isSettlementExists(const string &settlementName);
        Settlement &getSettlement(const string &settlementName);
        Plan &getPlan(const int planID);
        void step();
        void close();
        void open();
        SelectionPolicy* createSelectionPolicy(const string& policyType);
        const std::vector<Plan> &getPlans() const;
        const std::vector<FacilityType>& getFacilitiesOptions() const;
        const std::vector<BaseAction*>& getActionsLog() const;
        void clearPlans();
        void clearSettlements();
        

    private:
        bool isRunning;
        int planCounter; //For assigning unique plan IDs
        vector<BaseAction*> actionsLog;
        vector<Plan> plans;
        vector<Settlement*> settlements;
        vector<FacilityType> facilitiesOptions;
        
};
extern Simulation* backup;
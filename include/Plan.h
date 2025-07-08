#pragma once
#include <vector>
#include "Facility.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
using std::vector;

enum class PlanStatus {
    AVALIABLE,
    BUSY,
};

class Plan {
    public:
        Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions);
        ~Plan();                                     
        Plan(const Plan &other);                     
        Plan &operator=(const Plan &other) = delete;          
        Plan(Plan &&other) noexcept;                 
        Plan &operator=(Plan &&other) noexcept = delete;      

        int getlifeQualityScore() const;
        int getEconomyScore() const;
        int getEnvironmentScore() const;
        void setSelectionPolicy(SelectionPolicy *selectionPolicy);
        void step();
        void printStatus();
        const vector<Facility*> &getFacilities() const;
        void addFacility(Facility* facility);
        const string toString() const;
        int getConstructionLimit(const Settlement& settlement);
        bool isSamePolicy(const SelectionPolicy *policy) const;
        int getId() const;
        const string resultPrint() const;

    private:
        int plan_id;
        const Settlement &settlement;
        SelectionPolicy *selectionPolicy; //What happens if we change this to a reference?
        PlanStatus status;
        vector<Facility*> facilities;
        vector<Facility*> underConstruction;
        const vector<FacilityType> &facilityOptions;
        int life_quality_score, economy_score, environment_score;
};
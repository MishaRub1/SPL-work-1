#include "SelectionPolicy.h"
#include "Plan.h"
#include <iostream>
#include <stdexcept>
#include <climits>
#include <algorithm>

//helper function
void SelectionPolicy::markFacilityAsSelected(const FacilityType& facility) {
    selectedFacility.push_back(facility);
}

bool SelectionPolicy::isFacilitySelected(const FacilityType& facility) {

    for (const auto& selected : selectedFacility) {
        if (selected == facility) {
            return true;
        }
    }

    return false;
}



//Naive selection
NaiveSelection::NaiveSelection() : lastSelectedIndex(0) {}

const FacilityType &NaiveSelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {

    if (facilitiesOptions.empty()) {
        throw std::runtime_error("Error: No facilities available for selection");
    }

    const FacilityType &selected = facilitiesOptions[lastSelectedIndex];
    lastSelectedIndex = (lastSelectedIndex + 1) % facilitiesOptions.size(); //making sure we dont exceed list size
    return selected;
}

const string NaiveSelection::toString() const {
    return "NaiveSelection";
}

NaiveSelection *NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}

//Balanced selection
BalancedSelection::BalancedSelection(int lifeQualityScore, int economyScore, int environmentScore)
    : LifeQualityScore(lifeQualityScore), EconomyScore(economyScore), EnvironmentScore(environmentScore) {}

const FacilityType &BalancedSelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {

    if (facilitiesOptions.empty()) {
        throw std::runtime_error("Error: No facilities available for selection");
    }

    int bestIndex = -1;
    int smallestDistance = INT_MAX;

    for (size_t i=0; i < facilitiesOptions.size(); ++i) {

        if (facilitiesOptions.empty()) {
            throw std::runtime_error("Error: No facilities available for selection.");
        }

        const FacilityType &facility = facilitiesOptions[i];

        int newLifeQualityScore = LifeQualityScore + facility.getLifeQualityScore();
        int newEconomyScore = EconomyScore + facility.getEconomyScore();
        int newEnvironmentScore = EnvironmentScore + facility.getEnvironmentScore();

        std::vector<int> scores = {newLifeQualityScore, newEconomyScore, newEnvironmentScore};
        int maxScore = *std::max_element(scores.begin(), scores.end());
        int distance = 0;
        for (auto col : scores) {
            distance += maxScore - col;
        }

        if (distance < smallestDistance) {
            bestIndex = i;
            smallestDistance = distance;
        }

    }

    //checking we got a facility to give
    if (bestIndex == -1) {
        throw std::runtime_error("Error: Cannot select facility");
    }
    markFacilityAsSelected(facilitiesOptions[bestIndex]);
    return facilitiesOptions[bestIndex];
}

const string BalancedSelection::toString() const {
    return "BalancedSelection";
}

BalancedSelection *BalancedSelection::clone() const {
    return new BalancedSelection(LifeQualityScore, EconomyScore, EnvironmentScore);
}

void BalancedSelection::updateScore(const FacilityType& facility) {
    LifeQualityScore += facility.getLifeQualityScore();
    EconomyScore += facility.getEconomyScore();
    EnvironmentScore += facility.getEnvironmentScore();
}

//Economy selection
EconomySelection::EconomySelection() : lastSelectedIndex(0) {}

const FacilityType &EconomySelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {

    if (facilitiesOptions.empty()) {
        throw std::runtime_error("Error: no facilities available for selection.");
    }

    size_t numFacilities = facilitiesOptions.size();
    size_t startIndex = lastSelectedIndex;

    for (size_t i=0; i<facilitiesOptions.size(); ++i) {
        size_t currentIndex = (startIndex + i) % numFacilities;
        const FacilityType &facility = facilitiesOptions[currentIndex];

        if (facility.getCategory() == FacilityCategory::ECONOMY) {
            lastSelectedIndex = (currentIndex + 1) % numFacilities;
            return facility;
        }
    }

    throw std::runtime_error("Error: No facilities in the Economy category available.");
}

const string EconomySelection::toString() const {
    return "EconomySelection";
}

EconomySelection *EconomySelection::clone() const {
    return new EconomySelection();
}

//Sustainablity selection
SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(0) {}

const FacilityType &SustainabilitySelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {
    
    if (facilitiesOptions.empty()) {
        throw std::runtime_error("Error: no facilities available for selection.");
    }

    size_t numFacilities = facilitiesOptions.size();
    size_t startIndex = lastSelectedIndex;

    for (size_t i=0; i<facilitiesOptions.size(); ++i) {
        size_t currentIndex = (startIndex + i) % numFacilities;
        const FacilityType &facility = facilitiesOptions[currentIndex];

        if (facility.getCategory() == FacilityCategory::ENVIRONMENT) {
            lastSelectedIndex = (currentIndex + 1) % numFacilities;
            return facility;
        }
    }

    throw std::runtime_error("Error: No facilities in the Environment category available.");
}

const string SustainabilitySelection::toString() const {
    return "SustainabilitySelection";
}

SustainabilitySelection *SustainabilitySelection::clone() const {
    return new SustainabilitySelection();
}


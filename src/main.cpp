#include "MetroGraph.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    MetroGraph metro;
    bool loaded = metro.loadData("data/stations.csv");
    if (!loaded) {
        loaded = metro.loadData("../data/stations.csv");
    }
    if (!loaded) {
        std::cout << "Could not load station data. Put stations.csv in the data folder." << std::endl;
        return 1;
    }

    while (true) {
        cout << "\nKolkata Metro Route Planner" << endl;
        cout << "1. List stations" << endl;
        cout << "2. Find route by minimum stops" << endl;
        cout << "3. Find fastest route by travel time" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose an option: ";

        int choice;
        cin >> choice;
        cin.ignore(10000, '\n');

        if (choice == 4)
            break;

        if (choice == 1) {
            auto stations = metro.getStationNames();
            cout << "\nStations:\n";
            for (const auto& name : stations)
                cout << "- " << name << endl;
            continue;
        }

        std::string source, destination;
        cout << "Enter origin station: ";
        getline(cin, source);
        cout << "Enter destination station: ";
        getline(cin, destination);

        std::vector<std::string> route;
        if (choice == 2) {
            route = metro.findPathByStops(source, destination);
        } else if (choice == 3) {
            route = metro.findPathByTime(source, destination);
        } else {
            cout << "Invalid option." << endl;
            continue;
        }

        if (route.empty()){
            cout << "No route found between " << source << " and " << destination << "." << endl;
            continue;
        }

        cout << "\nRoute:" << endl;
        for (const auto& station : route)
            cout << station << " -> ";
        cout << "END\n";
        cout << "Stops: " << (route.size() - 1) << endl;
        
        if (choice == 3){
            vector<int> path;
            for(const auto& station : route)
                path.push_back(metro.getStationId(station));
            int totalTime = metro.computeTravelTime(path);
            cout << "Total travel time: " << totalTime << " minutes" << endl;
        }
    }

    return 0;
}

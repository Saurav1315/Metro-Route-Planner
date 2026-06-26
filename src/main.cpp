#include "MetroGraph.h"
#include <iostream>
#include <string>
#include <limits>

using namespace std;

int main() {
    MetroGraph metro;
    bool loaded = metro.loadData("data/stations.csv");
    if (!loaded) {
        loaded = metro.loadData("../data/stations.csv");
    }
    if (!loaded) {
        cout << "Could not load station data. Put stations.csv in the data folder." << endl;
        return 1;
    }

    while (true) {
        cout << "\n====================================" << endl;
        cout << "Kolkata Metro Route Planner" << endl;
        cout << "====================================" << endl;
        cout << "1. List all stations" << endl;
        cout << "2. Find route with minimum stops" << endl;
        cout << "3. Find fastest route by travel time" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose an option: ";

        int choice;
        // Prevents infinite loop if character input is given
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 4) {
            cout << "Goodbye!" << endl;
            break;
        }

        if (choice == 1) {
            auto stations = metro.getStationNames();
            cout << "\nStations:" << endl;
            for (const auto& name : stations)
                cout << "- " << name << endl;
            continue;
        }

        string source, destination;
        cout << "Enter origin station: ";
        getline(cin, source);
        cout << "Enter destination station: ";
        getline(cin, destination);

        if (source.empty() || destination.empty()) {
            cout << "Both station names are required." << endl;
            continue;
        }

        vector<string> route;
        if (choice == 2) {
            route = metro.findPathByStops(source, destination);
        } else if (choice == 3) {
            route = metro.findPathByTime(source, destination);
        } else {
            cout << "Invalid option. Please try again." << endl;
            continue;
        }

        if (route.empty()) {
            cout << "No route found between " << source << " and " << destination << "." << endl;
            continue;
        }

        cout << "\nRoute found:" << endl;
        for (size_t i = 0; i < route.size(); ++i) {
            cout << route[i];
            if (i + 1 < route.size()) {
                cout << " -> ";
            }
        }
        cout << endl;
        cout << "Stops: " << (route.size() - 1) << endl;

        if (choice == 3) {
            vector<int> path;
            for (const auto& station : route)
                path.push_back(metro.getStationIdCaseInsensitive(station));
            int totalTime = metro.computeTravelTime(path);
            cout << "Total travel time: " << totalTime << " minutes" << endl;
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }

    return 0;
}
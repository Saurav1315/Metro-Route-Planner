#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct Edge {
    int to;
    int time;
    string line; // Tracks which line this track segment belongs to (e.g. "Line 1")
};

struct Station {
    int id;
    string name;
    string line; // Primary line name
};

class MetroGraph {
public:
    MetroGraph();
    bool loadData(const string& path);
    int getStationId(const string& name) const;
    int getStationIdCaseInsensitive(const string& name) const;
    vector<string> getStationNames() const;
    vector<string> findPathByStops(const string& from, const string& to) const;
    vector<string> findPathByTime(const string& from, const string& to) const;
    int computeTravelTime(const vector<int>& path) const;

private:
    void addStation(const string& name, const string& line);
    void addEdge(int from, int to, int time, const string& lineName);
    vector<int> bfs(int start, int goal) const;
    vector<int> dijkstra(int start, int goal) const;

    string toLower(string str) const;

    vector<Station> stations;
    unordered_map<string, int> stationIndex;
    unordered_map<string, int> stationIndexLower; // For instant O(1) case-insensitive search
    vector<vector<Edge>> mapEdges;
};
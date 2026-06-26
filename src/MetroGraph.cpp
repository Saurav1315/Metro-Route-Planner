#include "MetroGraph.h"
#include <fstream>
#include <queue>
#include <sstream>
#include <limits>
#include <algorithm>
#include <iostream>

using namespace std;

MetroGraph::MetroGraph() {}

// Helper to lowercase a string and clean carriage returns (\r) from files
string MetroGraph::toLower(string str) const {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (!str.empty() && str.back() == '\r') {
        str.pop_back();
    }
    return str;
}

bool MetroGraph::loadData(const string& path) {
    ifstream file(path);
    if (!file.is_open())
        return false;

    stations.clear();
    mapEdges.clear();
    stationIndex.clear();
    stationIndexLower.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        stringstream ss(line);
        string first, second, timeText, lineName;

        if (!getline(ss, first, ',')) continue;
        if (!getline(ss, second, ',')) continue;
        if (!getline(ss, timeText, ',')) continue;
        if (!getline(ss, lineName, ',')) lineName = "Unknown";

        if (!lineName.empty() && lineName.back() == '\r') {
            lineName.pop_back();
        }

        int travelTime = stoi(timeText);
        
        if (stationIndex.find(first) == stationIndex.end())
            addStation(first, lineName);
        if (stationIndex.find(second) == stationIndex.end())
            addStation(second, lineName);

        int id1 = getStationId(first);
        int id2 = getStationId(second);
        
        addEdge(id1, id2, travelTime, lineName);
        addEdge(id2, id1, travelTime, lineName);
    }

    return true;
}

void MetroGraph::addStation(const string& name, const string& line) {
    if (stationIndex.find(name) != stationIndex.end())
        return;

    int id = static_cast<int>(stations.size());
    stationIndex[name] = id;
    stationIndexLower[toLower(name)] = id;
    stations.push_back({id, name, line});
    mapEdges.emplace_back();
}

void MetroGraph::addEdge(int from, int to, int time, const string& lineName) {
    if (from < 0 || to < 0 || from >= mapEdges.size() || to >= mapEdges.size())
        return;
    mapEdges[from].push_back({to, time, lineName});
}

vector<string> MetroGraph::getStationNames() const {
    vector<string> list;
    for (const auto& station : stations)
        list.push_back(station.name);
    return list;
}

int MetroGraph::getStationId(const string& name) const {
    auto it = stationIndex.find(name);
    return it == stationIndex.end() ? -1 : it->second;
}

int MetroGraph::getStationIdCaseInsensitive(const string& name) const {
    auto it = stationIndexLower.find(toLower(name));
    return it == stationIndexLower.end() ? -1 : it->second;
}

// BFS to find route with minimum stops
vector<int> MetroGraph::bfs(int start, int goal) const {
    vector<int> parent(stations.size(), -1);
    vector<bool> visited(stations.size(), false);
    queue<int> q;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (current == goal)
            break;

        for (const auto& edge : mapEdges[current]) {
            if (!visited[edge.to]) {
                visited[edge.to] = true;
                parent[edge.to] = current;
                q.push(edge.to);
            }
        }
    }

    if (!visited[goal])
        return {};

    vector<int> path;
    int node = goal;
    while (node != -1) {
        path.push_back(node);
        node = parent[node];
    }
    reverse(path.begin(), path.end());
    return path;
}

// O(E log V) Dijkstra using Min-Priority Queue & Line Transfer Penalties
vector<int> MetroGraph::dijkstra(int start, int goal) const {
    const int INF = numeric_limits<int>::max();
    const int TRANSFER_PENALTY = 5; // 5-minute transfer penalty
    
    vector<int> dist(stations.size(), INF);
    vector<int> parent(stations.size(), -1);
    vector<string> incomingLine(stations.size(), "");

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;
        if (u == goal) break;

        for (const auto& edge : mapEdges[u]) {
            int weight = edge.time;

            // Apply a 5-minute penalty if we switch lines at intermediate stations
            if (u != start && !incomingLine[u].empty() && incomingLine[u] != edge.line) {
                weight += TRANSFER_PENALTY;
            }

            if (dist[u] + weight < dist[edge.to]) {
                dist[edge.to] = dist[u] + weight;
                parent[edge.to] = u;
                incomingLine[edge.to] = edge.line;
                pq.push({dist[edge.to], edge.to});
            }
        }
    }

    if (dist[goal] == INF)
        return {};

    vector<int> path;
    int node = goal;
    while (node != -1) {
        path.push_back(node);
        node = parent[node];
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<string> MetroGraph::findPathByStops(const string& from, const string& to) const {
    int start = getStationIdCaseInsensitive(from);
    int goal = getStationIdCaseInsensitive(to);
    if (start == -1 || goal == -1)
        return {};

    vector<int> path = bfs(start, goal);
    vector<string> result;
    for (int id : path)
        result.push_back(stations[id].name);
    return result;
}

vector<string> MetroGraph::findPathByTime(const string& from, const string& to) const {
    int start = getStationIdCaseInsensitive(from);
    int goal = getStationIdCaseInsensitive(to);
    if (start == -1 || goal == -1)
        return {};

    vector<int> path = dijkstra(start, goal);
    vector<string> result;
    for (int id : path)
        result.push_back(stations[id].name);
    return result;
}

int MetroGraph::computeTravelTime(const vector<int>& path) const {
    int total = 0;
    string currentLine = "";

    for (size_t i = 1; i < path.size(); ++i) {
        int from = path[i - 1];
        int to = path[i];
        
        for (const auto& edge : mapEdges[from]) {
            if (edge.to == to) {
                total += edge.time;
                
                // Add 5-minute penalty if switching lines
                if (i > 1 && !currentLine.empty() && currentLine != edge.line) {
                    total += 5;
                }
                currentLine = edge.line;
                break;
            }
        }
    }
    return total;
}
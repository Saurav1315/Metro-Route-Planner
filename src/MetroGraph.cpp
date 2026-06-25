#include "MetroGraph.h"
#include <fstream>
#include <queue>
#include <sstream>
#include <climits>
#include <limits>
#include <algorithm>

MetroGraph::MetroGraph() {}

bool MetroGraph::loadData(const string& path) {
    ifstream file(path);
    if (!file.is_open())
        return false;

    stations.clear();
    mapEdges.clear();
    stationIndex.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty())
            continue;

        stringstream ss(line);
        string first;
        string second;
        string timeText;
        string lineName;

        if (!std::getline(ss, first, ','))
            continue;
        if (!std::getline(ss, second, ','))
            continue;
        if (!std::getline(ss, timeText, ','))
            continue;
        if (!std::getline(ss, lineName, ','))
            lineName = "Unknown";

        int travelTime = stoi(timeText);
        if (stationIndex.find(first) == stationIndex.end())
            addStation(first, lineName);
        if (stationIndex.find(second) == stationIndex.end())
            addStation(second, lineName);

        int id1 = getStationId(first);
        int id2 = getStationId(second);
        addEdge(id1, id2, travelTime);
        addEdge(id2, id1, travelTime);
    }

    return true;
}

void MetroGraph::addStation(const std::string& name, const std::string& line) {
    if (stationIndex.find(name) != stationIndex.end())
        return;

    int id = static_cast<int>(stations.size());
    stationIndex[name] = id;
    stations.push_back({id, name, line});
    mapEdges.emplace_back();
}

void MetroGraph::addEdge(int from, int to, int time) {
    if (from < 0 || to < 0)
        return;
    mapEdges[from].push_back({to, time});
}

vector<string> MetroGraph::getStationNames() const {
    vector<string> list;
    for (const auto& station : stations)
        list.push_back(station.name);
    return list;
}

int MetroGraph::getStationId(const std::string& name) const {
    auto it = stationIndex.find(name);
    return it == stationIndex.end() ? -1 : it->second;
}

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

    std::vector<int> path;
    int node = goal;
    while (node != -1) {
        path.push_back(node);
        node = parent[node];
    }

    std::reverse(path.begin(), path.end());
    return path;
}

vector<int> MetroGraph::dijkstra(int start, int goal) const {
    const int INF = numeric_limits<int>::max();
    vector<int> dist(stations.size(), INF);
    vector<int> parent(stations.size(), -1);
    vector<bool> visited(stations.size(), false);

    dist[start] = 0;

    for (size_t i = 0; i < stations.size(); ++i) {
        int u = -1;
        int bestDist = INF;
        for (size_t j = 0; j < stations.size(); ++j) {
            if (!visited[j] && dist[j] < bestDist) {
                u = static_cast<int>(j);
                bestDist = dist[j];
            }
        }
        if (u == -1)
            break;
        if (u == goal)
            break;

        visited[u] = true;
        for (const auto& edge : mapEdges[u]) {
            int alt = dist[u] + edge.time;
            if (alt < dist[edge.to]) {
                dist[edge.to] = alt;
                parent[edge.to] = u;
            }
        }
    }

    if (dist[goal] == INF)
        return {};

    std::vector<int> path;
    int node = goal;
    while (node != -1) {
        path.push_back(node);
        node = parent[node];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

vector<string> MetroGraph::findPathByStops(const string& source, const string& destination) const {
    int start = getStationId(source);
    int goal = getStationId(destination);
    if (start == -1 || goal == -1)
        return {};

    std::vector<int> path = bfs(start, goal);
    std::vector<std::string> result;
    for (int id : path)
        result.push_back(stations[id].name);
    return result;
}

vector<string> MetroGraph::findPathByTime(const string& source, const string& destination) const {
    int start = getStationId(source);
    int goal = getStationId(destination);
    if (start == -1 || goal == -1)
        return {};

    std::vector<int> path = dijkstra(start, goal);
    std::vector<std::string> result;
    for (int id : path)
        result.push_back(stations[id].name);
    return result;
}

int MetroGraph::computeTravelTime(const vector<int>& path) const {
    int total = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        int from = path[i - 1];
        int to = path[i];
        for (const auto& edge : mapEdges[from]) {
            if (edge.to == to) {
                total += edge.time;
                break;
            }
        }
    }
    return total;
}

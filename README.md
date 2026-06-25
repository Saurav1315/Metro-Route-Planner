# Kolkata Metro Route Planner

A simple C++ CLI application to plan routes on a metro network using BFS and Dijkstra's algorithm.

## Project structure

- `src/`
  - `main.cpp` — CLI menu and user interaction
  - `MetroGraph.h` — graph data structures and route APIs
  - `MetroGraph.cpp` — CSV loading and algorithm implementations
- `data/stations.csv` — sample Kolkata Metro station network
- `.gitignore`

## Build and run

1. Open a terminal in `metro-route-planner`.
2. Build using a C++ compiler, for example:
   - `g++ src/*.cpp -o metro-planner`
3. Run:
   - `./metro-planner`

## Usage

- Choose `1` to list stations.
- Choose `2` to find the route with minimum stops.
- Choose `3` to find the fastest route by travel time.
- Enter origin and destination station names exactly as shown.

## Notes

- The sample network models Kolkata Metro Line 1 and Line 2.
- You can extend `data/stations.csv` with more stations and travel times.

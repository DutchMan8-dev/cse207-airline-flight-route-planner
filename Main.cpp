#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <limits>
#include <cctype>

using namespace std;

const int MAX_AIRPORTS = 20;
const double INF = 1e18;

// =======================
// Convert airport code to uppercase
// =======================
string normalizeCode(string code) {
    for (int i = 0; i < (int)code.size(); i++) {
        code[i] = toupper((unsigned char)code[i]);
    }
    return code;
}

// =======================
// Airport information
// =======================
struct Airport {
    string code;
    string name;
    string city;
    string region;
    string category;
    string terminalInfo;
};

// =======================
// Route edge for graph
// =======================
struct Route {
    int to;
    double distanceKm;
    double durationMin;
    double costEUR;
    string label;
};

// =======================
// Priority Queue Node
// =======================
struct PQNode {
    double dist;
    int vertex;

    bool operator<(const PQNode& other) const {
        return dist > other.dist;   // makes priority_queue work like min-heap
    }
};

// =======================
// Airline Route Planner
// =======================
class AirlineRoutePlanner {
private:
    Airport airports[MAX_AIRPORTS];
    vector<Route> adj[MAX_AIRPORTS];
    int airportCount;

public:
    AirlineRoutePlanner() {
        airportCount = 0;
    }

    // Find airport index by code
    int findAirportIndex(string code) {
        code = normalizeCode(code);

        for (int i = 0; i < airportCount; i++) {
            if (airports[i].code == code) {
                return i;
            }
        }
        return -1;
    }

    // Get current airport count
    int getAirportCount() {
        return airportCount;
    }

    // Count total unique routes
    int getTotalRoutes() {
        int total = 0;
        for (int i = 0; i < airportCount; i++) {
            total += adj[i].size();
        }
        return total / 2;
    }

    // Check if route already exists
    bool routeExists(int from, int to) {
        for (int i = 0; i < (int)adj[from].size(); i++) {
            if (adj[from][i].to == to) {
                return true;
            }
        }
        return false;
    }

    // Add airport
    bool addAirport(string code, string name, string city, string region,
                    string category, string terminalInfo) {
        code = normalizeCode(code);

        if (airportCount >= MAX_AIRPORTS) {
            cout << "Maximum airport limit reached.\n";
            return false;
        }

        if (findAirportIndex(code) != -1) {
            cout << "Airport code already exists.\n";
            return false;
        }

        airports[airportCount].code = code;
        airports[airportCount].name = name;
        airports[airportCount].city = city;
        airports[airportCount].region = region;
        airports[airportCount].category = category;
        airports[airportCount].terminalInfo = terminalInfo;

        airportCount++;
        return true;
    }

    // Add bidirectional route
    bool addRoute(string fromCode, string toCode,
                  double distanceKm, double durationMin,
                  double costEUR, string label) {
        fromCode = normalizeCode(fromCode);
        toCode = normalizeCode(toCode);

        int from = findAirportIndex(fromCode);
        int to = findAirportIndex(toCode);

        if (from == -1 || to == -1) {
            cout << "Invalid airport code.\n";
            return false;
        }

        if (from == to) {
            cout << "Source and destination cannot be the same.\n";
            return false;
        }

        if (routeExists(from, to)) {
            cout << "Route already exists.\n";
            return false;
        }

        Route r1;
        r1.to = to;
        r1.distanceKm = distanceKm;
        r1.durationMin = durationMin;
        r1.costEUR = costEUR;
        r1.label = label;

        Route r2;
        r2.to = from;
        r2.distanceKm = distanceKm;
        r2.durationMin = durationMin;
        r2.costEUR = costEUR;
        r2.label = label;

        adj[from].push_back(r1);
        adj[to].push_back(r2);

        return true;
    }

    // Show airport list
    void showAirportCatalog() {
        cout << "\n===== Airport Catalog =====\n";
        for (int i = 0; i < airportCount; i++) {
            cout << airports[i].code << " - " << airports[i].name << endl;
            cout << "  City: " << airports[i].city << ", " << airports[i].region << endl;
            cout << "  Category: " << airports[i].category << endl;
            cout << "  Terminals: " << airports[i].terminalInfo << endl;
            cout << endl;
        }
    }

    // Show available airport codes
    void showAirportCodes() {
        cout << "Available airport codes: ";
        for (int i = 0; i < airportCount; i++) {
            cout << airports[i].code;
            if (i != airportCount - 1) cout << ", ";
        }
        cout << endl;
    }

    // Display adjacency list
    void displayRouteMap() {
        cout << "\n===== Route Map (Adjacency List) =====\n";
        for (int i = 0; i < airportCount; i++) {
            cout << airports[i].code << " -> ";
            for (int j = 0; j < (int)adj[i].size(); j++) {
                int v = adj[i][j].to;
                cout << airports[v].code
                     << " [" << adj[i][j].distanceKm << " km, "
                     << adj[i][j].durationMin << " min, €"
                     << adj[i][j].costEUR << "]";
                if (j != (int)adj[i].size() - 1) cout << " | ";
            }
            cout << endl;
        }
        cout << endl;
    }

    // Get chosen weight
    double getWeight(Route edge, int metricChoice) {
        if (metricChoice == 1) return edge.distanceKm;
        if (metricChoice == 2) return edge.durationMin;
        return edge.costEUR;
    }

    // Print path
    void printPath(vector<int>& parent, int destination) {
        vector<int> path;
        int cur = destination;

        while (cur != -1) {
            path.push_back(cur);
            cur = parent[cur];
        }

        reverse(path.begin(), path.end());

        cout << "Path: ";
        for (int i = 0; i < (int)path.size(); i++) {
            cout << airports[path[i]].code;
            if (i != (int)path.size() - 1) cout << " -> ";
        }
        cout << endl;

        cout << "Intermediate stops: " << max(0, (int)path.size() - 2) << endl;
    }

    // Dijkstra shortest path
    void shortestRoute(string sourceCode, string destinationCode, int metricChoice) {
        sourceCode = normalizeCode(sourceCode);
        destinationCode = normalizeCode(destinationCode);

        int source = findAirportIndex(sourceCode);
        int destination = findAirportIndex(destinationCode);

        if (source == -1 || destination == -1) {
            cout << "Invalid source or destination airport code.\n";
            return;
        }

        if (metricChoice < 1 || metricChoice > 3) {
            cout << "Invalid optimization choice.\n";
            return;
        }

        vector<double> dist(airportCount, INF);
        vector<int> parent(airportCount, -1);

        priority_queue<PQNode> pq;

        dist[source] = 0;
        pq.push({0, source});

        while (!pq.empty()) {
            PQNode topNode = pq.top();
            pq.pop();

            double currentDist = topNode.dist;
            int u = topNode.vertex;

            if (currentDist > dist[u]) continue;

            for (int i = 0; i < (int)adj[u].size(); i++) {
                Route edge = adj[u][i];
                int v = edge.to;
                double w = getWeight(edge, metricChoice);

                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        if (dist[destination] == INF) {
            cout << "No valid route found.\n";
            return;
        }

        vector<int> path;
        int cur = destination;
        while (cur != -1) {
            path.push_back(cur);
            cur = parent[cur];
        }
        reverse(path.begin(), path.end());

        double totalDistance = 0;
        double totalDuration = 0;
        double totalCost = 0;

        for (int i = 0; i + 1 < (int)path.size(); i++) {
            int u = path[i];
            int v = path[i + 1];

            for (int j = 0; j < (int)adj[u].size(); j++) {
                if (adj[u][j].to == v) {
                    totalDistance += adj[u][j].distanceKm;
                    totalDuration += adj[u][j].durationMin;
                    totalCost += adj[u][j].costEUR;
                    break;
                }
            }
        }

        cout << "\n===== Route Summary =====\n";
        printPath(parent, destination);

        if (metricChoice == 1) {
            cout << "Optimized by: Distance\n";
            cout << "Best distance: " << dist[destination] << " km\n";
        } else if (metricChoice == 2) {
            cout << "Optimized by: Duration\n";
            cout << "Best duration: " << dist[destination] << " min\n";
        } else {
            cout << "Optimized by: Cost\n";
            cout << "Best fare: €" << dist[destination] << endl;
        }

        cout << "Total distance: " << totalDistance << " km\n";
        cout << "Total duration: " << totalDuration << " min\n";
        cout << "Total cost: €" << totalCost << endl;
        cout << endl;
    }

    // Load built-in Spain data
    void loadSpainData() {
        addAirport("MAD", "Adolfo Suarez Madrid-Barajas Airport", "Madrid",
                   "Community of Madrid", "Major international hub",
                   "T1, T2, T3, T4, T4S");

        addAirport("BCN", "Josep Tarradellas Barcelona-El Prat Airport", "Barcelona",
                   "Catalonia", "Major international hub",
                   "T1, T2");

        addAirport("PMI", "Palma de Mallorca Airport", "Palma",
                   "Balearic Islands", "Large island leisure hub",
                   "Main Terminal, Module A, B, C, D");

        addAirport("AGP", "Malaga-Costa del Sol Airport", "Malaga",
                   "Andalusia", "Large tourism-focused airport",
                   "T2, T3");

        addAirport("LPA", "Gran Canaria Airport", "Las Palmas / Telde",
                   "Canary Islands", "Large island gateway",
                   "Main Terminal");

        addAirport("TFS", "Tenerife South Airport", "Granadilla de Abona",
                   "Canary Islands", "Large island leisure airport",
                   "Main Terminal");

        addAirport("BIO", "Bilbao Airport", "Bilbao",
                   "Basque Country", "Regional/international business gateway",
                   "Main Terminal");

        addAirport("IBZ", "Ibiza Airport", "Ibiza",
                   "Balearic Islands", "Seasonal island leisure airport",
                   "Main Terminal");

        addRoute("MAD", "BCN", 505, 75, 89, "Iberian trunk route");
        addRoute("MAD", "AGP", 431, 70, 74, "Capital to Costa del Sol");
        addRoute("MAD", "PMI", 549, 80, 82, "Capital to Mallorca");
        addRoute("MAD", "BIO", 323, 60, 69, "Capital to Basque Country");
        addRoute("MAD", "LPA", 1760, 180, 145, "Capital to Gran Canaria");
        addRoute("MAD", "TFS", 1792, 185, 149, "Capital to Tenerife South");
        addRoute("MAD", "IBZ", 470, 75, 78, "Capital to Ibiza");

        addRoute("BCN", "PMI", 203, 50, 49, "Mediterranean island hop");
        addRoute("BCN", "BIO", 467, 70, 72, "Barcelona to Bilbao");
        addRoute("BCN", "IBZ", 283, 55, 51, "Barcelona to Ibiza");

        addRoute("AGP", "PMI", 694, 95, 83, "Sun-and-sea connector");
        addRoute("AGP", "TFS", 1452, 150, 129, "Andalusia to Tenerife South");

        addRoute("PMI", "IBZ", 138, 40, 36, "Balearic inter-island route");
        addRoute("LPA", "TFS", 117, 35, 28, "Canary inter-island route");
    }
};

// =======================
// Main Function
// =======================
int main() {
    AirlineRoutePlanner planner;
    planner.loadSpainData();

    int choice;

    do {
        cout << "==============================\n";
        cout << " Airline Flight Route Planner \n";
        cout << "==============================\n";
        cout << "Total Airports: " << planner.getAirportCount()
             << " | Total Routes: " << planner.getTotalRoutes() << "\n";
        cout << "1. Show airport catalog\n";
        cout << "2. Show route map\n";
        cout << "3. Find shortest route\n";
        cout << "4. Add custom airport\n";
        cout << "5. Add custom route\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (!cin) {
            cout << "Invalid input or no more input. Exiting program.\n";
            break;
        }

        if (choice == 1) {
            planner.showAirportCatalog();
        }
        else if (choice == 2) {
            planner.displayRouteMap();
        }
        else if (choice == 3) {
            string source, destination;
            int metricChoice;

            planner.showAirportCodes();

            cout << "Enter source airport code: ";
            cin >> source;
            cout << "Enter destination airport code: ";
            cin >> destination;

            cout << "Optimize by:\n";
            cout << "1. Distance\n";
            cout << "2. Duration\n";
            cout << "3. Cost\n";
            cout << "Enter choice: ";
            cin >> metricChoice;

            if (!cin) {
                cout << "Invalid input or no more input. Exiting program.\n";
                break;
            }

            planner.shortestRoute(source, destination, metricChoice);
        }
        else if (choice == 4) {
            string code, name, city, region, category, terminalInfo;

            cout << "Enter airport code: ";
            cin >> code;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter airport name: ";
            getline(cin, name);

            cout << "Enter city: ";
            getline(cin, city);

            cout << "Enter region: ";
            getline(cin, region);

            cout << "Enter category: ";
            getline(cin, category);

            cout << "Enter terminal info: ";
            getline(cin, terminalInfo);

            if (planner.addAirport(code, name, city, region, category, terminalInfo)) {
                cout << "Airport added successfully.\n";
            }
        }
        else if (choice == 5) {
            string fromCode, toCode, label;
            double distanceKm, durationMin, costEUR;

            planner.showAirportCodes();

            cout << "Enter FROM airport code: ";
            cin >> fromCode;
            cout << "Enter TO airport code: ";
            cin >> toCode;
            cout << "Enter distance (km): ";
            cin >> distanceKm;
            cout << "Enter duration (minutes): ";
            cin >> durationMin;
            cout << "Enter cost (EUR): ";
            cin >> costEUR;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter route label: ";
            getline(cin, label);

            if (planner.addRoute(fromCode, toCode, distanceKm, durationMin, costEUR, label)) {
                cout << "Route added successfully.\n";
            }
        }
        else if (choice == 0) {
            cout << "Exiting program.\n";
        }
        else {
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}

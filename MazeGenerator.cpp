#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <queue>
#include <unordered_map>
#include <string>

class DisjointSet {
private:
    std::vector<int> parent;
    std::vector<int> rank;

public:
    DisjointSet(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        
        // Initialize each element as its own parent
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }
    
    // Find the parent of a set with path compression
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    
    // Union two sets by rank
    void unionSets(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        
        if (rootX == rootY) return;
        
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
    
    // Check if two elements are in the same set
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

class Maze {
private:
    int width;
    int height;
    std::vector<std::vector<char>> grid;
    std::mt19937 rng;

    // Check if a cell is isolated (no adjacent path cells)
    bool isIsolated(int y, int x) {
        // Count adjacent path cells
        int adjacentPaths = 0;
        
        // Check top
        if (y > 0 && (grid[y-1][x] == '.' || grid[y-1][x] == 'H' || grid[y-1][x] == 'B' || 
                      grid[y-1][x] == 'T' || grid[y-1][x] == 'M')) adjacentPaths++;
        // Check bottom
        if (y < height-1 && (grid[y+1][x] == '.' || grid[y+1][x] == 'H' || grid[y+1][x] == 'B' || 
                            grid[y+1][x] == 'T' || grid[y+1][x] == 'M')) adjacentPaths++;
        // Check left
        if (x > 0 && (grid[y][x-1] == '.' || grid[y][x-1] == 'H' || grid[y][x-1] == 'B' || 
                      grid[y][x-1] == 'T' || grid[y][x-1] == 'M')) adjacentPaths++;
        // Check right
        if (x < width-1 && (grid[y][x+1] == '.' || grid[y][x+1] == 'H' || grid[y][x+1] == 'B' || 
                           grid[y][x+1] == 'T' || grid[y][x+1] == 'M')) adjacentPaths++;
        
        return adjacentPaths == 0;
    }

    // Check if the cell is a walkable path (not a wall)
    bool isWalkable(char cell) {
        return cell == '.' || cell == 'H' || cell == 'B' || cell == 'T' || cell == 'M';
    }

    // Check if the maze is fully connected using BFS
    bool isFullyConnected() {
        if (width == 0 || height == 0) return true;
        
        // Find the first path cell
        int startY = -1, startX = -1;
        for (int y = 0; y < height && startY == -1; y++) {
            for (int x = 0; x < width && startX == -1; x++) {
                if (isWalkable(grid[y][x])) {
                    startY = y;
                    startX = x;
                    break;
                }
            }
        }
        
        // If no path cells exist
        if (startY == -1) return true;
        
        // BFS to count reachable path cells
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::queue<std::pair<int, int>> q;
        q.push({startY, startX});
        visited[startY][startX] = true;
        int reachableCount = 1;
        
        int dy[] = {-1, 1, 0, 0}; // up, down, left, right
        int dx[] = {0, 0, -1, 1};
        
        while (!q.empty()) {
            auto [cy, cx] = q.front();
            q.pop();
            
            for (int i = 0; i < 4; i++) {
                int ny = cy + dy[i];
                int nx = cx + dx[i];
                
                if (ny >= 0 && ny < height && nx >= 0 && nx < width && 
                    isWalkable(grid[ny][nx]) && !visited[ny][nx]) {
                    visited[ny][nx] = true;
                    q.push({ny, nx});
                    reachableCount++;
                }
            }
        }
        
        // Count total path cells
        int totalPathCount = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (isWalkable(grid[y][x])) totalPathCount++;
            }
        }
        
        return reachableCount == totalPathCount;
    }

    // Fix connectivity by connecting isolated components
    void fixConnectivity() {
        // If the maze is empty or a single cell, it's trivially connected
        if (width <= 1 && height <= 1) return;

        // Find all path cells
        std::vector<std::pair<int, int>> pathCells;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (isWalkable(grid[y][x])) {
                    pathCells.push_back({y, x});
                }
            }
        }
        
        // If no path cells, nothing to do
        if (pathCells.empty()) return;
        
        // Create a disjoint set for all path cells
        std::vector<std::vector<int>> cellIndex(height, std::vector<int>(width, -1));
        int idx = 0;
        for (const auto& [y, x] : pathCells) {
            cellIndex[y][x] = idx++;
        }
        
        DisjointSet ds(pathCells.size());
        
        // Union adjacent path cells
        int dy[] = {-1, 1, 0, 0}; // up, down, left, right
        int dx[] = {0, 0, -1, 1};
        
        for (int i = 0; i < pathCells.size(); i++) {
            auto [y, x] = pathCells[i];
            
            for (int d = 0; d < 4; d++) {
                int ny = y + dy[d];
                int nx = x + dx[d];
                
                if (ny >= 0 && ny < height && nx >= 0 && nx < width && 
                    isWalkable(grid[ny][nx])) {
                    ds.unionSets(i, cellIndex[ny][nx]);
                }
            }
        }
        
        // Find all unique components (root sets)
        std::unordered_map<int, std::vector<int>> components;
        for (int i = 0; i < pathCells.size(); i++) {
            int root = ds.find(i);
            components[root].push_back(i);
        }
        
        // If only one component, the maze is already connected
        if (components.size() <= 1) return;
        
        // Connect all components to the first one
        auto it = components.begin();
        std::vector<int>& firstComponent = it->second;
        it++;
        
        for (; it != components.end(); it++) {
            std::vector<int>& nextComponent = it->second;
            
            // Find the closest pair of cells between the two components
            int minDistance = width + height; // A distance greater than any possible distance
            std::pair<int, int> bestCell1, bestCell2;
            
            for (int idx1 : firstComponent) {
                auto [y1, x1] = pathCells[idx1];
                
                for (int idx2 : nextComponent) {
                    auto [y2, x2] = pathCells[idx2];
                    
                    // Manhattan distance
                    int distance = std::abs(y2 - y1) + std::abs(x2 - x1);
                    
                    if (distance < minDistance) {
                        minDistance = distance;
                        bestCell1 = {y1, x1};
                        bestCell2 = {y2, x2};
                    }
                }
            }
            
            // Create a path between the two closest cells
            int y1 = bestCell1.first, x1 = bestCell1.second;
            int y2 = bestCell2.first, x2 = bestCell2.second;
            
            // Carve a straight path
            while (y1 != y2 || x1 != x2) {
                if (y1 < y2) {
                    y1++;
                    grid[y1][x1] = '.';
                } else if (y1 > y2) {
                    y1--;
                    grid[y1][x1] = '.';
                } else if (x1 < x2) {
                    x1++;
                    grid[y1][x1] = '.';
                } else if (x1 > x2) {
                    x1--;
                    grid[y1][x1] = '.';
                }
            }
            
            // Merge the connected component into the first one
            firstComponent.insert(firstComponent.end(), nextComponent.begin(), nextComponent.end());
        }
    }

public:
    Maze(int w, int h) : width(w), height(h) {
        // Initialize random number generator
        rng.seed(std::time(0));
        
        // Create the grid with exactly the specified dimensions
        grid.resize(height, std::vector<char>(width, '#'));
    }
    
    void generateMaze() {
        // Initialize all cells as walls
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                grid[y][x] = '#';
            }
        }
        
        // Create a list of all possible edges between adjacent cells
        // excluding the outer edges of the maze
        std::vector<std::pair<int, std::pair<int, int>>> edges;
        
        for (int y = 1; y < height-1; y++) {
            for (int x = 1; x < width-1; x++) {
                int cellIdx = y * width + x;
                
                // Right neighbor (if not on right edge)
                if (x + 1 < width-1) {
                    int weight = rng() % 10000;
                    edges.push_back({weight, {cellIdx, cellIdx + 1}});
                }
                
                // Bottom neighbor (if not on bottom edge)
                if (y + 1 < height-1) {
                    int weight = rng() % 10000;
                    edges.push_back({weight, {cellIdx, cellIdx + width}});
                }
            }
        }
        
        // Sort edges by random weight
        std::sort(edges.begin(), edges.end());
        
        // Apply Kruskal's algorithm to create a spanning tree (the maze)
        DisjointSet ds(width * height);
        
        // Process edges
        for (const auto& edge : edges) {
            int cell1 = edge.second.first;
            int cell2 = edge.second.second;
            
            int y1 = cell1 / width;
            int x1 = cell1 % width;
            int y2 = cell2 / width;
            int x2 = cell2 % width;
            
            // If cells are not connected, make an opening
            if (!ds.connected(cell1, cell2)) {
                ds.unionSets(cell1, cell2);
                
                // Mark both cells as path
                grid[y1][x1] = '.';
                grid[y2][x2] = '.';
            }
        }
        
        // Make sure we have at least some paths in the maze
        // If the maze is too small or has no internal paths, add some
        bool hasInternalPaths = false;
        for (int y = 1; y < height-1; y++) {
            for (int x = 1; x < width-1; x++) {
                if (grid[y][x] == '.') {
                    hasInternalPaths = true;
                    break;
                }
            }
            if (hasInternalPaths) break;
        }
        
        // If no internal paths, create some
        if (!hasInternalPaths && height > 2 && width > 2) {
            // Create a simple path from near the entrance to near the exit
            int midY = height / 2;
            for (int x = 1; x < width-1; x++) {
                grid[midY][x] = '.';
            }
            
            // Add some random internal paths
            int numRandomPaths = (width + height) / 4;
            for (int i = 0; i < numRandomPaths; i++) {
                int y = 1 + rng() % (height - 2);
                int x = 1 + rng() % (width - 2);
                grid[y][x] = '.';
            }
        }
        
        // Calculate path ratio (excluding the outer edges)
        int totalInternalCells = (width-2) * (height-2);
        int pathCount = 0;
        for (int y = 1; y < height-1; y++) {
            for (int x = 1; x < width-1; x++) {
                if (grid[y][x] == '.') {
                    pathCount++;
                }
            }
        }
        
        // Target path ratio (40-60%)
        double pathRatio = static_cast<double>(pathCount) / totalInternalCells;
        double targetRatio = 0.5; // 50% paths
        
        // If we have too many paths, convert some back to walls
        if (pathRatio > 0.7 && totalInternalCells > 9) {
            int wallsToAdd = pathCount - static_cast<int>(targetRatio * totalInternalCells);
            
            for (int i = 0; i < wallsToAdd && pathCount > totalInternalCells/4; i++) {
                int attempts = 0;
                int y, x;
                bool validCell = false;
                
                // Try to find a path cell that can be converted to a wall
                while (!validCell && attempts < 100) {
                    y = 1 + rng() % (height - 2);
                    x = 1 + rng() % (width - 2);
                    
                    if (grid[y][x] != '.' || (y == 1 && x == 1) || (y == height-2 && x == width-2)) {
                        attempts++;
                        continue;
                    }
                    
                    // Temporarily mark as wall
                    char original = grid[y][x];
                    grid[y][x] = '#';
                    
                    // Verify this doesn't break connectivity
                    bool breaksConnectivity = !isFullyConnected();
                    
                    // Restore original state if it breaks connectivity
                    if (breaksConnectivity) {
                        grid[y][x] = original;
                        attempts++;
                    } else {
                        validCell = true;
                        pathCount--;
                    }
                }
                
                // If we couldn't find a valid cell after many attempts, stop adding walls
                if (!validCell) break;
            }
        }
        
        // If we have too few paths, convert some walls to paths
        if (pathRatio < 0.3 && totalInternalCells > 9) {
            int pathsToAdd = static_cast<int>(targetRatio * totalInternalCells) - pathCount;
            
            for (int i = 0; i < pathsToAdd; i++) {
                int attempts = 0;
                int y, x;
                bool validCell = false;
                
                // Find a wall that is adjacent to at least one path
                while (!validCell && attempts < 100) {
                    y = 1 + rng() % (height - 2);
                    x = 1 + rng() % (width - 2);
                    
                    if (grid[y][x] != '#') {
                        attempts++;
                        continue;
                    }
                    
                    // Check if this wall is adjacent to at least one path
                    if ((y > 1 && grid[y-1][x] == '.') ||
                        (y < height-2 && grid[y+1][x] == '.') ||
                        (x > 1 && grid[y][x-1] == '.') ||
                        (x < width-2 && grid[y][x+1] == '.')) {
                        validCell = true;
                    } else {
                        attempts++;
                    }
                }
                
                if (validCell) {
                    grid[y][x] = '.';
                    pathCount++;
                } else {
                    // If we couldn't find a valid cell, stop adding paths
                    break;
                }
            }
        }
    }
    
    void printMaze() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::cout << grid[y][x] << " ";
            }
            std::cout << std::endl;
        }
    }
    
    void addEntranceAndExit() {
        // First ensure all edges are walls
        for (int y = 0; y < height; y++) {
            grid[y][0] = '#';              // Left edge
            grid[y][width-1] = '#';        // Right edge
        }
        for (int x = 0; x < width; x++) {
            grid[0][x] = '#';              // Top edge
            grid[height-1][x] = '#';       // Bottom edge
        }
        
        // Connect interior cells if they aren't already connected
        fixConnectivity();
        
        // Add entrance at top left
        grid[0][0] = '.';
        
        // Add exit at bottom right
        grid[height - 1][width - 1] = '.';
        
        // Ensure entrance and exit are connected to the maze
        // Connect entrance if needed
        if (isIsolated(0, 0)) {
            if (width > 1) grid[0][1] = '.';
            else if (height > 1) grid[1][0] = '.';
        }
        
        // Connect exit if needed
        if (isIsolated(height-1, width-1)) {
            if (width > 1) grid[height-1][width-2] = '.';
            else if (height > 1) grid[height-2][width-1] = '.';
        }
        
        // Make sure everything is connected after adding entrance/exit
        fixConnectivity();
        
        // Final check for direct external path from entrance to exit
        // If the maze is too small, create a minimal path
        if (width <= 3 || height <= 3) {
            if (width > 1 && height > 1) {
                // Create a path from entrance to exit
                grid[1][1] = '.';  // First internal cell
                
                // Connect to exit somehow
                if (width > 2 && height > 2) {
                    grid[height-2][width-2] = '.';  // Last internal cell
                    // Ensure there's a path between first and last internal cells
                    grid[1][width-2] = '.';
                    grid[height-2][1] = '.';
                }
            }
        }
    }
    
    // Get all accessible path cells for placing items
    std::vector<std::pair<int, int>> getAccessiblePathCells() {
        std::vector<std::pair<int, int>> accessibleCells;
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Skip entrance and exit positions
                if ((y == 0 && x == 0) || (y == height-1 && x == width-1)) {
                    continue;
                }
                
                // Add to list if it's a path
                if (grid[y][x] == '.') {
                    accessibleCells.push_back({y, x});
                }
            }
        }
        
        return accessibleCells;
    }
    
    // Place specific characters in the maze
    void placeCharacters(int numH, int numB, int numT, int numM) {
        // Get all accessible path cells
        std::vector<std::pair<int, int>> accessibleCells = getAccessiblePathCells();
        
        // Check if we have enough cells to place all characters
        int totalChars = numH + numB + numT + numM;
        if (totalChars > accessibleCells.size()) {
            std::cout << "Warning: Not enough space to place all characters. "
                      << "Reducing the number of characters to fit." << std::endl;
            
            // Prioritize H and B (1 each) over T and M
            if (numH > 1) numH = 1;
            if (numB > 1) numB = 1;
            
            int remainingCells = accessibleCells.size() - numH - numB;
            if (remainingCells < 0) {
                numH = (accessibleCells.size() > 0) ? 1 : 0;
                numB = (accessibleCells.size() > 1) ? 1 : 0;
                numT = 0;
                numM = 0;
            } else {
                // Distribute remaining cells between T and M
                if (numT + numM > remainingCells) {
                    double tRatio = static_cast<double>(numT) / (numT + numM);
                    numT = static_cast<int>(remainingCells * tRatio);
                    numM = remainingCells - numT;
                }
            }
        }
        
        // Shuffle the cells for random placement
        std::shuffle(accessibleCells.begin(), accessibleCells.end(), rng);
        
        // Place characters using the shuffled cells
        int cellIndex = 0;
        
        // Place H characters
        for (int i = 0; i < numH && cellIndex < accessibleCells.size(); i++) {
            auto [y, x] = accessibleCells[cellIndex++];
            grid[y][x] = 'H';
        }
        
        // Place B characters
        for (int i = 0; i < numB && cellIndex < accessibleCells.size(); i++) {
            auto [y, x] = accessibleCells[cellIndex++];
            grid[y][x] = 'B';
        }
        
        // Place T characters
        for (int i = 0; i < numT && cellIndex < accessibleCells.size(); i++) {
            auto [y, x] = accessibleCells[cellIndex++];
            grid[y][x] = 'T';
        }
        
        // Place M characters
        for (int i = 0; i < numM && cellIndex < accessibleCells.size(); i++) {
            auto [y, x] = accessibleCells[cellIndex++];
            grid[y][x] = 'M';
        }
        
        // Verify that all characters can reach the entrance and exit
        // If not connected, fix connectivity
        if (!isFullyConnected()) {
            fixConnectivity();
        }
    }
    
    // Function to verify that all characters are accessible from entrance
    bool verifyAccessibility() {
        // BFS from entrance
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::queue<std::pair<int, int>> q;
        q.push({0, 0}); // Entrance at top-left
        visited[0][0] = true;
        
        int dy[] = {-1, 1, 0, 0}; // up, down, left, right
        int dx[] = {0, 0, -1, 1};
        
        while (!q.empty()) {
            auto [cy, cx] = q.front();
            q.pop();
            
            for (int i = 0; i < 4; i++) {
                int ny = cy + dy[i];
                int nx = cx + dx[i];
                
                if (ny >= 0 && ny < height && nx >= 0 && nx < width && 
                    isWalkable(grid[ny][nx]) && !visited[ny][nx]) {
                    visited[ny][nx] = true;
                    q.push({ny, nx});
                }
            }
        }
        
        // Check if all H, B, T, M characters are accessible
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                char cell = grid[y][x];
                if ((cell == 'H' || cell == 'B' || cell == 'T' || cell == 'M') && !visited[y][x]) {
                    return false; // Found inaccessible character
                }
            }
        }
        
        // Check if exit is accessible
        return visited[height-1][width-1];
    }
};

int main() {
    int width, height;
    int numH, numB, numT, numM;
    
    std::cout << "Enter maze width: ";
    std::cin >> width;
    
    std::cout << "Enter maze height: ";
    std::cin >> height;
    
    if (width <= 0 || height <= 0) {
        std::cout << "Error: Width and height must be positive integers." << std::endl;
        return 1;
    }
    
    // Get the number of each character to place
    std::cout << "Enter number of H characters (should be 1): ";
    std::cin >> numH;
    
    std::cout << "Enter number of B characters (should be 1): ";
    std::cin >> numB;
    
    std::cout << "Enter number of T characters: ";
    std::cin >> numT;
    
    std::cout << "Enter number of M characters: ";
    std::cin >> numM;
    
    // Validate input
    if (numH < 0 || numB < 0 || numT < 0 || numM < 0) {
        std::cout << "Error: Character counts must be non-negative." << std::endl;
        return 1;
    }
    
    Maze maze(width, height);
    maze.generateMaze();
    maze.addEntranceAndExit();
    
    // Place the characters
    maze.placeCharacters(numH, numB, numT, numM);
    
    // Verify accessibility
    if (!maze.verifyAccessibility()) {
        std::cout << "Warning: Some characters might not be accessible. Fixed connectivity." << std::endl;
    }
    
    std::cout << "\nGenerated Maze:" << std::endl;
    maze.printMaze();
    
    // std::cout << "\nLegend:" << std::endl;
    // std::cout << "# - Wall" << std::endl;
    // std::cout << ". - Path" << std::endl;
    // std::cout << "H - Hero" << std::endl;
    // std::cout << "B - Boss" << std::endl;
    // std::cout << "T - Treasure" << std::endl;
    // std::cout << "M - Monster" << std::endl;
    // std::cout << "Entrance: Top-left corner" << std::endl;
    // std::cout << "Exit: Bottom-right corner" << std::endl;
    
    return 0;
}
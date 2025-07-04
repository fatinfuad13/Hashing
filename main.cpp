
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <functional>
#include <algorithm>
#include <random>
#include <numeric>
#include <memory>

#include "RedBlackTree.cpp" // Your RBT implementation

using namespace std;
using namespace std::chrono;

int LINEAR_STEP = 3;

bool isPrime(int n) /// done
{
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if (n % 2 == 0 || n % 3 == 0)
        return false;

    for (int i = 5; i * i <= n; i += 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return false;

    return true;
}

// Helper: get the next prime ≥ n
int nextPrime(int n) /// done
{
    while (!isPrime(n))
        n++;

    return n;
}




// --- Random String Generator ---
string randomString(int minLen = 5, int maxLen = 10) /// done
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    int len = minLen + rand() % (maxLen - minLen + 1);
    string s;
    for (int i = 0; i < len; ++i)
        s += charset[rand() % 26];
    return s;
}

vector<string> generateWords(int count) /// done
{
   vector<string> words;
    for (int i = 0; i < count; ++i)
        words.push_back(randomString());
    return words;
}

// --- Hash Functions ---
int modHash(const string& s, int tableSize) /// hash function 1
{
    hash<string> hasher;
    return hasher(s) % tableSize;
}

int polyHash(const string& s, int tableSize, int p = 31, int m = 1e9 + 9) /// hash function 2
{
    long long hashVal = 0;
    long long p_pow = 1;
    for (char c : s)
    {
        hashVal = (hashVal + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hashVal % tableSize;
}

int secondHash(const string& s, int tableSize)
{
    int hashVal = 0;
    for (char c : s) hashVal = (hashVal * 131 + c) % tableSize;
    return (hashVal == 0 ? 1 : hashVal);
}

/*int getChainLength(int index) const
{
    if (strategy != SEPARATE_CHAINING_RBT) return 0;
    return chainTable[index]->size();
}*/



// --- General HashTable Modes ---
enum CollisionStrategy
{
    LINEAR_PROBING,
    DOUBLE_HASHING,
    SEPARATE_CHAINING_RBT
};

// --- General HashTable Class ---
class HashTable
{
private:
    struct Entry
    {
        string key;
        bool occupied = false;
        bool deleted = false;
    };

    vector<Entry> openTable;
    vector<unique_ptr<RedBlackTree>> chainTable;
    int capacity;
    CollisionStrategy strategy;
    function<int(const string&)> primaryHash; // hash function 1
    function<int(const string&)> secondaryHash; // hash function 2

public:
    HashTable(int size, CollisionStrategy mode,
              function<int(const string&)> h1,
              function<int(const string&)> h2 = nullptr)
        : capacity(size), strategy(mode), primaryHash(h1), secondaryHash(h2)
    {
        if (strategy == SEPARATE_CHAINING_RBT)
        {
            chainTable.resize(capacity);
            for (int i = 0; i < capacity; ++i)
                chainTable[i] = make_unique<RedBlackTree>();
        }
        else
        {
            openTable.resize(capacity);
        }
    }

    int getChainLength(int index) const /// only applies for chaining method of collision resolution
{
    if (strategy != SEPARATE_CHAINING_RBT) return 0;
    return chainTable[index]->getNodeCount();  // You must implement size() in RedBlackTree
}


    /*int insert(const string& key)
    {
        if (strategy == SEPARATE_CHAINING_RBT)
        {
            int idx = primaryHash(key);
            auto node = chainTable[idx]->searchTree((int)hash<string> {}(key));
            if (node == chainTable[idx]->getSentinelNode())
            {
                chainTable[idx]->insertNode((int)hash<string> {}(key), key, 0);
                return 0;
            }
            return 1; // collision
        }

        /// linear probing / double hashing
        int h1 = primaryHash(key);
        int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key) : 1;
        int probes = 0;

        for (int i = 0; i < capacity; ++i)
        {
            int idx = (h1 + i * h2) % capacity;
            probes++;
            if (!openTable[idx].occupied || openTable[idx].deleted)
            {
                openTable[idx] = {key, true, false};
                return probes - 1;
            }
        }
        return probes;
    }

    int search(const string& key)
    {
        if (strategy == SEPARATE_CHAINING_RBT)
        {
            int idx = primaryHash(key);
            auto node = chainTable[idx]->searchTree((int)hash<string> {}(key));
            return node == chainTable[idx]->getSentinelNode() ? 1 : 1;
        }

        int h1 = primaryHash(key);
        int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key) : 1;
        int probes = 0;
        for (int i = 0; i < capacity; ++i)
        {
            int idx = (h1 + i * h2) % capacity;
            probes++;
            if (!openTable[idx].occupied && !openTable[idx].deleted) break;
            if (openTable[idx].occupied && !openTable[idx].deleted && openTable[idx].key == key) break;
        }
        return probes;
    }

    void remove(const string& key)
    {
        if (strategy == SEPARATE_CHAINING_RBT)
        {
            int idx = primaryHash(key);
            chainTable[idx]->deleteNode((int)hash<string> {}(key));
            return;
        }

        int h1 = primaryHash(key);
        int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key) : 1;
        for (int i = 0; i < capacity; ++i)
        {
            int idx = (h1 + i * h2) % capacity;
            if (openTable[idx].occupied && openTable[idx].key == key)
            {
                openTable[idx].deleted = true;
                return;
            }
        }
    }

    bool contains(const string& key)
    {
        if (strategy == SEPARATE_CHAINING_RBT)
        {
            int idx = primaryHash(key);
            auto node = chainTable[idx]->searchTree((int)hash<string> {}(key));
            return node != chainTable[idx]->getSentinelNode();
        }

        int h1 = primaryHash(key);
        int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key) : 1;

        for (int i = 0; i < capacity; ++i)
        {
            int idx = (h1 + i * h2) % capacity;
            if (!openTable[idx].occupied && !openTable[idx].deleted)
                return false;
            if (openTable[idx].occupied && !openTable[idx].deleted && openTable[idx].key == key)
                return true;
        }
        return false;
    }*/

    int insert(const string& key)
{

    if (strategy == SEPARATE_CHAINING_RBT)
    {
        int idx = primaryHash(key);
        if (chainTable[idx]->searchTree(key) == chainTable[idx]->getSentinelNode())
        {
            chainTable[idx]->insertNode(key, key, 0);
            return 0;
        }
        return 1; // already present
    }

    int h1 = primaryHash(key);
    int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key)
             : strategy == LINEAR_PROBING ? LINEAR_STEP
             : 1;

    int probes = 0;
    for (int i = 0; i < capacity; ++i)
    {
        int idx = (h1 + i * h2) % capacity;
        probes++;

        if (!openTable[idx].occupied || openTable[idx].deleted) // unoccupied or has been deleted
        {
            openTable[idx] = {key, true, false};
            return probes - 1;
        }

        if (openTable[idx].occupied && !openTable[idx].deleted && openTable[idx].key == key) // already exists, do not insert
            return probes - 1;
    }
    return probes;
}

int search(const string& key)
{
    if (strategy == SEPARATE_CHAINING_RBT)
    {
        int idx = primaryHash(key);
        auto node = chainTable[idx]->searchTree(key);
        return 1; // always 1 probe for chaining
    }

    int h1 = primaryHash(key);
    int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key)
             : strategy == LINEAR_PROBING ? LINEAR_STEP
             : 1;

    int probes = 0;
    for (int i = 0; i < capacity; ++i)
    {
        int idx = (h1 + i * h2) % capacity;
        probes++;

        if (!openTable[idx].occupied && !openTable[idx].deleted)
            break;

        if (openTable[idx].occupied && !openTable[idx].deleted && openTable[idx].key == key)
            break;
    }
    return probes;
}

bool contains(const string& key) /// not used
{
        if (strategy == SEPARATE_CHAINING_RBT)
        {
            int idx = primaryHash(key);
            auto node = chainTable[idx]->searchTree(key);
            return 1;
        }


    int h1 = primaryHash(key);
    int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key)
             : strategy == LINEAR_PROBING ? LINEAR_STEP
             : 1;

    for (int i = 0; i < capacity; ++i)
    {
        int idx = (h1 + i * h2) % capacity;

        if (!openTable[idx].occupied && !openTable[idx].deleted)
            return false;

        if (openTable[idx].occupied && !openTable[idx].deleted && openTable[idx].key == key)
            return true;
    }
    return false;
}

void remove(const string& key)
{
        if (strategy == SEPARATE_CHAINING_RBT)
        {
            int idx = primaryHash(key);
            chainTable[idx]->deleteNode(key);
            return;
        }


    int h1 = primaryHash(key);
    int h2 = (strategy == DOUBLE_HASHING && secondaryHash) ? secondaryHash(key)
             : strategy == LINEAR_PROBING ? LINEAR_STEP
             : 1;

    for (int i = 0; i < capacity; ++i)
    {
        int idx = (h1 + i * h2) % capacity;

        if (openTable[idx].occupied && openTable[idx].key == key) // occupied and equal to key so mark as deleted
        {
            openTable[idx].deleted = true;
            return;
        }

        if (!openTable[idx].occupied && !openTable[idx].deleted) // unoccupied and not deleted
            break;
    }
}

bool isBucketEmpty(int index) const {
    if (strategy == SEPARATE_CHAINING_RBT && index >= 0 && index < capacity)
        return chainTable[index]->isEmpty();
    return true; // for other strategies, treat as empty
}



};

// --- Benchmarking ---
/*void benchmark(int tableSize, double loadFactor, CollisionStrategy mode,
               function<int(const string&)> h1,
               function<int(const string&)> h2 = nullptr)
{


    tableSize = nextPrime(20000);
    int totalWords = 10000;

    vector<string> words = generateWords(totalWords);
    HashTable table(tableSize, mode, h1, h2);

    vector<string> inserted;
    int value = 1;
    int totalInsertProbes = 0;

    for (const string& word : words)
    {
        if (!table.contains(word))
        {
            totalInsertProbes += table.insert(word);
            inserted.push_back(word);
            ++value;
        }
    }

    cout << "\n[Mode=" << (mode == LINEAR_PROBING ? "Linear" : mode == DOUBLE_HASHING ? "Double" : "Chaining")
         << "] Table Size: " << tableSize << ", Total Unique Inserts: " << inserted.size() << endl;
    cout << "Insert Total Probes: " << totalInsertProbes << endl;

    // Search before deletion
    int n = inserted.size();
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), default_random_engine(time(0)));

    vector<string> searchSet;
    for (int i = 0; i < n / 10; ++i) searchSet.push_back(inserted[indices[i]]);

    int totalSearchProbes = 0;
    auto t1 = high_resolution_clock::now();
    for (const string& word : searchSet)
        totalSearchProbes += table.search(word);
    auto t2 = high_resolution_clock::now();
    double avgSearchTime = duration<double, mili>(t2 - t1).count() / searchSet.size();

    // Deletion
    for (int i = 0; i < searchSet.size() / 2; ++i)
        table.remove(searchSet[i]);

    vector<string> mixedSearchSet;
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(searchSet[i]);  // deleted half
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(inserted[indices[n / 10 + i]]);  // still present

    int totalMixedProbes = 0;
    auto t3 = high_resolution_clock::now();
    for (const string& word : mixedSearchSet)
        totalMixedProbes += table.search(word);
    auto t4 = high_resolution_clock::now();
    double avgMixedTime = duration<double, mili>(t4 - t3).count() / mixedSearchSet.size();

    cout << "Search Before Deletion: Avg Time (us): " << avgSearchTime
         << ", Avg Probes: " << totalSearchProbes / (double)searchSet.size() << endl;

    cout << "Search After Deletion: Avg Time (us): " << avgMixedTime
         << ", Avg Probes: " << totalMixedProbes / (double)mixedSearchSet.size() << endl;
}

/*void benchmark(int tableSize, double loadFactor, CollisionStrategy mode,
               function<int(const string&)> h1,
               function<int(const string&)> h2 = nullptr)
{
    int n = loadFactor * tableSize;
    vector<string> dataset = generateWords(n);  // generate independent words without filtering duplicates in hash

    HashTable table(tableSize, mode, h1, h2);

    int totalProbesInsert = 0;
    int totalCollisionsInsert = 0;

    for (const auto& word : dataset)
    {
        int collisions = table.insert(word);
        totalCollisionsInsert += collisions;
        totalProbesInsert += (collisions + 1); // probes = collisions + 1, just for reference
    }

    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), default_random_engine(time(0)));

    vector<string> searchSet;
    for (int i = 0; i < n / 10; ++i)
        searchSet.push_back(dataset[indices[i]]);

    int totalProbesSearch = 0;
    auto t1 = high_resolution_clock::now();
    for (const auto& word : searchSet)
        totalProbesSearch += table.search(word);
    auto t2 = high_resolution_clock::now();
    double avgSearchTime = duration<double, mili>(t2 - t1).count() / searchSet.size();

    for (int i = 0; i < searchSet.size() / 2; ++i)
        table.remove(searchSet[i]);

    vector<string> mixedSearchSet;
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(searchSet[i]);
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(dataset[indices[n / 10 + i]]);

    int totalProbesMixed = 0;
    auto t3 = high_resolution_clock::now();
    for (const auto& word : mixedSearchSet)
        totalProbesMixed += table.search(word);
    auto t4 = high_resolution_clock::now();
    double avgMixedTime = duration<double, mili>(t4 - t3).count() / mixedSearchSet.size();

    cout << "\n[Mode=" << (mode == LINEAR_PROBING ? "Linear" : mode == DOUBLE_HASHING ? "Double" : "Chaining")
         << "] Load Factor: " << loadFactor << " Table Size: " << tableSize << " Total Unique Inserts: " << n << endl;
    cout << "Insert Total Probes: " << totalProbesInsert << endl;
    cout << "Insert Total Collisions: " << totalCollisionsInsert << endl;
    cout << "Search Before Deletion: Avg Time (us): " << avgSearchTime
         << ", Avg Probes: " << totalProbesSearch / (double)searchSet.size() << endl;
    cout << "Search After Deletion: Avg Time (us): " << avgMixedTime
         << ", Avg Probes: " << totalProbesMixed / (double)mixedSearchSet.size() << endl;
}*/


/*void benchmark(int tableSize, double loadFactor, CollisionStrategy mode,const vector<string>& generatedWords,
               function<int(const string&)> h1,
               function<int(const string&)> h2 = nullptr)
{
    //tableSize = nextPrime(20000);  // keep as you originally had
   // int totalWords = 10000;


    int numInsertions = floor(loadFactor * tableSize); /// Calculates the number of insertions for this load factor
    vector<string> words(generatedWords.begin(), generatedWords.begin() + numInsertions);

    HashTable table(tableSize, mode, h1, h2);

    vector<string> inserted;
    int value = 1;
    int totalInsertProbes = 0;
    int totalInsertCollisions = 0;  // added line

    for (const string& word : words)
    {
        int probes = table.insert(word);  // insert will skip duplicates itself
        if (probes >= 0)  // valid insert (0 probes means inserted without collision)
        {
            totalInsertProbes += probes;
            if (probes > 0)
                ++totalInsertCollisions;  // probe > 0 => collision
            inserted.push_back(word);
            ++value;
        }
    }


    cout << "\n[Mode=" << (mode == LINEAR_PROBING ? "Linear" : mode == DOUBLE_HASHING ? "Double" : "Chaining")
         << "] Table Size: " << tableSize << ", Total Unique Inserts: " << inserted.size() << endl;
    cout << "Insert Total Probes: " << totalInsertProbes << endl;
    cout << "Insert Total Collisions: " << totalInsertCollisions << endl;  // added line

    // Search before deletion
    // Selecting 10% randomly which we will search for later
    int n = inserted.size();
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), default_random_engine(time(0)));

    vector<string> searchSet;
    for (int i = 0; i < n / 10; ++i)
        searchSet.push_back(inserted[indices[i]]);


    int totalSearchProbes = 0;
    auto t1 = high_resolution_clock::now(); /// clock starts

        for (const string& word : searchSet)
             totalSearchProbes += table.search(word);



    auto t2 = high_resolution_clock::now(); /// clock ends
    double avgSearchTime = duration<double, milli>(t2 - t1).count() /(double) searchSet.size();

    // Deletion
    for (int i = 0; i < searchSet.size() / 2; ++i)
        table.remove(searchSet[i]);

    vector<string> mixedSearchSet;
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(searchSet[i]);  // deleted half of search set is pushed
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(inserted[indices[n / 10 + i]]);  // the other half that is still present is pushed, note to take is from index n/10 + 1 is surely present

    int totalMixedProbes = 0;
    auto t3 = high_resolution_clock::now(); /// clock starts

           for (const string& word : mixedSearchSet)
                totalMixedProbes += table.search(word);


    auto t4 = high_resolution_clock::now(); /// clock ends
    double avgMixedTime = duration<double, milli>(t4 - t3).count() / (double)mixedSearchSet.size();

    cout << "Search Before Deletion: Avg Time (us): " << avgSearchTime
         << ", Avg Probes: " << totalSearchProbes / (double)searchSet.size() << endl;

    cout << "Search After Deletion: Avg Time (us): " << avgMixedTime
         << ", Avg Probes: " << totalMixedProbes / (double)mixedSearchSet.size() << endl;
}*/


void benchmark(int tableSize, double loadFactor, CollisionStrategy mode, const vector<string>& generatedWords,
               function<int(const string&)> h1,
               function<int(const string&)> h2 = nullptr)
{
    int numInsertions = floor(loadFactor * tableSize);
    vector<string> words(generatedWords.begin(), generatedWords.begin() + numInsertions);

    HashTable table(tableSize, mode, h1, h2);

    vector<string> inserted;
    int totalInsertProbes = 0;
    int totalInsertCollisions = 0;

    for (const string& word : words)
    {
        if (mode == SEPARATE_CHAINING_RBT)
        {
            int idx = h1(word);
           if (!table.isBucketEmpty(idx))
                                      // collision if bucket not empty
                totalInsertCollisions++;

            int probes = table.insert(word);
            if (probes >= 0)
            {
                inserted.push_back(word);
            }
        }
        else
        {
            int probes = table.insert(word);
            if (probes >= 0)
            {
                totalInsertProbes += probes;
                if (probes > 0)
                    ++totalInsertCollisions;
                inserted.push_back(word);
            }
        }
    }

    cout << "\n[Mode=" << (mode == LINEAR_PROBING ? "Linear" : mode == DOUBLE_HASHING ? "Double" : "Chaining")
         << "] Table Size: " << tableSize << ", Total Unique Inserts: " << inserted.size() << endl;

    if (mode != SEPARATE_CHAINING_RBT)
    {
        cout << "Insert Total Collisions: " << totalInsertProbes << endl;
    }
    cout << "Insert Total Collisions: " << totalInsertCollisions << endl;

    int n = inserted.size();
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), default_random_engine(time(0)));

    vector<string> searchSet;
    for (int i = 0; i < n / 10; ++i)
        searchSet.push_back(inserted[indices[i]]);

    int totalSearchProbes = 0;
    auto t1 = high_resolution_clock::now();

    for(int i=0;i<1000;i++)
    {
         for (const string& word : searchSet)
               totalSearchProbes += table.search(word);
    }


    auto t2 = high_resolution_clock::now();
    double avgSearchTime = duration_cast<duration<double, micro>>(t2 - t1).count() / searchSet.size();

    for (int i = 0; i < searchSet.size() / 2; ++i)
        table.remove(searchSet[i]);

    vector<string> mixedSearchSet;
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(searchSet[i]);
    for (int i = 0; i < searchSet.size() / 2; ++i)
        mixedSearchSet.push_back(inserted[indices[n / 10 + i]]);

    int totalMixedProbes = 0;
    auto t3 = high_resolution_clock::now();

    for(int i=0;i<1000;i++)
    {
        for (const string& word : mixedSearchSet)
            totalMixedProbes += table.search(word);

    }


    auto t4 = high_resolution_clock::now();
    double avgMixedTime = duration_cast<duration<double, micro>>(t4 - t3).count() / mixedSearchSet.size();

    cout << "Search Before Deletion: Avg Time (us): " << avgSearchTime
         << ", Avg Probes: " << totalSearchProbes / ( (double)searchSet.size()*1000)<< endl;

    cout << "Search After Deletion: Avg Time (us): " << avgMixedTime
         << ", Avg Probes: " << totalMixedProbes / ( (double)mixedSearchSet.size() * 1000 ) << endl;
}




int main()
{
    srand(time(0));
    int initialSize = 10000; // this is N', take as input maybe?

    int tableSize = nextPrime(initialSize); // Use next prime for better distribution

    int totalWords = 10000;
    vector<string> words = generateWords(totalWords);

    vector<double> factors = {0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    for (double alpha : factors)
    {
        cout << "\n=== Load Factor: " << alpha << " ===\n";

        // LINEAR PROBING with modHash
        cout << "\n--- LINEAR with modHash ---";
        benchmark(tableSize, alpha, LINEAR_PROBING,words,
            [&](const string& s) {
                return modHash(s, tableSize);
            });

        // LINEAR PROBING with polyHash
        cout << "\n--- LINEAR with polyHash ---";
        benchmark(tableSize, alpha, LINEAR_PROBING,words,
            [&](const string& s) {
                return polyHash(s, tableSize);
            });

        // DOUBLE HASHING with modHash + polyHash
        cout << "\n--- DOUBLE with modHash + polyHash ---";
        benchmark(tableSize, alpha, DOUBLE_HASHING,words,
            [&](const string& s) {
                return modHash(s, tableSize);
            },
            [&](const string& s) {
                return polyHash(s, tableSize);
            });

        // DOUBLE HASHING with polyHash + modHash
        cout << "\n--- DOUBLE with polyHash + modHash ---";
        benchmark(tableSize, alpha, DOUBLE_HASHING,words,
            [&](const string& s) {
                return polyHash(s, tableSize);
            },
            [&](const string& s) {
                return modHash(s, tableSize);
            });

        // CHAINING with modHash
        cout << "\n--- CHAINING with modHash ---";
        benchmark(tableSize, alpha, SEPARATE_CHAINING_RBT,words,
            [&](const string& s) {
                return modHash(s, tableSize);
            });

        // CHAINING with polyHash
        cout << "\n--- CHAINING with polyHash ---";
        benchmark(tableSize, alpha, SEPARATE_CHAINING_RBT,words,
            [&](const string& s) {
                return polyHash(s, tableSize);
            });


    }

    return 0;
}


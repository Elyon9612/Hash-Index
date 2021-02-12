//2/12/2021
//Hash Indexing
//Honghao Li & Yue Xu

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <queue>
#include <cmath>
#include <unordered_map>
using namespace std;

unsigned int numBuffers, bufferSize;

class Block{
    vector<int> ID;
    Block *overflow;

public:
    //Initial the Block
    Block(){
        overflow = NULL;
        ID.clear();
    }

    bool Present(int x){
        Block *node = this;
        while (node){
            for (unsigned int i = 0; i < node->ID.size(); i++){
                if (node->ID[i] == x)
                {
                    return true;
                }
            }
            node = node->overflow;
        }
        return false;
    }

    void add(int x){
        if (ID.size() < (bufferSize / 4)){
            ID.push_back(x);
        }
        else{
            if (overflow == NULL){
                overflow = new Block();
            }
            overflow->add(x);
        }
    }

    void clearElements(vector<int> &v){
        for (unsigned int i = 0; i < ID.size(); i++){
            v.push_back(ID[i]);
        }
        ID.clear();
        if (overflow){
            overflow->clearElements(v);
            delete overflow;
            overflow = NULL;
        }
    }
};

class HashTable{
    int numRecords, numBits;
    vector<Block *> buckets;

public:
    HashTable(){
        // initial configuration of Hash table
        numRecords = 0;
        numBits = 1;
        buckets.push_back(new Block());
        buckets.push_back(new Block());
    }

    unsigned int hash(int x){
        unsigned int mod = (1 << numBits);
        return (unsigned int)(x % mod + mod) % mod;
    }

    int occupancy(){
        double ratio = 1.0 * numRecords / buckets.size();
        return (int)(100 * (ratio / (bufferSize / 4)));
    }

    bool Present(int x){
        unsigned int k = hash(x);
        if (k >= buckets.size()){
            k -= (1 << (numBits - 1));
        }
        if (buckets[k]->Present(x)){
            return true;
        }
        return false;
    }

    void insert(int x){
        unsigned int k = hash(x);
        if (k >= buckets.size()){
            k -= (1 << (numBits - 1));
        }
        buckets[k]->add(x);
        numRecords++;
        while (occupancy() >= 80){
            buckets.push_back(new Block());
            numBits = ceil(log2((double)buckets.size()));
            // split old bucket and rehash
            k = buckets.size() - 1 - (1 << (numBits - 1));
            vector<int> v;
            buckets[k]->clearElements(v);
            for (unsigned int i = 0; i < v.size(); i++)
            {
                buckets[hash(v[i])]->add(v[i]);
            }
        }
    }
};


//Basic Hash table
unordered_map<int,string> table;
HashTable h;

// Buffered IO
queue<int> inputBuffer, outputBuffer;
unsigned int inputSize, outputSize;

void clearOutput(){
    int num;
    ofstream F("Output.txt");
    while (!outputBuffer.empty()){
        num = outputBuffer.front();
        outputBuffer.pop();
        F << num;
        F << table[num] << endl;
    }
    F.close();
}

void clearInput(){
    int x;
    while (!inputBuffer.empty()){
        x = inputBuffer.front();
        inputBuffer.pop();
        if (!h.Present(x)){
            h.insert(x);
            if (outputBuffer.size() == outputSize){
                clearOutput();
            }
            outputBuffer.push(x);
        }
    }
}

int main(int argc, char *argv[]){
    int search_key;
    if(argc == 2) {
        cout << "Type the ID number for searching: ";
        cin >> search_key;
    }

    ifstream input("Employees.csv");
    numBuffers = 2;
    bufferSize = 32768;     // (bit)

    // as sizeof(int) is 4
    inputSize = (numBuffers - 1) * (bufferSize / 4);
    outputSize = bufferSize / 4;

    string line;
    while(getline(input,line)){
        string s = line.substr(0,8);
        // cout << s << endl;
        size_t pos = line.find(",");
        string last = line.substr(pos);
        
        int x = stoi(s);
        table.insert({x,last});
            if(inputBuffer.size() < inputSize) {
                inputBuffer.push(x);
            }
            else {
                clearInput();
                inputBuffer.push(x);
            }
    }
    input.close();
    clearInput();
    clearOutput();
    
    if(table.find(search_key) != table.end()){
        cout << search_key << table[search_key] << endl;
    }

    return 0;
}
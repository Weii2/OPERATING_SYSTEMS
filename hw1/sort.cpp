#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <vector>
#include <array>
#include <iterator>
using namespace std;

class Compare
{
public:
    bool operator() (array<long,2> p1, array<long,2> p2) {
        return p1[0] > p2[0];
    }
};

void mergeFiles(int BlockNum) {
    priority_queue<array<long,2>, vector<array<long,2>>, Compare> minHeap;
    
    ifstream* inFiles = new ifstream[BlockNum];

    for(int i=1; i<=BlockNum; ++i) {
        inFiles[i-1].open("output" + to_string(i) + ".txt");
        long FV;
        inFiles[i-1] >> FV;
        array<long,2> p{ {FV, i-1} };
		minHeap.push(p);
    }

    FILE *outfp=fopen("output.txt","w");
 
    while(minHeap.size()>0) {
		array<long,2> mp{ {minHeap.top()[0], minHeap.top()[1]} };
        minHeap.pop();
		fprintf(outfp,"%ld\n",mp[0]);
        long NV;
        if(inFiles[mp[1]] >> NV) {
            array<long,2> np{ {NV, mp[1]} };
            minHeap.push(np);
		}
    }
    for(int i=1; i<=BlockNum; ++i)
        inFiles[i-1].close();
    delete[] inFiles;
    fclose(outfp);
}

void sort_Out(vector<long> &inputNum, int ArrSize, int BlockNum) {
    sort(inputNum.begin(), inputNum.begin()+ArrSize); 
    ofstream outFile{"output" + to_string(BlockNum) + ".txt", ios::out};
    copy(begin(inputNum), begin(inputNum)+ArrSize, ostream_iterator<long>(outFile, "\n"));
    outFile.close();
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(0);

    int BlockNum = 1;
    long long int MemSize = 3221225472;
    long long int BlockSize = MemSize / sizeof(long);
    vector<long> inputNum(BlockSize);
    long input = 0;	int count = 0;
    bool HaveNumsToDo = true;
    ifstream inFile("input.txt");

    while(inFile >> input) {
        HaveNumsToDo=true;
        inputNum[count++]=input;
        if(count == BlockSize) {
            sort_Out(inputNum, count, BlockNum);
            BlockNum++;
            count=0;
            HaveNumsToDo = false;
        }
    }

    if(HaveNumsToDo)
        sort_Out(inputNum, count, BlockNum);
    else
        BlockNum--;

    inFile.close();

    inputNum.clear();
    inputNum.shrink_to_fit();

    if(BlockNum!=0) 
        mergeFiles(BlockNum);

    return 0;
}

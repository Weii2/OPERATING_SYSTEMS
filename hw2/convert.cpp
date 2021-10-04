#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include <chrono>

using namespace std;
int num_write;
int num_read;
int two_count1=0;
int two_count2=0;
pthread_mutex_t mutex;

class LinkedList;
class ListNode{
public:
    friend class LinkedList;
    ListNode():data(0),next(0){};
    ListNode(int a):data(a),next(0){};
private:
    int data;
    ListNode *next;
};

class LinkedList{
public:
    LinkedList():first(0), rear(0){};   
    void PrintList_toFile(ofstream& outFile, int num) {
	    ListNode *current = first;
	    while (current != 0) {
	    	outFile << "{" << endl;
			for(int j=0; j<19; ++j) {
				outFile << "\"col_" << j+1 << "\":" << current->data << "," << endl;
	        	current = current->next;
			}
	        outFile << "\"col_20\":" << current->data << endl;
	        current = current->next;
			if(current==0 && num==num_read-1 )
				outFile << "}" << endl;
			else
				outFile << "}," << endl;
    	}
	}
    void Push_back(int x){
    	ListNode *newNode = new ListNode(x);
	    if (first == 0) {
	        first = rear = newNode;
	        return;
	    }
	    rear->next = newNode;
        rear=newNode;
	}
private:
    ListNode *first;
    ListNode *rear;
};

struct arg_struct {
	LinkedList* in;
    int arg1;
    int arg2;
};

void* read_csv(void *arg)
{
    pthread_mutex_unlock(&mutex);
    
    arg_struct* argument = ((arg_struct*)arg);
	int offset = argument->arg1;
	int num = argument->arg2;
	
	ifstream inFile("input.csv");
	inFile.seekg(offset*num, inFile.beg);
	string line;
	if( (offset*num)!=0 )
		getline(inFile, line);
	while(inFile.tellg() < (offset*num + offset) && getline(inFile, line)) {
		istringstream split(line);
   		for(string each; getline(split, each, '|'); argument->in->Push_back(stoi(each)));
	}
}

void* write_json(void* arg)
{
    pthread_mutex_unlock(&mutex);
    arg_struct* argument = ((arg_struct*)arg);
	int offset = argument->arg1;
	int num = argument->arg2;
    
	ofstream outFile;
	sleep(1);
	if(num==0) {
        outFile.open("output.json", ios::out);
		outFile.seekp(0, outFile.beg);	
		outFile << "[";
	}
	else {
        sleep(1);
        outFile.open("output.json", ios::out | ios::in);
		outFile.seekp(3+offset*num*2, outFile.beg);
    }
    
	argument->in->PrintList_toFile(outFile, num);
	if(num == num_read-1)
		outFile << "]" << endl;
    outFile.close();
}

int main(int argc, char *argv[])
{
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    
	int num_thread=atoi(argv[1]);
	if(num_thread %2 == 0)
		num_write=num_read= num_thread/2;
	else {
		num_read = num_thread/2 +1;
		num_write = num_thread/2;
	}

	ifstream inFile("input.csv");
	inFile.seekg(0, inFile.end);
    int length = inFile.tellg()/num_read;
	
	vector<LinkedList> in(num_read);
	vector<arg_struct> p_arg(num_read);
    vector<pthread_t> t(num_thread);
	for(int i=0; i<num_read; ++i) {
		arg_struct temp={&in[i], length, i};
		p_arg[i]=temp;
	}
    for(int i=0; i<num_read; ++i)
    	pthread_create(&t[i], NULL, read_csv, &p_arg[i]);
    sleep(1);
	for(int i=0; i<num_write; ++i)
		pthread_create(&t[i+num_read], NULL, write_json, &p_arg[i]);
    for(int i=0; i<num_thread; ++i) {
    	pthread_join(t[i], NULL);
    	if(i==0 && (num_read!=num_write)) {
    		pthread_create(&t[i], NULL, write_json, &p_arg[num_read-1]);
    		pthread_join(t[i], NULL);
		}
	}
    if(num_thread==1) {
    	pthread_create(&t[0], NULL, write_json, &p_arg[0]);
    	pthread_join(t[0], NULL);
	}
    
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Time = " << chrono::duration_cast<chrono::microseconds>(end - begin).count() / 1000000.0 << "s" << endl;
	return 0;
}


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
using namespace std;

void scan_search(vector<string> &scan_vec, unsigned long long int &key_start, unsigned long long int &key_final, int tmpnum_s, int tmpnum_f)
{
	unsigned long long int tmp_key;
	string tmp_value;
	for(int i=tmpnum_s; i<=tmpnum_f; ++i) {
		ifstream inFile_for_search("./storage/" + to_string(i) + ".tmp", ios::in);
		while(inFile_for_search >> tmp_key >> tmp_value)
			if(tmp_key >= key_start && tmp_key <= key_final)
				scan_vec[tmp_key - key_start] = tmp_value;
		inFile_for_search.close();
	}
	return;
}

int main(int argc, char *argv[])
{
	ios_base::sync_with_stdio(false);
	
	int tmp_sep = 170000;
    ifstream inFile(argv[1]);
    string s(argv[1]);
    int s_num=0;
    for(int i=s.size()-1; i>=0; --i) {
    	if(s[i] == '/') {
    		s_num = s[i+1] - '0';
    		break;
		}
	}
	vector<ofstream> tmpFile(tmp_sep);
	ofstream outFile;
	int bou_f=0;
    if(s_num == 1) {
        system("mkdir -p ./storage");
        for(int i=0; i<tmp_sep; ++i) {
        	tmpFile[i].open("./storage/" + to_string(i+1) + ".tmp", ios::out);
        	tmpFile[i].close();
		}
    }
    else if(s_num == 2)
	    outFile.open("2.output");
    else if(s_num == 3)
        outFile.open("3.output");
    else
    	outFile.open("hw3example.output");
	string command;
	unsigned long long int read_key, key_start, key_final;
	string read_value;
	vector <unordered_map<unsigned long long int, string>> put_to_storage(tmp_sep);
	unordered_map<unsigned long long int, string> get_page;
	int get_tmp_num = 0;

	unsigned long long int sep_key = 9223372036854775807/tmp_sep;
    int tmpnum, tmpnum_s, tmpnum_f, bou = 15000000/tmp_sep, put_to_storage_index;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	while(inFile >> command) {
		if(command == "PUT") {
			inFile >> read_key >> read_value;
			if(read_key > sep_key * (tmp_sep-1)) {
				put_to_storage[tmp_sep-1][read_key] = read_value;
				put_to_storage_index = tmp_sep-1;
			}
			else {
				int p_index=read_key/sep_key;
				put_to_storage[p_index][read_key] = read_value;
				put_to_storage_index = p_index;
			}
			if(put_to_storage[put_to_storage_index].size() >= bou) {
				tmpFile[put_to_storage_index].open("./storage/" + to_string(put_to_storage_index+1) + ".tmp", ios::app);
				for(const auto& x: put_to_storage[put_to_storage_index])
				    tmpFile[put_to_storage_index] << x.first << " " << x.second << " ";
				put_to_storage[put_to_storage_index].clear();
				tmpFile[put_to_storage_index].close();
			}
		}
		else if(command == "GET") {
			inFile >> read_key;
			if(read_key >= sep_key * (tmp_sep-1))
				tmpnum = tmp_sep;
			else
				tmpnum = read_key/sep_key + 1;
			unordered_map<unsigned long long int, string>::const_iterator find = put_to_storage[tmpnum-1].find(read_key);
			if(find != put_to_storage[tmpnum-1].end()) {
				outFile << find->second << '\n';                                                    
				continue;
			}
			if(get_tmp_num == tmpnum) {
				find = get_page.find(read_key);
				if(find != get_page.end())
					outFile << find->second << '\n';
				else
					outFile << "EMPTY" << '\n';
				continue;
			}
			get_page.clear();
			get_tmp_num = tmpnum;
			ifstream in_tmp("./storage/" + to_string(tmpnum) + ".tmp", ios::in);
			unsigned long long int tmp_key=0;	string tmp_value="";
			while(in_tmp >> tmp_key >> tmp_value)
				get_page[tmp_key] = tmp_value;
			find = get_page.find(read_key);
			if(find != get_page.end())
				outFile << find->second << '\n';
			else
				outFile << "EMPTY" << '\n';
			in_tmp.close();
		}
		else if(command == "SCAN") {
			inFile >> key_start >> key_final;
			vector<string> scan_vec(key_final - key_start + 1);
			if(key_start >= sep_key * (tmp_sep-1))
				tmpnum_s = tmp_sep;
			else
				tmpnum_s = key_start/sep_key + 1;
			if(key_final >= sep_key * (tmp_sep-1))
				tmpnum_f = tmp_sep;
			else
				tmpnum_f = key_final/sep_key + 1;
			scan_search(scan_vec, key_start, key_final, tmpnum_s, tmpnum_f);
			for(int i=tmpnum_s; i<=tmpnum_f; ++i)
				for(auto& x: put_to_storage[i-1])
					if(x.first >= key_start && x.first <= key_final)
						scan_vec[x.first - key_start] = x.second;
			for(int i=0; i<scan_vec.size(); ++i) {
				if(scan_vec[i] == "")
					outFile << "EMPTY" << '\n';
				else
					outFile << scan_vec[i] << '\n';
			}
            scan_vec.clear();
            scan_vec.shrink_to_fit();
		}
	}
	for(int i=0; i<put_to_storage.size(); ++i) {
		tmpFile[i].open("./storage/" + to_string(i+1) + ".tmp", ios::app);
		for(auto& x: put_to_storage[i])
			tmpFile[i] << x.first << " " << x.second << " ";
		put_to_storage[i].clear();
		tmpFile[i].close();
	}
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Time = " << chrono::duration_cast<chrono::microseconds>(end - begin).count() / 1000000.0 << "s" << endl;	
    
	return 0;
}

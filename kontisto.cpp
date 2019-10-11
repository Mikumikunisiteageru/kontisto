#include<iostream>
#include<fstream>
#include<sstream>
// 20191007
// 20191008
using namespace std;
const string filename = "test.yaml";
const int hash_size = 1024, list_length = 4096, prime = 131;
string cathash[hash_size], acchash[hash_size];
int catfirst[hash_size], catlast[hash_size],
	accfirst[hash_size], acclast[hash_size];
long long catsum[hash_size], accsum[hash_size];
string lines[list_length];
int accnext[list_length], catnext[list_length];
long long cents[list_length];
long long round (double x) {
	if (x == 0)
		return 0;
	int sign = 1;
	if (x < 0) {
		x = -x;
		sign = -1;
	}
	return (long long) (x + 0.5) * sign;
}
long long m100 (double x) {
	return round(100 * x);
}
int loc (string hash[], string str) {
	int x = 0;
	for (int i = 0; i < str.length(); i ++)
		x = (x * prime + str[i]) % hash_size;
	while ((hash[x] != "") && (hash[x] != str))
		x = (x + 1) % hash_size;
	hash[x] = str;
	return x;
}
int linecode (string line) {
	if (line.length() == 0)
		return 0;
	switch (line[line.find_first_not_of(" \t")]) {
		case '#': return 0;
		case '+': return 0;
	}
	int i = line.find('\t', 0);
	i = line.find('\t', i + 1);
	if (i == -1)
		return -1;
	int j = line.find('\t', i + 1);
	if (j == -1)
		return 1;
	if (j - i == 1)
		return 3;
	return 2;
}
void reg (string hash[], int first[], int last[], int next[], 
		long long sum[], string str, int pointer) {
	int pos = loc(hash, str);
	if (last[pos] == 0)
		first[pos] = last[pos] = pointer;
	else
		last[pos] = next[last[pos]] = pointer;
	sum[pos] += cents[pointer];
}
void accreg (string acc, int pointer) {
	return reg(acchash, accfirst, acclast, accnext, accsum, acc, pointer);
}
void catreg (string cat, int pointer) {
	return reg(cathash, catfirst, catlast, catnext, catsum, cat, pointer);
}
int check (int s, int t, long long add, ofstream & fout) {
	if (s == 0)
		return 0;
	string cat, item;
	double one, factor = double(1) * cents[s] / add;
	long long sum = 0, psum;
	for (int j = s + 1; j <= t; j ++) {
		istringstream is(lines[j]);
		is >> cat >> item >> one;
		psum = sum;
		sum += m100(one);
		cents[j] = round(factor * sum) - round(factor * psum);
		catreg(cat, j);
	}
	return 0;
}
string formatint (char format [], long long x) {
	char chr[16];
	sprintf(chr, "%5.d", x);
	return string(chr);
}
string formatflt (long long x) {
	char chr[16];
	sprintf(chr, "%9.2f", x / 100.);
	return string(chr);
}
int main (void) {
	ifstream fin (filename);
	if (!fin) {
		cout << "No input!" << endl;
		getchar();
		return -1;
	}
	string line;
	ofstream fout ("wAnalysis.txt");
	int i = 0, last_code_2 = 0, accpos, catpos;
	string acc, cat, item;
	double one;
	long long cent, add = 0;
	while (!fin.eof()) {
		getline(fin, line);
		lines[++i] = line;
		switch (linecode(line)) {
			case 0: {
				last_code_2 = check(last_code_2, i-1, add, fout);
				break;
			}
			case 1: {
				last_code_2 = check(last_code_2, i-1, add, fout);
				istringstream is(line);
				is >> acc >> cat >> one;
				cents[i] = cent = m100(one) * (cat == "=+" ? 1 : -1);
				accreg(acc, i);
				break;
			}
			case 2: {
				last_code_2 = check(last_code_2, i-1, add, fout);
				istringstream is(line);
				is >> acc >> cat >> item >> one;
				cents[i] = cent = m100(one);
				accreg(acc, i);
				if (cat == ":") {
					add = 0;
					last_code_2 = i;
				}
				else 
					catreg(cat, i);
				break;
			}
			case 3: {
				if (last_code_2 == 0)
					break;
				istringstream is(line);
				is >> cat >> item >> one;
				add += m100(one);
				break;
			}
		}
		// fout << "Line " << i << ": Code " << linecode(line) << ": " << lines[i] << '\t' << cents[i] << endl;
	}
	fin.close();
	fout << "################" << endl;
	for (int i = 0; i < hash_size; i ++)
		if (acchash[i] != "")
			fout << acchash[i] << '\t' << formatflt(accsum[i]) << endl;
	fout << "################" << endl;
	for (int i = 0; i < hash_size; i ++)
		if (cathash[i] != "")
			fout << cathash[i] << '\t' << formatflt(catsum[i]) << endl;
	fout << "################" << endl;
	for (int i = 0; i < hash_size; i ++)
		if (acchash[i] != "") {
			fout << "ACCOUNT: " << acchash[i] << endl;
			int k = accfirst[i];
			while (k > 0) {
				char strout[16];
				sprintf(strout, "%5.d", k);
				fout << "\tLINE " << strout << ":\t" << lines[k] << '\t' << formatflt(cents[k]) << endl;
				k = accnext[k];
			}
		}
	fout << "################" << endl;
	for (int i = 0; i < hash_size; i ++)
		if (cathash[i] != "") {
			fout << "CATEGORY " << cathash[i] << endl;
			int k = catfirst[i];
			while (k > 0) {
				char strout[16];
				sprintf(strout, "%5.d", k);
				fout << "\tLINE " << strout << ":\t" << lines[k] << '\t' << cents[k] << endl;
				k = catnext[k];
			}
		}
	fout.close();
	cout << "OJBK" << endl;
	getchar();
	return 0;
}

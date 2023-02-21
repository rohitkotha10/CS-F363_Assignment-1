#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct NFATuple;
struct DFATuple;

vector<NFATuple> makeSingleNFA(int op);

void incrementNFATuple(NFATuple& tup, int incre);
void incrementNFA(vector<NFATuple>& nfa, int incre);

vector<NFATuple> makePlusNFA(vector<NFATuple>& nfa);
vector<NFATuple> makeStarNFA(vector<NFATuple>& nfa);
vector<NFATuple> makeOrNFA(vector<NFATuple>& nfa1, vector<NFATuple>& nfa2);

vector<NFATuple> concatNFA(vector<NFATuple>& nfa1, vector<NFATuple>& nfa2);

void checkAndAdd(vector<int>& arr, int elem);
bool sameVec(vector<int>& v1, vector<int>& v2);
vector<int> getEpsilonSingle(int curpos, vector<NFATuple>& nfa);
vector<int> getEpsilonAll(vector<int>& arr, vector<NFATuple>& nfa);
vector<int> getInputDFA(int op, vector<int>& arr, vector<NFATuple>& nfa);

int checkInDFA(vector<vector<int>>& dfaTable, vector<int>& arr);
bool checkFinalState(vector<int>& arr, vector<NFATuple>& nfa);
void processAndAdd(
    vector<DFATuple>& dfa, vector<NFATuple>& nfa, vector<vector<int>>& dfaTable, vector<int>& arr, int pos);
vector<DFATuple> convertNFA(vector<NFATuple>& nfa);

void printNFATuple(NFATuple tup);
void printDFATuple(DFATuple tup);
void printNFA(vector<NFATuple> fa);
void printDFA(vector<DFATuple> fa);

int findBrac(string reg, int i);
vector<NFATuple> evalReg(string reg);
int getDFAIndex(vector<DFATuple>& dfa, int num);
int processInputDFA(vector<DFATuple>& dfa, string query);

int main() {
    ifstream ifs("input.txt");
    ofstream ofs("output.txt");

    int n;
    ifs >> n;
    vector<string> regs(n);
    for (int i = 0; i < n; i++) { ifs >> regs[i]; }

    string query;
    ifs >> query;

    vector<vector<DFATuple>> myDFAs(n);
    for (int i = 0; i < n; i++) {
        vector<NFATuple> ansNFA = evalReg(regs[i]);
        myDFAs[i] = convertNFA(ansNFA);
    }

    string cur = query;
    while (true) {
        int foundReg = -1;
        int foundPos = -1;
        for (int i = 0; i < n; i++) {
            int curRes = processInputDFA(myDFAs[i], cur);
            if (curRes != -1) {
                if (curRes > foundPos) {
                    foundReg = i + 1;
                    foundPos = curRes;
                }
            }
        }
        if (foundPos == -1) {
            ofs << "@" << cur[0];
            if (cur.length() == 1) {
                ofs << "#" << endl;
                return 0;
            }
            cur = cur.substr(1, cur.length() - 1);
        } else {
            ofs << "$" << foundReg;
            if (foundPos + 1 == cur.length()) {
                ofs << "#" << endl;
                return 0;
            }
            cur = cur.substr(foundPos + 1, cur.length() - foundPos - 1);
        }
    }
}

struct NFATuple {
    NFATuple(int present, vector<int> input0, vector<int> input1, vector<int> inputE, int isFinal) {
        this->present = present;
        this->input0 = input0;
        this->input1 = input1;
        this->inputE = inputE;
        this->isFinal = isFinal;
    }

    int present;
    vector<int> input0;
    vector<int> input1;
    vector<int> inputE;
    int isFinal;
};

struct DFATuple {
    DFATuple(int present, int input0, int input1, int isFinal) {
        this->present = present;
        this->input0 = input0;
        this->input1 = input1;
        this->isFinal = isFinal;
    }

    int present;
    int input0;
    int input1;
    int isFinal;
};

vector<NFATuple> makeSingleNFA(int op) {
    // two state nfa
    vector<NFATuple> ans;
    NFATuple temp1 = NFATuple(0, vector<int>(), vector<int>(), vector<int>(), 0);
    NFATuple temp2 = NFATuple(1, vector<int>(), vector<int>(), vector<int>(), 1);
    if (op == 0) {
        temp1.input0.push_back(1);
    } else {
        temp1.input1.push_back(1);
    }
    ans = {temp1, temp2};
    return ans;
}

void incrementNFATuple(NFATuple& tup, int incre) {
    tup.present += incre;
    for (int& i: tup.input0) { i += incre; }
    for (int& i: tup.input1) { i += incre; }
    for (int& i: tup.inputE) { i += incre; }
}

void incrementNFA(vector<NFATuple>& nfa, int incre) {
    for (NFATuple& i: nfa) { incrementNFATuple(i, incre); }
}

vector<NFATuple> makePlusNFA(vector<NFATuple>& nfa) {
    vector<NFATuple> ans;
    incrementNFA(nfa, 1);
    NFATuple temp = NFATuple(0, vector<int>(), vector<int>(), vector<int>(), 0);
    temp.inputE.push_back(1);

    ans.push_back(temp);
    for (auto i: nfa) ans.push_back(i);
    for (int i = 1; i < ans.size(); i++) {
        if (ans[i].isFinal == 1) { ans[i].inputE.push_back(1); }
    }
    return ans;
}

vector<NFATuple> makeStarNFA(vector<NFATuple>& nfa) {
    vector<NFATuple> ans;
    incrementNFA(nfa, 1);
    NFATuple temp = NFATuple(0, vector<int>(), vector<int>(), vector<int>(), 1);
    temp.inputE.push_back(1);

    ans.push_back(temp);
    for (auto i: nfa) ans.push_back(i);
    for (int i = 1; i < ans.size(); i++) {
        if (ans[i].isFinal == 1) { ans[i].inputE.push_back(1); }
    }
    return ans;
}

vector<NFATuple> makeOrNFA(vector<NFATuple>& nfa1, vector<NFATuple>& nfa2) {
    vector<NFATuple> ans;
    incrementNFA(nfa1, 1);
    incrementNFA(nfa2, nfa1.size() + 1);
    NFATuple temp = NFATuple(0, vector<int>(), vector<int>(), vector<int>(), 0);
    temp.inputE.push_back(1);
    temp.inputE.push_back(nfa1.size() + 1);

    ans.push_back(temp);
    for (auto i: nfa1) ans.push_back(i);
    for (auto i: nfa2) ans.push_back(i);

    return ans;
}

vector<NFATuple> concatNFA(vector<NFATuple>& nfa1, vector<NFATuple>& nfa2) {
    vector<NFATuple> ans;
    incrementNFA(nfa2, nfa1.size());
    for (int i = 0; i < nfa1.size(); i++) {
        if (nfa1[i].isFinal == 1) {
            nfa1[i].inputE.push_back(nfa1.size());
            nfa1[i].isFinal = 0;
        }
    }
    for (auto i: nfa1) ans.push_back(i);
    for (auto i: nfa2) ans.push_back(i);
    return ans;
}

void checkAndAdd(vector<int>& arr, int elem) {
    int flag = 1;
    for (auto i: arr) {
        if (i == elem) {
            flag = 0;
            break;
        }
    }
    if (flag == 1) arr.push_back(elem);
    return;
}

bool sameVec(vector<int>& v1, vector<int>& v2) {
    if (v1.size() != v2.size()) return false;
    for (int i = 0; i < v1.size(); i++) {
        if (v1[i] != v2[i]) return false;
    }
    return true;
}

vector<int> getEpsilonSingle(int curpos, vector<NFATuple>& nfa) {
    vector<int> eps;
    eps.push_back(curpos);
    int cur = 0;
    while (cur < eps.size()) {
        NFATuple comp = nfa[eps[cur]];
        for (auto i: comp.inputE) { checkAndAdd(eps, i); }
        cur++;
    }
    sort(eps.begin(), eps.end());
    return eps;
}

vector<int> getEpsilonAll(vector<int>& arr, vector<NFATuple>& nfa) {
    vector<int> eps;
    for (int i = 0; i < arr.size(); i++) {
        vector<int> temp = getEpsilonSingle(arr[i], nfa);
        for (auto elem: temp) { checkAndAdd(eps, elem); }
    }
    sort(eps.begin(), eps.end());
    return eps;
}

vector<int> getInputDFA(int op, vector<int>& arr, vector<NFATuple>& nfa) {
    vector<int> ans;
    for (int i = 0; i < arr.size(); i++) {
        NFATuple comp = nfa[arr[i]];
        if (op == 0) {
            for (auto i: comp.input0) { checkAndAdd(ans, i); }
        } else {
            for (auto i: comp.input1) { checkAndAdd(ans, i); }
        }
    }
    sort(ans.begin(), ans.end());
    return ans;
}

int checkInDFA(vector<vector<int>>& dfaTable, vector<int>& arr) {
    for (int i = 0; i < dfaTable.size(); i++) {
        if (sameVec(dfaTable[i], arr)) return i;
    }
    return -1;
}

bool checkFinalState(vector<int>& arr, vector<NFATuple>& nfa) {
    for (int i = 0; i < arr.size(); i++) {
        if (nfa[arr[i]].isFinal == 1) return true;
    }
    return false;
}

void processAndAdd(
    vector<DFATuple>& dfa, vector<NFATuple>& nfa, vector<vector<int>>& dfaTable, vector<int>& arr, int pos) {
    vector<int> on0 = getInputDFA(0, arr, nfa);
    on0 = getEpsilonAll(on0, nfa);
    vector<int> on1 = getInputDFA(1, arr, nfa);
    on1 = getEpsilonAll(on1, nfa);

    int pres = pos;
    int fin = checkFinalState(arr, nfa);
    int res0;
    int res1;

    int flag0 = 0;
    int flag1 = 0;

    if (on0.size() == 0)
        res0 = -1;
    else {
        int pos = checkInDFA(dfaTable, on0);
        if (pos == -1) {
            flag0 = 1;
            res0 = dfaTable.size();
            dfaTable.push_back(on0);
        } else {
            res0 = pos;
        }
    }

    if (on1.size() == 0)
        res1 = -1;
    else {
        int pos = checkInDFA(dfaTable, on1);
        if (pos == -1) {
            flag1 = 1;
            res1 = dfaTable.size();
            dfaTable.push_back(on1);
        } else {
            res1 = pos;
        }
    }

    DFATuple cur(pres, res0, res1, fin);
    dfa.push_back(cur);
    if (flag0 == 1) processAndAdd(dfa, nfa, dfaTable, on0, res0);
    if (flag1 == 1) processAndAdd(dfa, nfa, dfaTable, on1, res1);
}

vector<DFATuple> convertNFA(vector<NFATuple>& nfa) {
    vector<DFATuple> ans;
    DFATuple temp(-1, -1, -1, 1);  // fail state
    ans.push_back(temp);
    vector<int> epsTemp = {0};
    vector<vector<int>> dfaTable;

    vector<int> eps = getEpsilonAll(epsTemp, nfa);
    dfaTable.push_back(eps);
    processAndAdd(ans, nfa, dfaTable, eps, 0);
    return ans;
}

void printNFATuple(NFATuple tup) {
    cout << tup.present << ' ';
    for (auto i: tup.input0) { cout << i << ','; }
    if (tup.input0.size() == 0) cout << "Empty";
    cout << ' ';
    for (auto i: tup.input1) { cout << i << ','; }
    if (tup.input1.size() == 0) cout << "Empty";
    cout << ' ';
    for (auto i: tup.inputE) { cout << i << ','; }
    if (tup.inputE.size() == 0) cout << "Empty";
    cout << ' ';

    cout << tup.isFinal << endl;
}

void printDFATuple(DFATuple tup) {
    cout << tup.present << ' ' << tup.input0 << ' ' << tup.input1 << ' ' << tup.isFinal << endl;
}

void printNFA(vector<NFATuple> fa) {
    for (auto i: fa) { printNFATuple(i); }
    cout << endl;
}

void printDFA(vector<DFATuple> fa) {
    for (auto i: fa) { printDFATuple(i); }
    cout << endl;
}

int findBrac(string reg, int i) {
    int bracs = 1;
    for (int j = i + 1; j < reg.length(); j++) {
        if (reg[j] == '(')
            bracs++;
        else if (reg[j] == ')')
            bracs--;
        if (bracs == 0) { return j; }
    }
    return -1;
}

vector<NFATuple> evalReg(string reg) {
    vector<vector<NFATuple>> ans;
    for (int i = 0; i < reg.length(); i++) {
        if (reg[i] == 'a') {
            vector<NFATuple> temp = makeSingleNFA(0);
            ans.push_back(temp);
        } else if (reg[i] == 'b') {
            vector<NFATuple> temp = makeSingleNFA(1);
            ans.push_back(temp);
        } else if (reg[i] == '(') {
            int last = findBrac(reg, i);
            if (last + 1 == reg.length()) {
                vector<NFATuple> temp = evalReg(reg.substr(i + 1, last - i - 1));
                ans.push_back(temp);
                i = last;
            } else if (reg[last + 1] == '(' || reg[last + 1] == ')') {
                vector<NFATuple> temp = evalReg(reg.substr(i + 1, last - i - 1));
                ans.push_back(temp);
                i = last;
            } else if (reg[last + 1] == '*') {
                vector<NFATuple> temp = evalReg(reg.substr(i + 1, last - i - 1));
                temp = makeStarNFA(temp);
                ans.push_back(temp);
                i = last + 1;
            } else if (reg[last + 1] == '+') {
                vector<NFATuple> temp = evalReg(reg.substr(i + 1, last - i - 1));
                temp = makePlusNFA(temp);
                ans.push_back(temp);
                i = last + 1;
            } else if (reg[last + 1] == '|') {
                vector<NFATuple> temp1 = evalReg(reg.substr(i + 1, last - i - 1));
                vector<NFATuple> temp2;
                // reg[last + 2] == '(' it must be for reg to be valid
                int lastOr = findBrac(reg, last + 2);
                temp2 = evalReg(reg.substr(last + 3, lastOr - (last + 2) - 1));
                i = lastOr;
                vector<NFATuple> temp = makeOrNFA(temp1, temp2);
                ans.push_back(temp);
            }
        }
    }

    vector<NFATuple> cur = ans[0];
    for (int i = 1; i < ans.size(); i++) { cur = concatNFA(cur, ans[i]); }
    return cur;
}

int getDFAIndex(vector<DFATuple>& dfa, int num) {
    for (int j = 0; j < dfa.size(); j++) {
        if (dfa[j].present == num) { return j; }
    }
    return -1;
}

int processInputDFA(vector<DFATuple>& dfa, string query) {
    // start at dfa[1] that has pres = 0;
    int curIndex = getDFAIndex(dfa, 0);
    int lastAns = -1;
    for (int i = 0; i < query.length(); i++) {
        if (query[i] == 'a') {
            curIndex = getDFAIndex(dfa, dfa[curIndex].input0);
            if (dfa[curIndex].isFinal == 1) {
                if (dfa[curIndex].present == -1) {
                    return lastAns;
                } else
                    lastAns = i;
            }

        } else {
            curIndex = getDFAIndex(dfa, dfa[curIndex].input1);
            if (dfa[curIndex].isFinal == 1) {
                if (dfa[curIndex].present == -1) {
                    return lastAns;
                } else
                    lastAns = i;
            }
        }
    }
    return lastAns;
}
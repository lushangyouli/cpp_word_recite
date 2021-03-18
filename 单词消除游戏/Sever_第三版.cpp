// Sever.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <Winsock2.h>
#pragma once
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */

typedef struct sockaddr_in SOCKADDR_IN;
int SocketInit();
#define MaxSize 1024
void sendToClient(SOCKET sockClient, const char s[MaxSize]);

int SocketInit()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData); //返回0，成功，否则就是错误码

	if (err != 0)
	{
		printf("WinSock DLL版本不足要求n");
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return 0;
	}

	return 1;
}
/**
* [sendToClient description]:向客户端发送数据
* @param sockClient [description]：客户端
* @param s          [description]：消息
*/
void sendToClient(SOCKET sockClient, const char s[MaxSize])
{
	char sendBuf[MaxSize];
	strcpy_s(sendBuf, s);
	int byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
	if (byte <= 0)
		cout << "send error\n";
}


class Game {
private:
public:
	static vector<vector<string>> wordList;//被添加的单词列表
	int level;
	Game(int l = 0) { level = l; }
	~Game() {}
	static vector<vector<string>>& getWordList() { return wordList; }
	static void InitFromFile(SOCKET sockClient);
	int Display(SOCKET sockClient);
	int getLevel() { return level; }
	void setLevel(int a) { level = a; }
};


vector<vector<string>> Game::wordList = {};
int Game::Display(SOCKET sockClient)
{
	char revBuf[MaxSize];
	sendToClient(sockClient, "The level is: ");
	Sleep(20);
	char l[MaxSize];
	_itoa_s(level + 1, l, 10);
	sendToClient(sockClient, l);
	Sleep(20);
	sendToClient(sockClient, "\n");
	Sleep(20);
	if ((unsigned int)level < wordList.size() && wordList[level].size() > 0) {
		srand((unsigned int)time(NULL));
		int pos = rand() % wordList[level].size();
		char w[MaxSize];
		strcpy_s(w, (wordList[level][pos]).c_str());
		send(sockClient, w, strlen(w), 0);
		Sleep(1000);
		char sendBuf[MaxSize];
		strcpy_s(sendBuf, "\r 				 \r");
		send(sockClient, sendBuf, strlen(sendBuf), 0);
		time_t timeBegin;
		timeBegin = time(NULL);
		string answer;
		int n = recv(sockClient, revBuf, MaxSize, 0);
		revBuf[n] = '\0';
		if (n < 0)
			cout << "Rec Error" << endl;
		answer = revBuf;
		if (wordList[level][pos] == answer) {
			time_t timeEnd;
			timeEnd = time(NULL);
			char l[MaxSize];
			_itoa_s((int)(timeEnd - timeBegin), l, 10);
			sendToClient(sockClient, l);
			Sleep(20);
			sendToClient(sockClient, "\n");
			Sleep(20);
			sendToClient(sockClient, "Your answer is right.   \n");
			Sleep(20);
			level++;
			int point;
			if (level > 7 && (timeEnd - timeBegin) < 10)
				point = 3;
			else if (level > 4 && (timeEnd - timeBegin) < 4)
				point = 2;
			else
				point = 1;
			return point;
		}
		else {
			sendToClient(sockClient, "Your answer is wrong.\n");
			Sleep(20);
			return 0;
		}
	}
	else {
		sendToClient(sockClient, "Congratulations! You have reached the last level.\n");
		return 0;
	}
}

void Game::InitFromFile(SOCKET sockClient)
{
	string w;
	ifstream infile("wordList.csv");
	if (!infile) {
		sendToClient(sockClient, "File wordList.csv open failed!\n");
		abort();
	}
	unsigned int i = 0;
	while (!infile.eof())
	{
		infile >> w;
		for (; i < wordList.size(); i++)
			if (w.size() == wordList[i][0].size())
				break;
		if (i < wordList.size())
			wordList[i].push_back(w);
		else {
			vector <string> add;
			add.push_back(w);
			wordList.push_back(add);
		}
	}
	infile.close();
}

class Battle :
	public Game
{
private:
public:
	string word;//题目
	time_t t1;//客户端A挑战用时
	time_t t2;//客户端B挑战用时
	time_t timeBegin;//挑战开始时间
	SOCKET sockClientA;//A挑战者
	SOCKET sockClientB;//B挑战者
	Battle() { t1 = 1000000; t2 = 1000000; }
	~Battle() {}
	void resetTime() { t1 = 1000000; t2 = 1000000; }//重置时间参数
	void givePro();//出题函数
	int onBattle(SOCKET sockClient);//客户端答题
};

void Battle::givePro()
{
	if ((unsigned int)level < wordList.size() && wordList[level].size() > 0) {
		srand((unsigned int)time(NULL));
		int pos = rand() % wordList[level].size();
		word = wordList[level][pos];
		sendToClient(sockClientA, (wordList[level][pos]).c_str());
		sendToClient(sockClientB, (wordList[level][pos]).c_str());
		Sleep(1000);
		sendToClient(sockClientA, "\r                  \r");
		sendToClient(sockClientB, "\r                  \r");
	}
	else {
		sendToClient(sockClientA, "Congratulations! You have reached the last level.\n");
		sendToClient(sockClientB, "Congratulations! You have reached the last level.\n");
	}
	level++;
}
int Battle::onBattle(SOCKET sockClient)
{
	char revBuf[MaxSize];
	string answer;
	int n = recv(sockClient, revBuf, MaxSize, 0);
	revBuf[n] = '\0';
	if (n < 0)
		cout << "Rec Error" << endl;
	answer = revBuf;

	if (word == answer) {
		time_t timeEnd;
		timeEnd = time(NULL);
		if (sockClientA == sockClient)
			t1 = timeEnd - timeBegin;
		else
			t2 = timeEnd - timeBegin;

		if (t1 > t2) {
			sendToClient(sockClientA, "You lose the battle!   \n");
			sendToClient(sockClientB, "You win the battle!   \n");
			return 2;
		}
		else if (t1 < t2) {
			sendToClient(sockClientA, "You win the battle!   \n");
			sendToClient(sockClientB, "You lose the battle!   \n");
			return 1;
		}
		else {
			sendToClient(sockClientA, "It is a draw!   \n");
			sendToClient(sockClientB, "It is a draw!   \n");
			return 0;
		}
	}
	else {
		sendToClient(sockClient, "Your answer is wrong.\n");
		if (sockClientA == sockClient)
			return 2;
		else
			return 1;
	}
}

class User {
private:
	string name;
	string password;
public:
	User(const string& n, const string& p) : name(n), password(p) {}
	~User() {}
	string getName() { return name; }
	string getPassword() { return password; }
	virtual void Register(SOCKET sockClient) {}
	virtual int Login(SOCKET sockClient) { return false; }
};

vector<string> split(const  string& s, const string& delim);

vector<string> split(const  string& s, const string& delim)
{
	vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

class TestBuilder :
	public User
{
private:
	int problemsNumbers;
	int testBuilderLevel;
	static bool isLogin;
	static vector <TestBuilder> testBuilderList;//registered player list
public:
	TestBuilder(const string& n, const string& p, int pNum = 0, int g = 0) : User(n, p)
	{
		problemsNumbers = pNum;
		testBuilderLevel = g;
	}
	~TestBuilder() {}
	static vector<TestBuilder>& getTestBuilderList() { return testBuilderList; }
	static void InitFromFile(SOCKET sockClient);
	void updateInfo(SOCKET sockClient);
	void Register(SOCKET sockClient);
	int Login(SOCKET sockClient);
	void quitLogin() { isLogin = false; }
	void addProblemsNumbers() { problemsNumbers++; }
	void addTestBuilderLevel() { testBuilderLevel += problemsNumbers / 5; }
	int getTestBuilderLevel() { return testBuilderLevel; }
	void SearchTestBuilder(SOCKET sockClient, const string& name);
	void SearchPlayer(SOCKET sockClient, const string& name);
	void SearchTestBuilderBylevel(SOCKET sockClient, int level);
	int getProNum() { return problemsNumbers; }
	void RankPlayersByProNum(SOCKET sockClient);
	int getTesterLevel() { return testBuilderLevel; }
	void RankPlayersByTesterLevel(SOCKET sockClient);
	void AddWord(SOCKET sockClient);
};


class Player :
	public User
{
private:
	int playerLevel;//等级
	int experience;//经验值
	int gainedGrades;//最好成绩
	bool isLogin;
	static vector<Player> playerList;//registered player list
	SOCKET relatedSocket;
public:
	Player(const string& n, const string& p, int e = 0, int l = 0, int g = 0, bool is = false, bool isA = false) : User(n, p)
	{
		playerLevel = l;
		experience = e;
		gainedGrades = g;
		isLogin = is;
		isAccept = isA;
	}
	~Player() {}
	static vector<Player>& getList() { return playerList; }
	static void InitFromFile(SOCKET sockClient);
	void updateInfo(SOCKET sockClient);
	void Register(SOCKET sockClient);
	int Login(SOCKET sockClient);
	int getGrades() { return gainedGrades; }
	int getExperience() { return experience; }
	int getPlayerLevel() { return playerLevel; }
	SOCKET& getRelatedSocket() { return relatedSocket; }
	void quitLogin() { isLogin = false; }
	void addLevel() { playerLevel += experience / 5; }
	void addExperience(int exp) { experience += exp; }
	void setGainedGrades(int n) { gainedGrades = (n > gainedGrades) ? n : gainedGrades; }
	void SearchPlayer(SOCKET sockClient, string& name);
	void SearchPlayerBylevel(SOCKET sockClient, int level);
	void SearchTestBuilder(SOCKET sockClient, string& name);
	void RankPlayersByGrades(SOCKET sockClient);
	void RankPlayersByExperience(SOCKET sockClient);
	vector<Player> getOnlineList();

	bool isAccept;
};


vector<TestBuilder> TestBuilder::testBuilderList = {};
bool TestBuilder::isLogin = 0;
void TestBuilder::Register(SOCKET sockClient)
{
	char sendBuf[MaxSize];

	for (int i = 0; i < (int)testBuilderList.size(); i++)
		if (testBuilderList[i].getName() == this->getName()) {
			strcpy_s(sendBuf, "The Tester-Name has been registered. Register failed!\n");
			send(sockClient, sendBuf, strlen(sendBuf), 0);
			return;
		}

	ofstream outfile("testerList.csv", ios::out | ios::app);
	outfile << this->getName() << "," << this->getPassword() << "," << this->problemsNumbers << "," << this->testBuilderLevel << "     " << endl;
	if (!outfile) {
		strcpy_s(sendBuf, "File playerList.csv open failed!");
		send(sockClient, sendBuf, strlen(sendBuf), 0);
		abort();
	}
	outfile.close();
	testBuilderList.push_back(*this);
	strcpy_s(sendBuf, "Tester successfully registers! \n");
	send(sockClient, sendBuf, strlen(sendBuf), 0);
}
int TestBuilder::Login(SOCKET sockClient)
{
	char sendBuf[MaxSize];
	for (int i = 0; i < (int)testBuilderList.size(); i++)
		if (testBuilderList[i].getName() == this->getName() && testBuilderList[i].getPassword() == this->getPassword()) {
			sendToClient(sockClient, "Tester successfully logins!\n");
			Sleep(100);
			isLogin = true;
			return i;
		}
	strcpy_s(sendBuf, "Tester-login failed!\n");
	Sleep(10);
	send(sockClient, sendBuf, strlen(sendBuf), 0);
	return -1;
}
void TestBuilder::SearchTestBuilder(SOCKET sockClient, const string& name)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			sendToClient(sockClient, "It is the ");
			Sleep(20);
			char l[MaxSize];
			_itoa_s(i + 1, l, 10);
			sendToClient(sockClient, l);
			Sleep(20);
			sendToClient(sockClient, "th tester\n");
			return;
		}
	sendToClient(sockClient, "It is not exited in the testBuilderList.\n");
}
void TestBuilder::SearchPlayer(SOCKET sockClient, const string& name)
{
	vector<Player> v = Player::getList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			sendToClient(sockClient, "It is the ");
			Sleep(20);
			char l[MaxSize];
			_itoa_s(i + 1, l, 10);
			sendToClient(sockClient, l);
			Sleep(20);
			sendToClient(sockClient, "th player\n");
			return;
		}
	sendToClient(sockClient, "It is not exited in the playerList.\n");
}
void TestBuilder::SearchTestBuilderBylevel(SOCKET sockClient, int level)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getTestBuilderLevel() >= level) {
			sendToClient(sockClient, "The ");
			Sleep(20);
			char l[MaxSize];
			_itoa_s(i + 1, l, 10);
			sendToClient(sockClient, l);
			Sleep(20);
			sendToClient(sockClient, "th tester: ");
			Sleep(20);
			sendToClient(sockClient, v[i].getName().c_str());
			Sleep(20);
			sendToClient(sockClient, "\n");
		}
}

bool sortByProNum(TestBuilder a, TestBuilder b)
{
	return a.getProNum() > b.getProNum();
}
void TestBuilder::RankPlayersByProNum(SOCKET sockClient)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	sort(v.begin(), v.end(), sortByProNum);
	for (unsigned int i = 0; i < v.size(); i++) {
		char l[MaxSize];
		_itoa_s(i + 1, l, 10);
		sendToClient(sockClient, l);
		Sleep(20);
		sendToClient(sockClient, "th   ");
		Sleep(20);
		sendToClient(sockClient, v[i].getName().c_str());
		Sleep(20);
		sendToClient(sockClient, " with problemsNumbers is: ");
		Sleep(20);
		_itoa_s(v[i].getProNum(), l, 10);
		sendToClient(sockClient, l);
		Sleep(20);
		sendToClient(sockClient, "\n");
	}
}
bool sortByLevel(TestBuilder a, TestBuilder b)
{
	return a.getTesterLevel() > b.getTesterLevel();
}
void TestBuilder::RankPlayersByTesterLevel(SOCKET sockClient)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	sort(v.begin(), v.end(), sortByLevel);
	for (unsigned int i = 0; i < v.size(); i++) {
		char l[MaxSize];
		_itoa_s(i + 1, l, 10);
		sendToClient(sockClient, l);
		Sleep(20);
		sendToClient(sockClient, "th   ");
		Sleep(20);
		sendToClient(sockClient, v[i].getName().c_str());
		Sleep(20);
		sendToClient(sockClient, " with testBuilderLevel is: ");
		_itoa_s(v[i].getTesterLevel(), l, 10);
		sendToClient(sockClient, l);
		Sleep(20);
		sendToClient(sockClient, "\n");
	}
}
void TestBuilder::AddWord(SOCKET sockClient)
{
	string s;
	char revBuf[MaxSize];
	sendToClient(sockClient, "Please input newly added word:\n");
	int n = recv(sockClient, revBuf, MaxSize, 0);
	if (n < 0)
		cout << "error" << endl;
	revBuf[n] = '\0';
	s = revBuf;
	unsigned int i = 0;
	for (; i < Game::getWordList().size(); i++)
		if (s.size() < Game::getWordList()[i][0].size())
			break;
	if (Game::getWordList()[i - 1][0].size() == s.size()) {
		for (unsigned int m = 0; m < Game::getWordList()[i - 1].size(); m++) {
			if (Game::getWordList()[i - 1][m] == s) {
				sendToClient(sockClient, "The word is already exited! Add word failed!\n");
				return;
			}
		}
		Game::getWordList()[i - 1].push_back(s);
		string n;
		vector<string> s1;
		vector<string> s2;
		ifstream infile("wordList.csv");
		if (!infile) {
			sendToClient(sockClient, "File wordList.csv open failed!\n");
			abort();
		}
		while (!infile.eof())
		{
			infile >> n;
			if (s.size() < n.size())
				s2.push_back(n);
			else
				s1.push_back(n);
		}
		infile.close();
		ofstream outfile("wordList.csv");
		if (!outfile) {
			sendToClient(sockClient, "File wordList.csv open failed!\n");
			abort();
		}
		for (unsigned int i = 0; i < s1.size(); i++)
			outfile << s1[i] << endl;
		outfile << s;
		if (s2.size() > 0)
			outfile << endl;
		unsigned int k = 0;
		for (; k < s2.size() - 1; k++)
			outfile << s2[k] << endl;
		outfile << s2[k];
		infile.close();
	}
	else if (i < Game::getWordList().size()) {
		vector <string> add;
		add.push_back(s);
		Game::getWordList().insert(Game::getWordList().begin() + i, add);
		string n;
		vector<string> s1;
		vector<string> s2;
		ifstream infile("wordList.csv");
		if (!infile) {
			sendToClient(sockClient, "File wordList.csv open failed!\n");
			abort();
		}
		while (!infile.eof())
		{
			infile >> n;
			if (s.size() < n.size())
				s2.push_back(n);
			else
				s1.push_back(n);
		}
		infile.close();
		ofstream outfile("wordList.csv");
		if (!outfile) {
			sendToClient(sockClient, "File wordList.csv open failed!\n");
			abort();
		}
		for (unsigned int i = 0; i < s1.size(); i++)
			outfile << s1[i] << endl;
		outfile << s;
		if (s2.size() > 0)
			outfile << endl;
		unsigned int k = 0;
		for (; k < s2.size() - 1; k++)
			outfile << s2[k] << endl;
		outfile << s2[k];
		infile.close();
	}
	else {
		vector <string> add;
		add.push_back(s);
		Game::getWordList().push_back(add);
		ofstream outfile("wordList.csv", ios::out | ios::app);
		if (!outfile) {
			sendToClient(sockClient, "File wordList.csv open failed!\n");
			abort();
		}
		outfile << endl << s;
		outfile.close();
	}
}
void TestBuilder::InitFromFile(SOCKET sockClient)
{
	string line;
	vector<string> v;
	ifstream infile("testerList.csv");
	if (!infile) {
		sendToClient(sockClient, "File testerList.csv open failed!\n");
		abort();
	}
	getline(infile, line);
	while (!infile.eof())
	{
		getline(infile, line);
		if (line == "")
			break;
		v = split(line, ",");
		TestBuilder tester(v[0], v[1], atoi(v[2].c_str()), atoi(v[3].c_str()));
		testBuilderList.push_back(tester);
	}
	infile.close();
}
void TestBuilder::updateInfo(SOCKET sockClient)
{
	fstream outfile("testerList.csv", ios::out | ios::in);
	if (!outfile) {
		sendToClient(sockClient, "File testerList.csv open failed!\n");
		abort();
	}
	string line;
	vector<string> v;
	outfile.seekp(0);
	getline(outfile, line);
	streamoff cur_tmp = outfile.tellg();
	while (!outfile.eof())
	{
		getline(outfile, line);
		v = split(line, ",");
		if (strcmp(this->getName().c_str(), v[0].c_str()) == 0) {
			outfile.seekp(cur_tmp);
			//outfile.seekp(loc, ios::cur );
			outfile << this->getName() << "," << this->getPassword() << "," << this->problemsNumbers << "," << this->testBuilderLevel;
			break;
		}
		cur_tmp = outfile.tellg();
	}
	outfile.close();
}


vector<Player> Player::playerList = {};
void Player::Register(SOCKET sockClient)
{
	char sendBuf[MaxSize];

	for (int i = 0; i < (int)playerList.size(); i++)
		if (playerList[i].getName() == this->getName()) {
			strcpy_s(sendBuf, "The Player-Name has been registered. Register failed!\n");
			send(sockClient, sendBuf, strlen(sendBuf), 0);
			return;
		}

	ofstream outfile("playerList.csv", ios::out | ios::app);
	if (!outfile) {
		strcpy_s(sendBuf, "File playerList.csv open failed!\n");
		send(sockClient, sendBuf, strlen(sendBuf), 0);
		abort();
	}
	outfile << this->getName() << "," << this->getPassword() << "," << this->playerLevel << "," << this->experience << "," << this->gainedGrades << "      " << endl;
	outfile.close();
	playerList.push_back(*this);
	strcpy_s(sendBuf, "Player successfully registers!\n");
	send(sockClient, sendBuf, strlen(sendBuf), 0);
}
int Player::Login(SOCKET sockClient)
{
	char sendBuf[MaxSize];
	for (int i = 0; i < (int)playerList.size(); i++)
		if (playerList[i].getName() == this->getName() && playerList[i].getPassword() == this->getPassword()) {
			strcpy_s(sendBuf, "Player successfully logins!\n");
			Sleep(20);
			send(sockClient, sendBuf, strlen(sendBuf), 0);
			playerList[i].isLogin = true;
			playerList[i].relatedSocket = sockClient;
			return i;
		}
	strcpy_s(sendBuf, "Player-login failed!\n");
	Sleep(20);
	send(sockClient, sendBuf, strlen(sendBuf), 0);
	return -1;
}
void Player::SearchPlayer(SOCKET sockClient, string& name)
{
	int flag = 1;
	vector<Player> v = Player::getList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			sendToClient(sockClient, "It is the ");
			Sleep(10);
			char l[MaxSize];
			_itoa_s(i + 1, l, 10);
			sendToClient(sockClient, l);
			Sleep(10);
			sendToClient(sockClient, "th player\n");
			flag = 0;
		}
	if(flag)
		sendToClient(sockClient, "It is not exited in the playerList.\n");
}
void Player::SearchPlayerBylevel(SOCKET sockClient, int level)
{
	vector<Player> v = Player::getList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getPlayerLevel() >= level) {
			sendToClient(sockClient, "The ");
			Sleep(10);
			char l[MaxSize];
			_itoa_s(i + 1, l, 10);
			sendToClient(sockClient, l);
			Sleep(10);
			sendToClient(sockClient, "th player: ");
			Sleep(10);
			sendToClient(sockClient, v[i].getName().c_str());
			Sleep(10);
			sendToClient(sockClient, "\n");
		}
}
void Player::SearchTestBuilder(SOCKET sockClient, string& name)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			sendToClient(sockClient, "It is the ");
			Sleep(10);
			char l[MaxSize];
			_itoa_s(i + 1, l, 10);
			sendToClient(sockClient, l);
			Sleep(10);
			sendToClient(sockClient, "th tester\n");
			return;
		}
	sendToClient(sockClient, "It is not exited in the testBuilderList.\n");
}
bool sortByGrades(Player a, Player b)
{
	return a.getGrades() > b.getGrades();
}
void Player::RankPlayersByGrades(SOCKET sockClient)
{
	vector<Player> v = Player::getList();
	sort(v.begin(), v.end(), sortByGrades);
	for (unsigned int i = 0; i < v.size(); i++) {
		char l[MaxSize];
		_itoa_s(i + 1, l, 10);
		sendToClient(sockClient, l);
		Sleep(10);
		sendToClient(sockClient, "th   ");
		Sleep(10);
		sendToClient(sockClient, v[i].getName().c_str());
		Sleep(10);
		sendToClient(sockClient, " with gainedGrades is: ");
		Sleep(10);
		_itoa_s(v[i].getGrades(), l, 10);
		sendToClient(sockClient, l);
		Sleep(10);
		sendToClient(sockClient, "\n");
	}
}
bool sortByExperience(Player a, Player b)
{
	return a.getExperience() > b.getExperience();
}
void Player::RankPlayersByExperience(SOCKET sockClient)
{
	vector<Player> v = Player::getList();
	sort(v.begin(), v.end(), sortByGrades);
	for (unsigned int i = 0; i < v.size(); i++) {
		char l[MaxSize];
		_itoa_s(i + 1, l, 10);
		sendToClient(sockClient, l);
		Sleep(10);
		sendToClient(sockClient, "th   ");
		Sleep(10);
		sendToClient(sockClient, v[i].getName().c_str());
		Sleep(10);
		sendToClient(sockClient, " with experience is: ");
		Sleep(10);
		_itoa_s(v[i].getGrades(), l, 10);
		sendToClient(sockClient, l);
		Sleep(10);
		sendToClient(sockClient, "\n");
	}
}
void Player::InitFromFile(SOCKET sockClient)
{
	string line;
	vector<string> v;
	ifstream infile("playerList.csv");
	if (!infile) {
		sendToClient(sockClient, "File playerList.csv open failed!\n");
		abort();
	}
	getline(infile, line);
	while (!infile.eof())
	{
		getline(infile, line);
		if (line == "")
			break;
		v = split(line, ",");
		Player player(v[0], v[1], atoi(v[2].c_str()), atoi(v[3].c_str()), atoi(v[4].c_str()));
		playerList.push_back(player);
	}
	infile.close();
}

void Player::updateInfo(SOCKET sockClient)
{
	fstream outfile("playerList.csv", ios::out | ios::in);
	if (!outfile) {
		sendToClient(sockClient, "File playerList.csv open failed!\n");
		abort();
	}
	string line;
	vector<string> v;
	outfile.seekp(0);
	getline(outfile, line);
	streamoff cur_tmp = outfile.tellg();
	while (!outfile.eof())
	{
		getline(outfile, line);
		v = split(line, ",");
		if (strcmp(this->getName().c_str(), v[0].c_str()) == 0) {
			outfile.seekp(cur_tmp);
			//outfile.seekp( 0, ios::cur );
			outfile << this->getName() << "," << this->getPassword() << "," << this->playerLevel << "," << this->experience << "," << this->gainedGrades;
			outfile.seekp(cur_tmp);
			getline(outfile, line);
			break;
		}
		cur_tmp = outfile.tellg();
	}
	outfile.close();
}
vector<Player> Player::getOnlineList()
{
	vector<Player> onlintList;
	for (unsigned int i = 0; i < playerList.size(); i++) {
		if (playerList[i].isLogin)
			onlintList.push_back(playerList[i]);
	}
	return onlintList;
}

Battle battle;

HANDLE hMutex;
void SendAndRec(SOCKET sockClient)
{
	int choice = 1;

	char sendBuf[MaxSize];
	char revBuf[MaxSize];
	int byte = 0;
	while (1)
	{
		WaitForSingleObject(hMutex, INFINITE);
		while (choice != 0) {
			strcpy_s(sendBuf, "             Welcome to word elemination system !\n                        1  register\n                        2  player_login\n                        3  tester_login\n                        0  quit\n");
			byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
			if (byte <= 0)
				cout << "Send error" << endl;
			do {
				int n = recv(sockClient, revBuf, MaxSize, 0);
				if (n < 0)
					cout << "Recv error" << endl;
				choice = revBuf[0] - '0';
				if (choice < 0 || choice > 3) {
					strcpy_s(sendBuf, "Input is wrong, please input again!\n");
					byte = send(sockClient, sendBuf, strlen(sendBuf), 0); //服务器向客户端发送数据
				}
				else
					break;
			} while (1);
			switch (choice) {
			case 1: {
				strcpy_s(sendBuf, "Please set your username: ");
				byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
				string name;
				int n = recv(sockClient, revBuf, MaxSize, 0);
				revBuf[n] = '\0';
				while (n < 0)
					cout << "Rec Error" << endl;
				name = revBuf;

				strcpy_s(sendBuf, "Please set your account password: ");
				byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
				string password;
				n = recv(sockClient, revBuf, MaxSize, 0);
				while (n < 0)
					cout << "Revc error" << endl;
				password = revBuf;
				if (name[0] == 't') {
					TestBuilder tester(name, password);
					tester.Register(sockClient);
				}
				else {
					Player player(name, password);
					player.Register(sockClient);
				}
			}
					break;
					/**
					* player operation page
					*/
			case 2: {
				string name;
				strcpy_s(sendBuf, "Please input username:");
				byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
				int n = recv(sockClient, revBuf, MaxSize, 0);
				revBuf[n] = '\0';
				while (n < 0)
					cout << "Rec Error" << endl;
				name = revBuf;

				string password;
				strcpy_s(sendBuf, "Please input password:");
				byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
				n = recv(sockClient, revBuf, MaxSize, 0);
				revBuf[n] = '\0';
				while (n < 0)
					cout << "Rec Error" << endl;
				password = revBuf;

				Player player(name, password);
				int pos = player.Login(sockClient);
				if (pos != -2) {
					while (pos == -1) {
						string name;
						strcpy_s(sendBuf, "Username or password is wrong,please input them again:\n Please input username:");
						byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
						int n = recv(sockClient, revBuf, MaxSize, 0);
						revBuf[n] = '\0';
						if (n < 0)
							cout << "Rec Error" << endl;
						name = revBuf;

						string password;
						strcpy_s(sendBuf, "Please input password:");
						byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
						n = recv(sockClient, revBuf, MaxSize, 0);
						revBuf[n] = '\0';
						if (n < 0)
							cout << "Rec Error" << endl;
						password = revBuf;

						Player player(name, password);
						pos = player.Login(sockClient);
					}
					Player& tempPlayer = (Player&)Player::getList()[pos];
					int playerChoice = 1;
					Game game;
					while (playerChoice != 0) {
						sendToClient(sockClient, "\n              ******  Welcome  to  playing  page  ****** ");
						Sleep(20);
						sendToClient(sockClient, "\n                  1  select the difficulty of the game");
						Sleep(20);
						sendToClient(sockClient, "\n                  2  start the game");
						Sleep(20);
						sendToClient(sockClient, "\n                  3  next level ");
						Sleep(20);
						sendToClient(sockClient, "\n                  4  search user ");
						Sleep(20);
						sendToClient(sockClient, "\n                  5  rank playerList ");
						Sleep(20);
						sendToClient(sockClient, "\n                  6  find all the players online ");
						Sleep(20);
						sendToClient(sockClient, "\n                  7  accept battle ");
						Sleep(20);
						sendToClient(sockClient, "\n                  8  refuse battle ");
						Sleep(20);
						sendToClient(sockClient, "\n                  0  quit playing  ");
						Sleep(20);
						sendToClient(sockClient, "\n              ******************************************\n");
						do {
							int n = recv(sockClient, revBuf, MaxSize, 0);
							if (n < 0)
								cout << "Recv error" << endl;
							playerChoice = revBuf[0] - '0';

							if (playerChoice < 0 || playerChoice > 8) {
								strcpy_s(sendBuf, "Input is wrong, please input again!");
								byte = send(sockClient, sendBuf, strlen(sendBuf), 0);
							}
							else
								break;
						} while (1);
						switch (playerChoice) {
						case 1: {
							int l = 0;
							sendToClient(sockClient, "Please choose the difficulty (0-10):");
							int n = recv(sockClient, revBuf, MaxSize, 0);
							if (n < 0)
								cout << "Recv error" << endl;
							l = revBuf[0] - '0';
							game.setLevel(l);
						}
								break;
						case 2: {
							int b = game.Display(sockClient);
							if (b > 0) {
								tempPlayer.setGainedGrades(game.getLevel());
								tempPlayer.addExperience(b);
								tempPlayer.addLevel();
							}
						}
								break;
						case 3: {
							int b = game.Display(sockClient);
							if (b > 0) {
								tempPlayer.setGainedGrades(game.getLevel());
								tempPlayer.addExperience(b);
								tempPlayer.addLevel();
							}
						}
								break;
								/**
								* 查询用户
								*/
						case 4: {
							sendToClient(sockClient, "Please choose the way to search user:\n");
							Sleep(10);
							sendToClient(sockClient, "              a username  \n              b userLevel    \n");
							char rankChoice;
							int n = recv(sockClient, revBuf, MaxSize, 0);
							if (n < 0)
								cout << "error" << endl;
							rankChoice = revBuf[0];
							if (rankChoice == 'a') {
								sendToClient(sockClient, "Please input the username:");
								string name;
								n = recv(sockClient, revBuf, MaxSize, 0);
								if (n < 0)
									cout << "Rec Error" << endl;
								revBuf[n] = '\0';
								name = revBuf;
								tempPlayer.SearchPlayer(sockClient, name);
								tempPlayer.SearchTestBuilder(sockClient, name);
							}
							else {
								sendToClient(sockClient, "Please input the Player's level:");
								int level;
								n = recv(sockClient, revBuf, MaxSize, 0);
								if (n < 0)
									cout << "Rec Error" << endl;
								revBuf[n] = '\0';
								level = atoi(revBuf);
								tempPlayer.SearchPlayerBylevel(sockClient, level);
							}

						}
								break;
						case 5: {
							sendToClient(sockClient, "\n      Please choose the characters to rank the Player list:\n");
							sendToClient(sockClient, "\n             a gainedGrades  \n              b experience    \n");
							char rankChoice;
							int n = recv(sockClient, revBuf, MaxSize, 0);
							if (n < 0)
								cout << "error" << endl;
							rankChoice = revBuf[0];
							if (rankChoice == 'a')
								tempPlayer.RankPlayersByGrades(sockClient);
							else
								tempPlayer.RankPlayersByExperience(sockClient);
						}
								break;
								/**
								* 查询当前时间所有在线用户并选择是否进入对战界面
								*/
						case 6: {
							vector<Player> onList;
							onList = tempPlayer.getOnlineList();
							sendToClient(sockClient, "The onLine player list is:\n");
							Sleep(20);
							for (unsigned int i = 0; i < onList.size(); i++) {
								sendToClient(sockClient, onList[i].getName().c_str());
								Sleep(20);
								sendToClient(sockClient, "\n");
								Sleep(20);
							}
							sendToClient(sockClient, "If you want to battle with someone online,please input 'y';If don't,input 'n':\n");
							n = recv(sockClient, revBuf, MaxSize, 0);
							revBuf[n] = '\0';
							if (n < 0)
								cout << "Rec Error" << endl;
							if (revBuf[0] == 'y') {
								sendToClient(sockClient, "Please input its username:\n");
								int n = recv(sockClient, revBuf, MaxSize, 0);
								revBuf[n] = '\0';
								if (n < 0)
									cout << "Rec Error" << endl;
								string username = revBuf;
								unsigned int i = 0;
								for (; i < onList.size(); i++) {
									if (username == onList[i].getName())
										break;
								}
								battle.sockClientA = sockClient;
								battle.sockClientB = onList[i].getRelatedSocket();
								sendToClient(onList[i].getRelatedSocket(), tempPlayer.getName().c_str());
								Sleep(20);
								sendToClient(onList[i].getRelatedSocket(), " want to battle with you,do you agree(Make a reply in 8 seconds,'7'or'8'):\n");
								Sleep(8000);
								unsigned int k = 0;
								for (; k < Player::getList().size(); k++) {
									if (Player::getList()[k].getName() == onList[i].getName()) {
										onList[i].isAccept = Player::getList()[k].isAccept;
										break;
									}
								}
								if (onList[i].isAccept) {
									Player& opponent = (Player&)Player::getList()[k];
									opponent.isAccept = false;
									battle.resetTime();
									sendToClient(onList[i].getRelatedSocket(), "The battle will start after 3 seconds!\n");
									sendToClient(sockClient, "The opponent has agreed,the battle will start after 3 seconds!\n");
									Sleep(3000);
									battle.givePro();
									battle.timeBegin = time(NULL);
									int a = battle.onBattle(sockClient);
									if (a == 1)
										tempPlayer.addExperience(10);
									else if (a == 2)
										tempPlayer.addExperience(-2);
									else
										tempPlayer.addExperience(0);
									tempPlayer.addLevel();
								}
								else {
									sendToClient(sockClient, "The oponent refused your request!\n");
								}
							}
						}
								break;
								/**
								* 被挑战者应战界面
								*/
						case 7: {
							tempPlayer.isAccept = true;
							Sleep(3000);
							int a = battle.onBattle(sockClient);
							if (a == 2)
								tempPlayer.addExperience(10);
							else if (a == 1)
								tempPlayer.addExperience(-1);
							else
								tempPlayer.addExperience(0);
							tempPlayer.addLevel();
						}
								break;
						}
					}
					tempPlayer.updateInfo(sockClient);
					tempPlayer.quitLogin();
				}
				else
					sendToClient(sockClient, "There is others online. Please wait for a while.");
			}
					break;
					/**
					* tester operation page
					*/
			case 3: {
				sendToClient(sockClient, "Please input username:");
				string name;
				int n = recv(sockClient, revBuf, MaxSize, 0);
				revBuf[n] = '\0';
				if (n < 0)
					cout << "Rec Error" << endl;
				name = revBuf;
				string password;
				sendToClient(sockClient, "Please input password:");
				n = recv(sockClient, revBuf, MaxSize, 0);
				revBuf[n] = '\0';
				if (n < 0)
					cout << "Rec Error" << endl;
				password = revBuf;
				TestBuilder tester(name, password);
				int pos = tester.Login(sockClient);
				if (pos != -2) {
					while (pos == -1) {
						sendToClient(sockClient, "Username or password is wrong,please input them again:\n");
						sendToClient(sockClient, "Please input username again:");
						string name;
						int n = recv(sockClient, revBuf, MaxSize, 0);
						revBuf[n] = '\0';
						if (n < 0)
							cout << "Rec Error" << endl;
						name = revBuf;
						sendToClient(sockClient, "Please input password again:");
						string password;
						n = recv(sockClient, revBuf, MaxSize, 0);
						revBuf[n] = '\0';
						if (n < 0)
							cout << "Rec Error" << endl;
						password = revBuf;
						TestBuilder tester(name, password);
						pos = tester.Login(sockClient);
					}
					TestBuilder& tempTester = (TestBuilder&)TestBuilder::getTestBuilderList()[pos];
					tempTester.addTestBuilderLevel();
					int testerChoice = 1;
					while (testerChoice != 0) {
						sendToClient(sockClient, "\n\n            ********************************");
						Sleep(20);
						sendToClient(sockClient, "\n                Welcome to management page ! ");
						Sleep(20);
						sendToClient(sockClient, "\n                 1  build new tester case");
						Sleep(20);
						sendToClient(sockClient, "\n                 2  search user");
						Sleep(20);
						sendToClient(sockClient, "\n                 3  rank testBuilderList ");
						Sleep(20);
						sendToClient(sockClient, "\n                 0  quit management page ");
						Sleep(20);
						sendToClient(sockClient, "\n             ********************************\n");
						do {
							int n = recv(sockClient, revBuf, MaxSize, 0);
							if (n < 0)
								cout << "Revc error" << endl;
							testerChoice = revBuf[0] - '0';
							if (testerChoice < 0 || testerChoice > 3)
								sendToClient(sockClient, "Input is wrong，please input again!");
							else
								break;
						} while (1);
						switch (testerChoice) {
							/**
							* 新增单词
							*/
						case 1:
							tempTester.AddWord(sockClient);
							tempTester.addProblemsNumbers();
							break;
							/**
							* 查询
							*/
						case 2: {
							sendToClient(sockClient, "Please choose the way to search user:\n");
							Sleep(20);
							sendToClient(sockClient, "              a username  \n              b userLevel    \n");
							char rankChoice;
							int n = recv(sockClient, revBuf, MaxSize, 0);
							if (n < 0)
								cout << "error" << endl;
							rankChoice = revBuf[0];
							if (rankChoice == 'a') {
								sendToClient(sockClient, "Please input the username:");
								string name;
								n = recv(sockClient, revBuf, MaxSize, 0);
								if (n < 0)
									cout << "Rec Error" << endl;
								revBuf[n] = '\0';
								name = revBuf;
								tempTester.SearchPlayer(sockClient, name);
								tempTester.SearchTestBuilder(sockClient, name);
							}
							else {
								sendToClient(sockClient, "Please input the tester's level:");
								int level;
								n = recv(sockClient, revBuf, MaxSize, 0);
								if (n < 0)
									cout << "Rec Error" << endl;
								revBuf[n] = '\0';
								level = atoi(revBuf);
								tempTester.SearchTestBuilderBylevel(sockClient, level);
							}
						}
								break;
								/**
								* 排行榜
								*/
						case 3: {
							sendToClient(sockClient, "\n      Please choose the characters to rank the Tester list:\n");
							sendToClient(sockClient, "             a problemsNumbers  \n             b testBuilderLevel    \n");
							char rankChoice;
							int n = recv(sockClient, revBuf, MaxSize, 0);
							revBuf[n] = '\0';
							if (n < 0)
								cout << "Rec Error" << endl;
							rankChoice = revBuf[0];
							if (rankChoice == 'a')
								tempTester.RankPlayersByProNum(sockClient);
							else
								tempTester.RankPlayersByTesterLevel(sockClient);
						}
								break;
						}
					}
					tempTester.updateInfo(sockClient);
					tempTester.quitLogin();
				}
				else
					sendToClient(sockClient, "There is others online. Please wait for a while.");
			}
					break;
			}
		}
		if (choice == 0) {
			sendToClient(sockClient, "Goodbye, welcome to come again!\n");
			getchar();
		}
	}
	closesocket(sockClient);
}

int main()
{
	SOCKADDR_IN addrServer;
	int sockServer;

	if (SOCKET_ERROR == SocketInit())
		return -1;

	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    //htol将主机字节序long型转换为网络字节序
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6666);                      //htos用来将端口转换成字符，1024以上的数字即可

	sockServer = socket(AF_INET, SOCK_STREAM, 0);           //面向连接的可靠性服务SOCK_STRAM
	bind(sockServer, (SOCKADDR*)&addrServer, sizeof(addrServer)); //将socket绑定到相应地址和端口上
	listen(sockServer, 5);                                  //等待队列中的最大长度为5

	printf("Welcome,the Host %s is running!Now Wating for someone comes in!\n", inet_ntoa(addrServer.sin_addr));

	int len = sizeof(SOCKADDR_IN);

	SOCKADDR_IN addrClient;

	int key = 0;

	while (1)
	{
		SOCKET sockClient = accept(sockServer, (SOCKADDR*)&addrClient, &len); //阻塞调用进程直至新的连接出现

		if (sockClient == INVALID_SOCKET)
		{
			printf("Accept Failed!\n");
			continue; //继续监听
		}

		cout << "New Client Joins In.\n";
		if (key == 0) {
			Game::InitFromFile(sockClient);
			Player::InitFromFile(sockClient);
			TestBuilder::InitFromFile(sockClient);
			key = 1;
		}

		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendAndRec, (LPVOID)sockClient, 0, 0);

		if (hThread != NULL)
			CloseHandle(hThread);
		Sleep(100);

	}

	getchar();
	return 0;
}

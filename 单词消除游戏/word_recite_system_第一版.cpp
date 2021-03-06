// word_recite_system.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include<Windows.h>
using namespace std;

void Interface_Jumping() {
	cout << "The interface will jump in 3 seconds!!!";
	Sleep(3000);
	system("CLS");
	cout << "Jumping.";
	Sleep(500);
	cout << ".";
	Sleep(500);
	cout << ".";
	Sleep(500);
	system("CLS");

}


class Game {
private:
	int level;
	static vector<string> wordList;//被添加的单词列表
public:
	Game(int l = 0) { level = l; }
	~Game() {}
	bool Display(); //当答案输入错误或者通关时会返回0
	void setLevel(int a) { level = a; }
	static vector<string>& getWordList() { return wordList; }
	static void InitFromFile();
};

vector<string> Game::wordList = {};
bool Game::Display()
{
	cout << "This the " << level + 1 << "th level : "
		<< "There are " << wordList.size() << " levels totally" << endl;
	if ((unsigned)level < wordList.size()) {
		cout << wordList[level];
		Sleep(2000);
		cout << "\r                   \r";
		string answer;
		cin >> answer;
		if (wordList[level] == answer) {
			cout << "Your answer is right." << endl << endl;
			level++;
			return true;
		}
		else {
			cout << "Your answer is wrong." << endl << endl;
			return false;
		}
	}
	else {
		cout << "Congratulations! You have reached the last level." << endl << endl;
		return false;
	}
}
void Game::InitFromFile()
{
	string w;
	ifstream infile("wordList.csv");
	if (!infile) {
		cout << "File wordList.csv open failed!" << endl;
		abort();
	}
	while (!infile.eof())
	{
		infile >> w;
		wordList.push_back(w);
	}
	infile.close();
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
	virtual void Register() {}
	virtual int Login() { return false; }
};

class TestBuilder :
	public User
{
private:
	int problemsNumbers;
	int testBuilderLevel;
	static bool isLogin;
	static vector<TestBuilder> testBuilderList;//registered TestBuilder list
public:
	TestBuilder(const string& n, const string& p, int pNum = 0, int g = 0) : User(n, p)
	{
		problemsNumbers = pNum;
		testBuilderLevel = g;
	}
	~TestBuilder() {}
	static vector<TestBuilder>& getTestBuilderList() { return testBuilderList; }
	void Register();
	int Login();
	void quitLogin() { isLogin = false; }
	void addProblemsNumbers() { problemsNumbers++; }
	void addTestBuilderLevel() { testBuilderLevel++; }
	void SearchTestBuilder(const string& name);
	void SearchPlayer(const string& name);
	void SearchTesterBylevel(int level);
	int getProNum() { return problemsNumbers; }
	void RankPlayersByProNum();
	int getTesterLevel() { return testBuilderLevel; }
	void RankPlayersByTesterLevel();
	void AddWord();
	static void InitFromFile();
	void updateInfo();
};


class Player : public User {
private:
	int playerLevel;//等级
	int experience;//经验值
	int gainedGrades;//最好成绩
	static bool isLogin;
	static vector<Player> playerList;//registered player list
public:
	Player(const string& n, const string& p, int e = 0, int l = 0, int g = 0) : User(n, p)
	{
		playerLevel = l;
		experience = e;
		gainedGrades = g;
	}
	~Player() {}
	static vector<Player>& getList() { return playerList; }
	void Register();
	int Login();//返回值为-2时表明已有账号登录，返回值为-1时表明登录的用户名或者密码错误，为其他值时登录成功并且返回用户名当前的序号
	int getGrades() { return gainedGrades; }
	int getExperience() { return experience; }
	int getPlayerLevel() { return playerLevel; }
	void quitLogin() { isLogin = false; }
	void addLevel() { playerLevel++; }
	void addExperience() { experience++; }
	void addGainedGrades() { gainedGrades++; }
	void SearchPlayer(string& name);
	void SearchTestBuilder(string& name);
	void SearchPlayerBylevel(int level);
	void RankPlayersByGrades();
	void RankPlayersByExperience();
	void RankPlayersByBestLevel();
	void updateInfo();
	static void InitFromFile();
};


vector<string> split(const  string& s, const string& delim);


vector<TestBuilder> TestBuilder::testBuilderList = {};
bool TestBuilder::isLogin = 0;
void TestBuilder::Register()
{
	for (int i = 0; i < (int)testBuilderList.size(); i++)
		if (testBuilderList[i].getName() == this->getName()) {
			cout << "The Tester-Name has been registered. Register failed!" << endl;
			return;
		}

	ofstream outfile("testerList.csv", ios::out | ios::app);
	if (!outfile) {
		cout << "File testerList.csv open failed!" << endl;
		abort();
	}
	outfile << this->getName() << "," << this->getPassword() << "," << this->problemsNumbers << "," << this->testBuilderLevel << "    " << endl;
	outfile.close();
	testBuilderList.push_back(*this);
	cout << "Tester successfully registers!" << endl;
}
int TestBuilder::Login()
{
	cout << endl;
	if (isLogin)
		return -2;
	for (int i = 0; i < (int)testBuilderList.size(); i++)
		if (testBuilderList[i].getName() == this->getName() && testBuilderList[i].getPassword() == this->getPassword()) {
			cout << "Tester successfully logins!" << endl << endl;
			isLogin = true;
			return i;
		}
	cout << "Tester-login failed!" << endl << endl;
	return -1;
}
void TestBuilder::SearchTestBuilder(const string& name)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			cout << "It is the " << i + 1 << "th tester" << endl;
			return;
		}
	cout << "It is not exited in the testBuilderList." << endl;
}
void TestBuilder::SearchPlayer(const string& name)
{
	vector<Player> v = Player::getList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			cout << "It is the " << i + 1 << "th player" << endl;
			return;
		}
	cout << "It is not exited in the playerList." << endl;
}
void TestBuilder::SearchTesterBylevel(int level)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getTesterLevel() >= level) {
			cout << "The " << i + 1 << "th tester: " << v[i].getName() << endl;
		}
}
bool sortByProNum(TestBuilder a, TestBuilder b)
{
	return a.getProNum() > b.getProNum();
}
void TestBuilder::RankPlayersByProNum()
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	sort(v.begin(), v.end(), sortByProNum);//快排，利用cmp参数即sortByProNum进行排序
	for (unsigned int i = 0; i < v.size(); i++)
		cout << i + 1 << "th   " << v[i].getName()
		<< " with problemsNumbers is: " << v[i].getProNum() << endl;
}
bool sortByLevel(TestBuilder a, TestBuilder b)
{
	return a.getTesterLevel() > b.getTesterLevel();
}
void TestBuilder::RankPlayersByTesterLevel()
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	sort(v.begin(), v.end(), sortByLevel);//快排，利用cmp参数即sortByLevel进行排序
	for (unsigned int i = 0; i < v.size(); i++)
		cout << i + 1 << "th   " << v[i].getName()
		<< " with testBuilderLevel is: " << v[i].getTesterLevel() << endl;
}
void TestBuilder::AddWord()
{
	string s;
	cout << "Please input newly added word: ";
	cin >> s;
	Game::getWordList().push_back(s);
	ofstream outfile("wordList.csv", ios::out | ios::app);
	if (!outfile) {
		cout << "File wordList.csv open failed!" << endl;
		abort();
	}
	outfile << endl << s;
	outfile.close();
}
void TestBuilder::InitFromFile()
{
	string line;
	vector<string> v;
	ifstream infile("testerList.csv");
	if (!infile) {
		cout << "File testerList.csv open failed!" << endl;
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
void TestBuilder::updateInfo()//更新出题者的出题数和等级
{
	fstream outfile("testerList.csv", ios::out | ios::in);
	if (!outfile) {
		cout << "File testerList.csv open failed!" << endl;
		abort();
	}
	string line;
	vector<string> v;
	outfile.seekp(0);
	getline(outfile, line);
	streamoff cur_tmp = outfile.tellg();//返回文件指针当前位置
	while (!outfile.eof())
	{
		getline(outfile, line);
		v = split(line, ",");
		if (strcmp(this->getName().c_str(), v[0].c_str()) == 0) {
			outfile.seekp(cur_tmp);
			outfile << this->getName() << "," << this->getPassword() << "," << this->problemsNumbers << "," << this->testBuilderLevel;
			break;
		}
		cur_tmp = outfile.tellg();
	}
	outfile.close();
}


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



bool Player::isLogin = 0;
vector<Player> Player::playerList = {};
void Player::Register()
{
	for (int i = 0; i < (int)playerList.size(); i++)
		if (playerList[i].getName() == this->getName()) {
			cout << "The Player-Name has been registered. Register failed!" << endl;
			return;
		}

	ofstream outfile("playerList.csv", ios::out | ios::app);
	if (!outfile) {
		cout << "File playerList.csv open failed!" << endl;
		abort();
	}
	outfile << this->getName() << "," << this->getPassword() << "," << this->playerLevel << "," << this->experience << "," << this->gainedGrades << "    " << endl;
	outfile.close();
	playerList.push_back(*this);
	cout << "Player successfully registers!" << endl;
}
int Player::Login()
{
	cout << endl;
	if (isLogin)
		return -2;
	for (int i = 0; i < (int)playerList.size(); i++)
		if (playerList[i].getName() == this->getName() && playerList[i].getPassword() == this->getPassword()) {
			cout << "Player successfully logins!" << endl << endl;
			isLogin = true;
			return i;
		}
	cout << "Player-login failed!" << endl << endl;
	return -1;
}
void Player::SearchPlayer(string& name)
{
	vector<Player> v = Player::getList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			cout << "It is the " << i + 1 << "th player" << endl;
			return;
		}
	cout << "It is not exited in the playerList." << endl;
}
void Player::SearchTestBuilder(string& name)
{
	vector<TestBuilder> v = TestBuilder::getTestBuilderList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getName() == name) {
			cout << "It is the " << i + 1 << "th tester" << endl;
			return;
		}
	cout << "It is not exited in the testBuilderList." << endl;
}
void Player::SearchPlayerBylevel(int level)
{
	vector<Player> v = Player::getList();
	for (unsigned int i = 0; i < v.size(); i++)
		if (v[i].getPlayerLevel() >= level) {
			cout << "The " << i + 1 << "th player: " << v[i].getName() << endl;
		}
}
bool sortByGrades(Player a, Player b)
{
	return a.getGrades() > b.getGrades();
}
void Player::RankPlayersByGrades()
{
	vector<Player> v = Player::getList();
	sort(v.begin(), v.end(), sortByGrades);
	for (unsigned int i = 0; i < v.size(); i++)
		cout << i + 1 << "th   " << v[i].getName()
		<< " with gainedGrades is: " << v[i].getGrades() << endl;
}
bool sortByExperience(Player a, Player b)
{
	return a.getExperience() > b.getExperience();
}
void Player::RankPlayersByExperience()
{
	vector<Player> v = Player::getList();
	sort(v.begin(), v.end(), sortByGrades);
	for (unsigned int i = 0; i < v.size(); i++)
		cout << i + 1 << "th   " << v[i].getName()
		<< " with experience is: " << v[i].getExperience() << endl;
}
bool sortByBestLevel(Player a, Player b)
{
	return a.getPlayerLevel() > b.getPlayerLevel();
}
void Player::RankPlayersByBestLevel()
{
	vector<Player> v = Player::getList();
	sort(v.begin(), v.end(), sortByBestLevel);
	for (unsigned int i = 0; i < v.size(); i++)
		cout << i + 1 << "th   " << v[i].getName()
		<< " with best level is: " << v[i].getPlayerLevel() << endl;
}
void Player::InitFromFile()
{
	string line;
	vector<string> v;
	ifstream infile("playerList.csv");
	if (!infile) {
		cout << "File playerList.csv open failed!" << endl;
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

void Player::updateInfo()
{
	fstream outfile("playerList.csv", ios::out | ios::in);
	if (!outfile) {
		cout << "File playerList.csv open failed!" << endl;
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
			outfile << this->getName() << "," << this->getPassword() << "," << this->playerLevel << "," << this->experience << "," << this->gainedGrades;
			/*outfile.seekp(cur_tmp);
			getline(outfile, line);*/
			break;
		}
		cur_tmp = outfile.tellg();
	}
	outfile.close();
}



int main()
{
	int choice = 1;
	Game::InitFromFile();
	Player::InitFromFile();
	TestBuilder::InitFromFile();
	while (choice != 0) {
		cout << "\n\n\n" << "                    **********************************          " << endl;
		cout << "                       Welcome to word elemination system ! " << endl;
		cout << "\n                                 1  user-register(Note:TesterBuilder's name starts as't_')" << endl;//-用户注册
		cout << "\n                                 2  player-login" << endl;//-玩家登陆
		cout << "\n                                 3  tester-login" << endl;//-出题者登陆
		cout << "\n                                 0  quit" << endl;//-退出系统
		cout << "                    ******************************************         " << endl;
		do {
			cin >> choice;
			if (choice < 0 || choice > 3)
				cout << "Input is wrong，please input again!" << endl;
			else
				break;
		} while (1);
		switch (choice) {
		case 1: {
			cout << "Please set your username:";
			string name;
			cin >> name;
			if (name == "Q") {
				cout << "\nExiting\n\n";
				Interface_Jumping();
				break;
			}
			cout << "Please set your account password:";
			string password;
			char test = 'a';
			int i = 0;
			while ((test = _getch()) != '\r') {
				if (test == 8) {
					if (i > 0) {
						cout << test << " " << test;
						password = password.substr(0, password.length() - 1);
						i--;
					}
				}
				else if (i < 20) {
					cout << "*";
					password += test;
					i++;
				}
			}
			cout << endl << "If you want to see the password, please enter 1. Please enter 0 to skip. Please enter 2 to change." << endl;
			int check;
			int flag = 0;
			cin >> check;
			while (check != 0) {
				if(check)
					cout << password << endl;
				if (check == 2) {
					password = "";
					i = 0;
					while ((test = _getch()) != '\r') {
						if (test == 8) {
							if (i > 0) {
								cout << test << " " << test;
								password = password.substr(0, password.length() - 1);
								i--;
							}
						}
						else if (i < 20) {
							cout << "*";
							password += test;
							i++;
						}
					}
				}
				if (password == "Q") {
					flag = 1;
					break;
				}
				cout << endl << "If you want to see the password, please enter 1. Please enter 0 to skip. Please enter 2 to change." << endl;
				cin >> check;
			}
			if (flag) {
				flag = 0;
				cout << "\nExiting\n\n";
				Interface_Jumping();
				break;
			}
			Interface_Jumping();
			if (name[0] == 't') {
				TestBuilder tester(name, password);
				tester.Register();
			}
			else {
				Player player(name, password);
				player.Register();
			}
		}
				break;
				/**
				* player operation page
				*/
		case 2: {
			cout << "Please input username:";
			string name;
			cin >> name;
			if (name == "Q") {
				cout << "\nExiting\n\n";
				Interface_Jumping();
				break;
			}
			cout << "Please input password:";
			string password;
			char test = 'a';
			int i = 0;
			while ((test = _getch()) != '\r') {
				if (test == 8) {
					if (i > 0) {
						cout << test << " " << test;
						password = password.substr(0, password.length() - 1);
						i--;
					}
				}
				else if (i < 20) {
					cout << "*";
					password += test;
					i++;
				}
			}
			int flag = 0;
			Player player(name, password);
			int pos = player.Login();
			if (pos != -2) {
				while (pos == -1) {
					cout << "Username or password is wrong,please input them again:" << endl;
					cout << "Please input username:";
					string name;
					cin >> name;
					if (name == "Q") {
						flag = 1;
						break;
					}
					cout << "Please input password:";
					string password;
					int i = 0;
					while ((test = _getch()) != '\r') {
						if (test == 8) {
							if (i > 0) {
								cout << test << " " << test;
								password = password.substr(0, password.length() - 1);
								i--;
							}
						}
						else if (i < 20) {
							cout << "*";
							password += test;
							i++;
						}
					}
					Player player(name, password);
					pos = player.Login();
				}
				if (flag) {
					flag = 0;
					cout << "\nExiting\n\n";
					Interface_Jumping();
					break;
				}
				Player& tempPlayer = (Player&)Player::getList()[pos];
				int playerChoice = 1;
				Game game;
				while (playerChoice != 0) {
					Interface_Jumping();
					cout << "\n\n\n\n" << "              ************************" << endl;
					cout << "                Welcome to playing page ! " << endl;
					cout << "\n                 1  select the difficulty of the game" << endl;//-请选择难度游戏难度
					cout << "\n                 2  start the game" << endl;// -开始游戏
					cout << "\n                 3  next level" << endl;// -下一关
					cout << "\n                 4  search user " << endl;//-查询用户
					cout << "\n                 5  rank playerList" << endl;// -排行榜
					cout << "\n                 0  quit playing page" << endl;// -退出游戏界面
					cout << "              ************************" << endl;
					do {
						cin >> playerChoice;
						if (playerChoice < 0 || playerChoice > 5)
							cout << "Input is wrong，please input again!" << endl;
						else
							break;
					} while (1);
					switch (playerChoice) {
					case 1: {
						int l = 0;
						cout << "Please choose the difficulty (0-10):" << endl;
						cin >> l;
						game.setLevel(l);
					}
							break;
					case 2:
						if (game.Display()) {
							tempPlayer.addGainedGrades();
							tempPlayer.addLevel();
						}
						tempPlayer.addExperience();

						break;
					case 3:
						if (game.Display()) {
							tempPlayer.addGainedGrades();
							tempPlayer.addLevel();
						}
						tempPlayer.addExperience();

						break;
					case 4: {
						cout << "Please choose the way to search user:" << endl;
						cout << "              a username  \n              b userLevel    \n" << endl;
						char rankChoice;
						cin >> rankChoice;
						while ((rankChoice == 'a') || (rankChoice == 'b')) {
							if (rankChoice == 'a') {
								cout << "Please input the username:" << endl;
								string name;
								cin >> name;
								tempPlayer.SearchPlayer(name);
								tempPlayer.SearchTestBuilder(name);
							}
							else if (rankChoice == 'b'){
								cout << "Please input the Player's level:" << endl;
								int level;
								cin >> level;
								tempPlayer.SearchPlayerBylevel(level);
							}
							cin >> rankChoice;
						}
					}
							break;
					case 5: {
						cout << "      Please choose the characters to rank the Player list:\n"
							<< "              a gainedGrades  \n"
							<< "              b experience    \n"
							<< "              c best level    \n";
						char rankChoice;
						cin >> rankChoice;
						while ((rankChoice == 'a') || (rankChoice == 'b') || (rankChoice == 'c')) {
							if (rankChoice == 'a')
								tempPlayer.RankPlayersByGrades();
							else if (rankChoice == 'b')
								tempPlayer.RankPlayersByExperience();
							else if (rankChoice == 'c')
								tempPlayer.RankPlayersByBestLevel();
							cin >> rankChoice;
						}
					}
							break;
					case 0:
						Interface_Jumping();
							break;
					}
				}
				tempPlayer.updateInfo();
				tempPlayer.quitLogin();
			}
			else
				cout << "There is others online. Please wait for a while.";
		}
				break;
				/**
				* tester operation page
				*/
		case 3: {
			cout << "Please input username:";
			string name;
			cin >> name;
			if (name == "Q") {
				cout << "\nExiting\n\n";
				Interface_Jumping();
				break;
			}
			cout << "Please input password:";
			string password;
			char test = 'a';
			int i = 0;
			while ((test = _getch()) != '\r') {
				if (test == 8) {
					if (i > 0) {
						cout << test << " " << test;
						password = password.substr(0, password.length() - 1);
						i--;
					}
				}
				else if (i < 20) {
					cout << "*";
					password += test;
					i++;
				}
			}
			int flag = 0;
			TestBuilder tester(name, password);
			int pos = tester.Login();
			if (pos != -2) {
				while (pos == -1) {
					cout << "Username or password is wrong,please input them again:" << endl;
					cout << "Please input username again:";
					string name;
					cin >> name;
					if (name == "Q") {
						flag = 1;
						break;
					}
					cout << "Please input password again:";
					string password;
					int i = 0;
					while ((test = _getch()) != '\r') {
						if (test == 8) {
							if (i > 0) {
								cout << test << " " << test;
								password = password.substr(0, password.length() - 1);
								i--;
							}
						}
						else if (i < 20) {
							cout << "*";
							password += test;
							i++;
						}
					}
					TestBuilder tester(name, password);
					pos = tester.Login();
				}
				if (flag) {
					flag = 0;
					cout << "\nExiting\n\n";
					Interface_Jumping();
					break;
				}
				TestBuilder& tempTester = (TestBuilder&)TestBuilder::getTestBuilderList()[pos];
				tempTester.addTestBuilderLevel();
				int testerChoice = 1;
				while (testerChoice != 0) {
					Interface_Jumping();
					cout << "\n\n\n\n" << "              *************************" << endl;
					cout << "                Welcome to management page ! " << endl;
					cout << "\n                 1  build new tester case" << endl;//-增加单词
					cout << "\n                 2  search user" << endl;// -查询用户
					cout << "\n                 3  rank testBuilderList " << endl;//-排行榜
					cout << "\n                 0  quit management page " << endl;//-退出管理界面
					cout << "              ************************" << endl;
					do {
						cin >> testerChoice;
						if (testerChoice < 0 || testerChoice > 3)
							cout << "Input is wrong，please input again!" << endl;
						else
							break;
					} while (1);
					switch (testerChoice) {
					case 1:
						tempTester.AddWord();
						tempTester.addProblemsNumbers();
						break;
					case 2: {
						cout << "Please choose the way to search user:" << endl;
						cout << "              a username  \n              b userLevel    \n" << endl;
						char rankChoice;
						cin >> rankChoice;
						while ((rankChoice == 'a') || (rankChoice == 'b')) {
							if (rankChoice == 'a') {
								cout << "Please input the username:" << endl;
								string name;
								cin >> name;
								tempTester.SearchPlayer(name);
								tempTester.SearchTestBuilder(name);
							}
							else if (rankChoice == 'b'){
								cout << "Please input the tester's level:" << endl;
								int level;
								cin >> level;
								tempTester.SearchTesterBylevel(level);
							}
							cin >> rankChoice;
						}
					}

							break;
					case 3: {
						cout << "      Please choose the characters to rank the Tester list:\n"
							<< "              a problemsNumbers  \n"
							<< "              b testBuilderLevel    \n";
						char rankChoice;
						cin >> rankChoice;
						while ((rankChoice == 'a') || (rankChoice == 'b')) {
							if (rankChoice == 'a')
								tempTester.RankPlayersByProNum();
							else if(rankChoice == 'b')
								tempTester.RankPlayersByTesterLevel();
							cin >> rankChoice;
						}
					}
							break;
					case 0:
						Interface_Jumping();
							break;
					}
				}
				tempTester.updateInfo();
				tempTester.quitLogin();
			}
			else
				cout << "There is others online. Please wait for a while.";
		}
				break;
		}
	}
	return 0;
}


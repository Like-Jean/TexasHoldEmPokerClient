#include <iostream>
#include <time.h>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strstream>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <map>
#include "cal.h"
int TEMPSUM = 0;
map<string, int> cnums;
map<string, int> clors;
bool Print = true;
int AF_HOLD = 0;
int AF_FLOP = 3;
int AF_TURN = 4;
int AF_RIVER = 5;
void closeSocket(int sockfd) {
	close(sockfd);
}

//注意：当字符串为空时，也会返回一个空字符串
void split(std::string& s, std::string& delim, std::vector<std::string>* ret) {
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos) {
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0) {
		ret->push_back(s.substr(last, index - last));
	}
}

void getHoldCard(string src, Card* c) {
	vector<string> holdmes;
	holdmes.clear();
	string kk = " ";
	split(src, kk, &holdmes);
	c->num = cnums.find(holdmes[1])->second;
	c->color = clors.find(holdmes[0])->second;
}

int main(int argc, char *argv[]) {
	cnums.insert(pair<string, int>("A", 14));
	cnums.insert(pair<string, int>("K", 13));
	cnums.insert(pair<string, int>("Q", 12));
	cnums.insert(pair<string, int>("J", 11));
	cnums.insert(pair<string, int>("10", 10));
	cnums.insert(pair<string, int>("9", 9));
	cnums.insert(pair<string, int>("8", 8));
	cnums.insert(pair<string, int>("7", 7));
	cnums.insert(pair<string, int>("6", 6));
	cnums.insert(pair<string, int>("5", 5));
	cnums.insert(pair<string, int>("4", 4));
	cnums.insert(pair<string, int>("3", 3));
	cnums.insert(pair<string, int>("2", 2));
	clors.insert(pair<string, int>("DIAMONDS", 1));
	clors.insert(pair<string, int>("CLUBS", 2));
	clors.insert(pair<string, int>("HEARTS", 3));
	clors.insert(pair<string, int>("SPADES", 4));
	int step = 0;
	Card cards[5]; //公共排+翻牌+河牌
	Card mycards[2]; //手牌
	vector<string> message;
	memset(cards, 0, sizeof(int) * 5);
	memset(mycards, 0, sizeof(int) * 2);
	int card1, card2, card3, card4, card5, money, jetton, jet, total;
	bool isb = false;
	bool isbb = false;
	bool issb = false;
	char temp[6];
	char color1, color2, color3, color4, color5;
	const char *inq = "inquire/";
	const char *get_pot = "total pot:";
	const char *get_flop = "flop/"; //公牌
	const char *game_over = "game-over";
	const char *seat = "seat/";
	const char *blind = "blind/";
	const char *hold = "hold/";
	const char *turn = "turn/";
	const char *river = "river/";
	const char *showdown = "showdown/";
	const char *pot_win = "pot-win/";
	const char *notify = "notify/";

	//建立socket
	int sockfd;
	struct sockaddr_in servaddr;
	if (argc < 5) {
		cout << "arguments error" << endl;
		return -1;
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}
	sockaddr_in clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(atoi(argv[4]));
	clientaddr.sin_addr.s_addr = inet_addr(argv[3]);
	while (bind(sockfd, (sockaddr*) &clientaddr, sizeof(clientaddr)) == -1) {
		cout << "bind error" << endl;
//		exit(-1);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", argv[1]);
		exit(0);
	}

	while (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		usleep(100);
		//printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
		//exit(0);
	}

	char sendl[1024], rev[1024];
	string pid = argv[5];
	const char *p = pid.data();
	char *select;

	string pname = "Absolute";

	memset(sendl, 0, sizeof(sendl));
	memset(rev, 0, sizeof(rev));

	//向服务器注册
	string msg = "reg: " + pid + " " + pname + " \n";
	unsigned i, j;
	for (i = 0; i <= msg.length(); i++)
		sendl[i] = msg[i];
	sendl[i] = '\0';

	if (send(sockfd, sendl, strlen(sendl), 0) < 0) {
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}

	for (i = 0; i <= msg.length(); i++)
		sendl[i] = '\0';
	//主循环
//	time_t bb = 0;
//	time_t ee = 0;
	string strrev;
	int contDEAD = 0;
	while (1) {
//		cout<<"torec"<<endl;
//		bb = clock();
		while (recv(sockfd, rev, 1024, 0) < 0) {
			cout << "rec_error" << endl;
			close(sockfd);
			while (connect(sockfd, (struct sockaddr*) &servaddr,
					sizeof(servaddr)) < 0) {
				usleep(100);
				//printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
				//exit(0);
			}
		}

//		ee = clock();
//		if (ee - bb > 0.5 * CLOCKS_PER_SEC) {
//			cout << "接收耗时：" << ((float) (bb - ee) / CLOCKS_PER_SEC) << endl;
//		}
		//cout << rev << "***********************************end" << endl;
//		if(strrev.size()!=0){
//		cout << strrev + rev << endl;
//		}
		strrev += rev;
		if (strrev.size() == 0) {
			cout << "dead?";
			contDEAD++;
			if (contDEAD >= 1000) {
				close(sockfd);
				exit(-1);
			}
		}
//		cout << strrev;
		string sub;
		string nextdo;
		while (strrev.size() != 0) {
			message.clear();
			if (strrev.find("/") == string::npos) {
				if (strrev.find(game_over) != string::npos) {
					close(sockfd);
					exit(-1);
				}
			}
			sub = strrev.substr(0, strrev.find("/"));
			if (strrev.find("/" + sub) == string::npos) {	//并非一个完整的指令，需要叠加下一条
				//cout<<"*********************hahahahahahahhahzhaodaole !!!!!"<<endl;
				strrev.erase(strrev.size() - 3, strrev.size());
				memset(rev, 0, sizeof(rev));
				break;
			}
			nextdo = strrev.substr(0, strrev.find("/" + sub) + sub.size() + 3);
			string nn = "\n";
			split(nextdo, nn, &message);
			//cout<<message.size()<<endl;
			if (nextdo.find(seat) != string::npos) {		//座次信息
				vector<string> seat;
				vector<string> sb;
				vector<string> bb;
				seat.clear();
				string kk = " ";
				split(message[1], kk, &seat);
				split(message[2], kk, &sb);
				split(message[3], kk, &bb);
				isb = false;
				isbb = false;
				issb = false;
				if (seat.at(1) == p) {		//bottom
					isb = true;
				} else if (sb.at(2) == p) {
					issb = true;
				} else if (bb.at(2) == p) {
					isbb = true;
				}
				//接受筹码信息
				jetton = 0;
				jet = 0;

				for (i = 0;
						*(strstr(strrev.data(), p) + i + 1 + pid.length())
								!= ' '; i++) {
					temp[i] =
							*(strstr(strrev.data(), p) + i + 1 + pid.length());
				}
				for (j = 0; j < i; j++) {
					jetton *= 10;
					jetton += (temp[j] - '0');
				}

				//接受money信息
				money = 0;
				for (i = 0;
						*(strstr(strrev.data(), p) + i + j + 2 + pid.length())
								!= ' '; i++) {
					temp[i] = *(strstr(strrev.data(), p) + i + j + 2
							+ pid.length());
				}
				for (j = 0; j < i; j++) {
					money *= 10;
					money += (temp[j] - '0');
				}
//				if(Print){
//				if (isb) {
//					cout << "isb" << endl;
//				} else if (issb) {
//					cout << "issb" << endl;
//				} else if (isbb) {
//					cout << "isbb" << endl;
//				}
//				cout << money << "ddd" << jetton << endl;
//				}
			} else if (nextdo.find(blind) != string::npos) {		//盲注

			} else if (nextdo.find(hold) != string::npos) {		//手牌消息
				step = AF_HOLD;
				getHoldCard(message[1], &mycards[0]);
				getHoldCard(message[2], &mycards[1]);
			} else if (nextdo.find(get_flop) != string::npos) {		//公牌消息
				step = AF_FLOP;
				getHoldCard(message[1], &cards[0]);
				getHoldCard(message[2], &cards[1]);
				getHoldCard(message[3], &cards[2]);
			} else if (nextdo.find(turn) != string::npos) {		//转牌（第四张）
				step = AF_TURN;
				getHoldCard(message[1], &cards[3]);
			} else if (nextdo.find(river) != string::npos) {		//河牌（第五张）
				step = AF_RIVER;
				getHoldCard(message[1], &cards[4]);
			} else if (nextdo.find(inq) != string::npos) {		//TODO 行动
				string flod = "flod ";
				string check = "check ";
				string call = "call ";
				string raise = "raise ";
				string all_in = "all-in ";
				string action;
				time_t start,finish;
				start = clock();
				//决定行动action=flod/check/call/raise/all-in
				//TODO
//				bb = clock();
				int sum = 0;
				int max = 0;
				strstream ss;
				string ra;
				for (int k = 1; k < message.size() - 4; k++) {
					vector<string> iq;
					iq.clear();
					string kk = " ";
					split(message[k], kk, &iq);
					max = max > atoi(iq.at(3).data()) ?
							max : atoi(iq.at(3).data());
					sum += atoi(iq.at(3).data());
				}
				if (step == AF_HOLD) {			//刚发手牌
					int value = getValue(mycards[0], mycards[1]);
					if (value >= 10 || isbb || (issb && value >= 6)) {
						if (value < 20 && max * 2 > jetton) {
							action = flod;
						} else {
							action = call;
						}
					} else {
						action = flod;
					}
				} else {
					//cout << "**********进入前sca***********step:" << step << " "
					//		<< TEMPSUM << endl;
					time_t sss,eee;
					sss = clock();
					int losePresent = toSCA(cards, step, mycards, 2);
					eee = clock();
//					cout <<"决策耗时:"<< ((float) (eee - sss) / CLOCKS_PER_SEC)<<endl;
					//cout << "**********进入sca后***********" << ++TEMPSUM << "输的概率"
					//		<< losePresent / 100 << endl;

					if (losePresent >= 5000) {
						action = flod;
					} else if (losePresent > 3000) {				//30-50失败

						action = call;
						if (max > jetton && jetton > 1000) {
							action = flod;
						}
					} else if (losePresent > 1500) {
						action = call;
						if (step == AF_FLOP) {
							if (jetton > max * 3) {
								action = raise + "100";
							}
						} else if (step == AF_TURN) {

							if (jetton > max * 2) {
								ss << (int) (jetton - max) / 2;
								ss >> ra;
								action = raise + ra;
							}
						} else if (step == AF_RIVER) {
							if (jetton > max * 2) {
								ss << (int) (jetton - max) / 2;
								ss >> ra;
								action = raise + ra;
							}
						}

					} else if (losePresent > 500) {
						action = call;
						if (step == AF_FLOP) {
							if (jetton > max * 3) {
								action = raise + "100";
							}
						} else if (step == AF_TURN) {

							if (jetton > max * 2) {
								ss << (int) (jetton - max) / 2;
								ss >> ra;
								action = raise + ra;
							}
						} else if (step == AF_RIVER) {
							if (jetton > max * 2) {
								ss << (int) (jetton - max) / 2;
								ss >> ra;
								action = raise + ra;
							}
						}
					} else {				//5以下输率
						action = all_in;
					}
				}
				//action = call;
				//action = raise+"200";
				//决定完毕，发送
				memset(sendl, 0, sizeof(sendl));
				for (i = 0; i <= action.length(); i++) {
					sendl[i] = action[i];
				}
				sendl[i] = '\0';
				if (send(sockfd, sendl, strlen(sendl), 0) < 0) {
					cout << "send_error" << endl;
				}
				finish = clock();
//				cout <<"决策耗时:"<< ((float) (finish - start) / CLOCKS_PER_SEC)<<endl;
//				ee = clock();
//				if (ee - bb > 0.5 * CLOCKS_PER_SEC)
//					cout << "发送耗时：" << ((float) (bb - ee) / CLOCKS_PER_SEC)
//							<< endl;

				//			for(int i=0;i<2;i++){
				//				cout<<mycards[i].color<<"~"<<mycards[i].num<<"\t";
				//			}
				//			cout<<endl;
				//			for(int i=0;i<step;i++){
				//				cout<<cards[i].color<<"~"<<cards[i].num<<"\t";
				//			}
				//cout << "\t\t\t\t\t\t\t\t\t" << step << "\t" << sendl << "\tsdddddd" << endl;
				//					<< "\t\t\t\t\t\t\t\t\t\t\tsdddddd\n" << endl;
			} else if (nextdo.find(showdown) != string::npos) {		//摊牌

			} else if (nextdo.find(pot_win) != string::npos) {		//彩池分配信息
//				step = 0;
//				memset(cards, 0, sizeof(cards));
//				memset(mycards, 0, sizeof(mycards));
			} else if (nextdo.find(game_over) != string::npos) {		//结束游戏
				close(sockfd);
				exit(0);
			}
			strrev.erase(0, strrev.find("/" + sub) + sub.size() + 3);
		}
		memset(rev, 0, sizeof(rev));
	}
	exit(0);
}

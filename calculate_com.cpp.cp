#include <iostream>
#include <stdlib.h>
#include <vector>
#include <list>
#include <time.h>
#include <string.h>
#include "cal.h"
using namespace std;
//
//struct Card {
//	int color;
//	int num;
//};
//
//struct HandCard {
//	int rank; //牌组等级
//	int handIndex; //牌组的排位
//	bool isMyHand; //是否手牌
//};
int sum = 0;
bool IS_PRINT = true;
Card* CARDS_LIST = new Card[52];
const int RAND_LOWEST = -20000; //-18548~80014,阈值:最低比较的
int PAIR_VALUE[] = { 0, 0, 11, 11, 11, 12, 13, 13, 15, 16, 19, 22, 27, 30, 40, };
int SAME_VALUE[15][14] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //0
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //1
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //2
		{ 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //3
		{ 0, 0, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //4
		{ 0, 0, 8, 9, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //5
		{ 0, 0, 6, 8, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0 }, //6
		{ 0, 0, 6, 7, 9, 10, 11, 0, 0, 0, 0, 0, 0, 0 }, //7
		{ 0, 0, 6, 6, 8, 9, 11, 12, 0, 0, 0, 0, 0, 0 }, //8
		{ 0, 0, 7, 7, 7, 8, 10, 12, 13, 0, 0, 0, 0, 0 }, //9
		{ 0, 0, 8, 8, 8, 8, 10, 11, 13, 15, 0, 0, 0, 0 }, //10
		{ 0, 0, 8, 9, 9, 9, 9, 11, 13, 15, 18, 0, 0, 0 }, //11
		{ 0, 0, 9, 10, 10, 10, 11, 11, 13, 15, 18, 19, 0, 0 }, //12
		{ 0, 0, 11, 11, 11, 12, 12, 13, 13, 15, 18, 20, 21, 0 }, //13
		{ 0, 0, 13, 14, 14, 14, 14, 14, 15, 16, 19, 20, 22, 24 }, //A
		};

int OPPO_VALUE[15][14] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //0
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //1
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //2
		{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //3
		{ 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //4
		{ 0, 0, 2, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //5
		{ 0, 0, 1, 3, 4, 6, 0, 0, 0, 0, 0, 0, 0, 0 }, //6
		{ 0, 0, 0, 1, 3, 5, 6, 0, 0, 0, 0, 0, 0, 0 }, //7
		{ 0, 0, 0, 1, 2, 4, 5, 7, 0, 0, 0, 0, 0, 0 }, //8
		{ 0, 0, 1, 1, 1, 3, 4, 6, 8, 0, 0, 0, 0, 0 }, //9
		{ 0, 0, 2, 2, 2, 2, 4, 6, 8, 10, 0, 0, 0, 0 }, //10
		{ 0, 0, 2, 2, 3, 3, 3, 5, 7, 9, 13, 0, 0, 0 }, //11
		{ 0, 0, 3, 3, 4, 4, 4, 5, 7, 9, 12, 14, 0, 0 }, //12
		{ 0, 0, 4, 5, 5, 5, 6, 6, 7, 9, 13, 14, 16, 0 }, //13
		{ 0, 0, 7, 7, 8, 8, 7, 8, 9, 10, 13, 14, 16, 19 }, //A
		};
int getValue(Card c, Card d) {
	if (c.num == d.num) {
		return PAIR_VALUE[c.num];
	} else if (c.color == d.color) {
		if (c.num > d.num) {
			return SAME_VALUE[c.num][d.num];
		} else {
			return SAME_VALUE[d.num][c.num];
		}
	} else {
		if (c.num > d.num) {
			return OPPO_VALUE[c.num][d.num];
		} else {
			return OPPO_VALUE[d.num][c.num];
		}
	}
}
// 按照号码排序：梅花2 红桃2 黑桃3 方块A 黑桃A
void sortByNum(Card* cards, int length) {
	for (int i = 0; i < length - 1; i++) {
		for (int j = 0; j < length - i - 1; j++) {
			if (cards[j].num > cards[j + 1].num) {
				Card temp = cards[j];
				cards[j] = cards[j + 1];
				cards[j + 1] = temp;
			} else if (cards[j].num == cards[j + 1].num
					&& cards[j].color > cards[j + 1].color) { //TODO
				Card temp = cards[j];
				cards[j] = cards[j + 1];
				cards[j + 1] = temp;
			}
		}
	}
}

int comp(const void* a, const void* b) {
	return (*(HandCard*) b).rank - (*(HandCard*) a).rank;
}

/**
 * 数组排序和扫描index排名
 *
 * @param allLength    数组长度,一般为991=1+C(2)(45)
 * @param allHandCards 数组
 * @return 我的那组牌组占其中数组的index, 除以allLength便是胜率
 */
int sort(int allLength, HandCard* allHandCards) {

	// 系统排序,判断rank从大到小,index为0的最大
	qsort(allHandCards, allLength, sizeof(HandCard), comp);
	int myHandIndex = 0;

	allHandCards[0].handIndex = 0;
	for (int i = 1; i < allLength; i++) {
		// 让排名可以相等的情况,类似 8 5 5 5 3 3 2 1 0
		if (allHandCards[i].rank == allHandCards[i - 1].rank) {
			allHandCards[i].handIndex = allHandCards[i - 1].handIndex;
		} else {
			allHandCards[i].handIndex = i;
		}

		// 处理手牌的平均排名
		if (allHandCards[i].isMyHand) {
			myHandIndex = allHandCards[i].handIndex;
		}
	}

	return myHandIndex;
}
//isSame
bool isSame(Card c1, Card c2) {
	return c1.num == c2.num && c1.color == c2.color;
}

// 逆序按照号码排序： 黑桃A 方块A 黑桃3 红桃2 梅花2
void sortByNumDesc(Card* cards, int length) {
	for (int i = 0; i < length - 1; i++) {
		for (int j = 0; j < length - i - 1; j++) {
			if (cards[j].num < cards[j + 1].num) {
				Card temp = cards[j];
				cards[j] = cards[j + 1];
				cards[j + 1] = temp;
			} else if (cards[j].num == cards[j + 1].num
					&& cards[j].color < cards[j + 1].color) { //TODO
				Card temp = cards[j];
				cards[j] = cards[j + 1];
				cards[j + 1] = temp;
			}
		}
	}
}
/**
 * 给定5张牌和2张手牌,测算等级
 *
 * @param targetCards 牌组
 * @return 牌组的rank
 */
int findRank(Card* targetCards) {

	int sameStart = 0, sameEnd = 0;
	int flushMaxStart = 0, flushMaxEnd = 0;
	int flushStart = 0, flushEnd = 0;
	vector<int> sameInList; // = new ArrayList<Integer>();//记录蛇里面相同的数字,除了最后面那个位置其他都记录
	int* colorCount = new int[5];
	int colorNum[5][7]; // 分颜色的牌组
	memset(colorCount, 0, sizeof(int) * 5);
	memset(colorNum, 0, sizeof(int) * 5 * 7);
	int hasFour = 0, hasThree = 0, hasThree2 = 0;
	vector<int> hasTwoList; // = new ArrayList<Integer>();
	vector<int> singleList; // = new ArrayList<Integer>();//非相同,最大列表,第七个永远不会拉入

	// 大到小排序
	sortByNumDesc(targetCards, 7);

	// 调试
//        if (samePrint(targetCards, "[{4, 14}, {1, 14}, {4, 13}, {1, 13}, {4, 12}, {4, 11}, {4, 10}]")) {
//            int x = 0;
//        }

	// 判断相同和蛇
	colorNum[targetCards[0].color][colorCount[targetCards[0].color]++] =
			targetCards[0].num;

	for (int i = 1; i < 7; i++) {
		Card nowCard = targetCards[i];
		Card lastCard = targetCards[i - 1];
		colorNum[nowCard.color][colorCount[nowCard.color]++] = nowCard.num;

		if (nowCard.num == lastCard.num) {
			// 相同的情况下

			// 相同的跨度更大
			sameEnd = i;

			// 蛇记录相同
			flushEnd = i;
			sameInList.push_back(i - 1);
		} else {
			// 不相同的情况下

			// 相同段取出
			switch (sameEnd - sameStart) {
			case 0:
				// 加入非相同的最大列表
				singleList.push_back(targetCards[i - 1].num);
				break;
			case 1:
				hasTwoList.push_back(targetCards[sameEnd].num);
				break;
			case 2:
				if (hasThree == 0) {
					hasThree = sameEnd;
				} else {
					hasThree2 = sameEnd;
				}
				break;
			case 3:
				hasFour = sameEnd;
				break;
			}

			// 重置相同
			sameEnd = i;
			sameStart = i;

			// 判断蛇
			if (nowCard.num == lastCard.num - 1) {
				// 差一的情况下
				flushEnd = i;
			} else {
				// 不是差一的情况下

				// 把最长的蛇段取出
				if (flushEnd - flushStart > flushMaxEnd - flushMaxStart) {
					flushMaxEnd = flushEnd;
					flushMaxStart = flushStart;
				}

				// 重置蛇
				flushEnd = i;
				flushStart = i;
			}

		}
	}

	// 最后一次把最长的蛇段取出
	if (flushEnd - flushStart > flushMaxEnd - flushMaxStart) {
		flushMaxEnd = flushEnd;
		flushMaxStart = flushStart;
	}

	// 最后一次相同段 和 最大 取出
	switch (sameEnd - sameStart) {
	case 1:
		hasTwoList.push_back(targetCards[sameEnd].num);
		break;
	case 2:
		if (hasThree == 0) {
			hasThree = sameEnd;
		} else {
			hasThree2 = sameEnd;
		}
		break;
	case 3:
		hasFour = sameEnd;
		break;
	}

	// 判断4条 Rank = 700
	if (hasFour != 0) {
		if (hasFour == 3) {
			return 70000 + 8 * targetCards[hasFour].num
					+ targetCards[hasFour + 1].num;
		} else {
			return 70000 + 8 * targetCards[hasFour].num + targetCards[0].num;
		}
	}

	// 判断同花或者同花顺 Rank = 500 or 800
	int color = 0;
	for (int i = 1; i < 5; i++) {
		if (colorCount[i] >= 5) {
			color = i;
			break;
		}
	}
	if (color != 0) {
		// 判断同花顺
		int sameIndex = 0, sameCount = 0;
		for (int i = 1; i < colorCount[color]; i++) {
			if (colorNum[color][i] == colorNum[color][i - 1] - 1) {
				sameCount++;
			} else {
				if (sameCount < 4) {
					sameCount = 0;
					sameIndex = i;
				}
			}
		}
		if (sameCount >= 4) {
			return 80000 + colorNum[color][sameIndex];
		}
		return 50000 + colorNum[color][0];
	}

	// 判断顺子 Rank = 400
	if (flushMaxEnd - flushMaxStart - (int) sameInList.size() > 3) {
		return 40000 + targetCards[flushMaxStart].num;
	}

	// 判断3条或者俘虏 Rank = 300 or 600
	if (hasThree != 0) {
		// 有一对
		if (hasTwoList.size() != 0) {
			return 60000 + 8 * targetCards[hasThree].num + hasTwoList.at(0);
		}
		// 2个3条
		if (hasThree2 != 0) {
			return 60000 + 8 * targetCards[hasThree].num
					+ targetCards[hasThree2].num;
		}
		return 30000 + 64 * targetCards[hasThree].num + 8 * singleList.at(0)
				+ singleList.at(1);
	}

	// 判断2对和1对 Rank = 200 or 100
	switch (hasTwoList.size()) {
	case 1:
		// 1对
		return 10000 + 512 * hasTwoList.at(0) + 64 * singleList.at(0)
				+ 8 * singleList.at(1) + singleList.at(2);
	case 2:
		// 2对
		return 20000 + 64 * hasTwoList.at(0) + 8 * hasTwoList.at(1)
				+ singleList.at(0);
	case 3:
		// 3对
		int max =
				(singleList.size() == 0 || hasTwoList.at(2) > singleList.at(0)) ?
						hasTwoList.at(2) : singleList.at(0);
		return 20000 + 64 * hasTwoList.at(0) + 8 * hasTwoList.at(1) + max;

	}

	// 高牌 最大64 866,不能超过10000
	return 4096 * singleList.at(0) + 512 * singleList.at(1)
			+ 64 * singleList.at(2) + 8 * singleList.at(3) + singleList.at(4)
			- 60000;
}

/**
 * 测试总数桌面已经有x张牌,剩下别人手上的2张牌的组合 : cards长度为x, myHandCards长度为2
 *
 * @param tableCards  桌面的牌
 * @param myHandCards 我的手牌
 * @return 返回胜率的整数, 除以100便是胜率,如9876代表98.76%
 */

int scanAll(Card* tableCards, const int CL, Card* myHandCards, const int myCL) {

	// 连接我的手牌和桌面牌,形成需要去除的数组
	Card* needRemoveCards = new Card[CL + myCL];
	memset(needRemoveCards, 0, sizeof(int) * (CL + myCL));
	for (int i = 0; i < CL; i++) {
		needRemoveCards[i] = tableCards[i];
	}
	for (int i = CL; i < CL + myCL; i++) {
		needRemoveCards[i] = myHandCards[i - CL];
	} //TODO

	// 数组排序
	sortByNum(needRemoveCards, CL + myCL);
	sortByNum(tableCards, CL);
	sortByNum(myHandCards, myCL);

	// 初始化
	int sameIndex = 0, arrayIndex = 0, sumAllMyHandPresent = 0;
	Card* cardArray = new Card[52 - CL - myCL]; // 无手牌和桌面牌的数组
	memset(cardArray, 0, sizeof(int) * (52 - CL - myCL));
	for (int i = 0; i < 52; i++) {
		if (sameIndex < (CL + myCL)
				&& isSame(CARDS_LIST[i], needRemoveCards[sameIndex])) {
			sameIndex++;
		} else {
			cardArray[arrayIndex].color = CARDS_LIST[i].color;
			cardArray[arrayIndex++].num = CARDS_LIST[i].num;
		}
	}

	Card* randomCards = new Card[5]; // 随机生成的桌面牌
	memset(randomCards, 0, sizeof(int) * 5);
	int allLength = 0;

	switch (CL) {
	// 5张牌的情况下
	case 5: {
		Card targetCards[7]; // = new Card[7];// 随机敌方的手牌+桌面牌
		memset(targetCards, 0, sizeof(targetCards));
		allLength = 0;
		HandCard allHandCards[991]; // = new HandCard[991];// 排序后的存放数组 C(2)(52-7)+1 我的
		memset(allHandCards, 0, sizeof(allHandCards));
		// 第一次循环
		for (int i = 0; i < 52 - CL - myCL; i++) {
			// 第二次循环
			for (int j = i + 1; j < 52 - CL - myCL; j++) {
				// 初始化循环
				targetCards[0].color = cardArray[i].color;
				targetCards[0].num = cardArray[i].num;
				targetCards[1].color = cardArray[j].color;
				targetCards[1].num = cardArray[j].num;

				// 5牌确定,得到新数组
				for (int k = 0; k < CL; k++) {
					targetCards[k + 2] = tableCards[k];
				}

				int rank = findRank(targetCards);
				if (rank > RAND_LOWEST) {
					allHandCards[allLength].rank = rank;
					allHandCards[allLength++].isMyHand = false;
				}

				sum++;
			}
		}

		// 插入自己的手牌
		targetCards[0].color = myHandCards[0].color;
		targetCards[0].num = myHandCards[0].num;
		targetCards[1].color = myHandCards[1].color;
		targetCards[1].num = myHandCards[1].num;
		for (int i = 0; i < CL; i++) {
			targetCards[i + 2] = tableCards[i];
		}
		int rank = findRank(targetCards);
		allHandCards[allLength].rank = rank;
		allHandCards[allLength++].isMyHand = true;
		// 排序

		int myHandIndex = sort(allLength, allHandCards);

////		if (IS_PRINT) {
//////			for (int i = 0; i < allLength; i++) {
////////                        println(allHandCards[i].toString());
////////                        if (allHandCards[i].isMyHand) {
////////                            println(allHandCards[i].toString());
////////                        }
//////			}
////			cout << "牌组数量：" << allLength << endl;
////		cout << "排名：" << myHandIndex << endl << endl;
//		cout << "得分：" << rank << endl << endl;
//////			println(Arrays.toString(tableCards));
//////			println("阈值上的牌组数量：" + allLength);
//////			println("您的排名为：" + myHandIndex);
//////			println("您的胜率为：" + myHandIndex * 10000 / allLength);
//////			println();
////		}
//		cout << myHandIndex << endl;
		return myHandIndex * 10000 / allLength;
		break;
	}
		// 4张牌的情况下
	case 4: {
		/**
		 * 第一个循环
		 */
		for (int i = 0; i < 52 - CL - myCL; i++) {
			randomCards[0].color = cardArray[i].color;
			randomCards[0].num = cardArray[i].num;
			for (int i = 0; i < CL; i++) {
				randomCards[i + 1] = tableCards[i];
			}
			sumAllMyHandPresent += scanAll(randomCards, 5, myHandCards, myCL);
		}

		sumAllMyHandPresent /= 46; // C(1)(52-6)种桌面组合
		break;
	}
		// 3张牌的情况下
	case 3: {
		/**
		 * 第一个循环
		 */
		for (int i = 0; i < 52 - CL - myCL; i++) {
			/**
			 * 第二个循环
			 */
			for (int j = i + 1; j < 52 - CL - myCL; j++) {
				randomCards[0].color = cardArray[i].color;
				randomCards[0].num = cardArray[i].num;
				randomCards[1].color = cardArray[j].color;
				randomCards[1].num = cardArray[j].num;
				for (int i = 0; i < CL; i++) {
					randomCards[i + 2] = tableCards[i];
				}
				sumAllMyHandPresent += scanAll(randomCards, 5, myHandCards,
						myCL);
			}
		}
		sumAllMyHandPresent /= 1081; // C(2)(52-5)种桌面组合

		break;
	}

	}
// 4,5牌的返回胜率的整数
	return sumAllMyHandPresent;
}

int toSCA(Card* tableCards, const int CL, Card* myHandCards, const int myCL) {

	for (int i = 2; i < 15; i++) {
		for (int j = 1; j < 5; j++) {
			CARDS_LIST[(i - 2) * 4 + j - 1].color = j;
			CARDS_LIST[(i - 2) * 4 + j - 1].num = i;
		}
	}
	sum = 0;
	return scanAll(tableCards, CL, myHandCards, myCL);
}

//int main(int argc, char **argv) {
//	time_t start, finish;
//	start = clock();
//
//	Card* share = new Card[5];
//	share[0].color = 3;
//	share[0].num = 7;
//	share[1].color = 1;
//	share[1].num = 4;
//	share[2].color = 4;
//	share[2].num = 5;
//	Card* share2 = new Card[5];
//	share2[0].color = 1;
//	share2[0].num = 2;
//	share2[1].color = 3;
//	share2[1].num = 2;
//	share2[2].color = 2;
//	share2[2].num = 4;
//	share2[3].color = 4;
//	share2[3].num = 4;
//	share2[4].color = 3;
//	share2[4].num = 4;
//	Card* own = new Card[2];
//	own[0].color = 4;
//	own[0].num = 7;
//	own[1].color = 4;
//	own[1].num = 13;
//
//	int losePresent = toSCA(share2, 5, own, 2);
//	cout << losePresent << endl;
//	cout << "chance: " << (100 - (float) losePresent / 100) << "%" << endl;
//	cout << "sum: " << sum << endl;
//
//	losePresent = toSCA(share, 3, own, 2);
//	cout << losePresent << endl;
//	cout << "chance: " << (100 - (float) losePresent / 100) << "%" << endl;
//	cout << "sum: " << sum << endl;
//
//	finish = clock();
//	cout << "time: " << (double) (finish - start) / CLOCKS_PER_SEC << "s"
//			<< endl;
//}

//int main(int argc, char *argv[]) {
//
//	for (int i = 2; i < 15; i++) {
//		for (int j = 1; j < 5; j++) {
//			CARDS_LIST[(i - 2) * 4 + j - 1].color = j;
//			CARDS_LIST[(i - 2) * 4 + j - 1].num = i;
//		}
//	}
//
//	Card* share = new Card[5];
//	share[0].color = 3;
//	share[0].num = 2;
//	share[1].color =2;
//	share[1].num = 10;
//	share[2].color = 4;
//	share[2].num = 14;
////	Card* share = new Card[5];
////	share[0].color = 1;
////	share[0].num = 2;
////	share[1].color = 3;
////	share[1].num = 2;
////	share[2].color = 2;
////	share[2].num = 4;
////	share[3].color = 4;
////	share[3].num = 4;
////	share[4].color = 3;
////	share[4].num = 4;
//	Card* own = new Card[2];
//	own[0].color = 3;
//	own[0].num = 13;
//	own[1].color = 4;
//	own[1].num = 13;
//	time_t start, finish;
//	start = clock();
//	float losePresent = scanAll(share, 3, own, 2);
//	cout<<losePresent<<endl;
//	cout << "chance: " << (100 - (float) losePresent / 100) << "%" << endl;
//	cout << "sum: " << sum << endl;
//	finish = clock();
//	cout << "time: " << (double) (finish - start) / CLOCKS_PER_SEC << "s"
//			<< endl;
//	return 0;
//}

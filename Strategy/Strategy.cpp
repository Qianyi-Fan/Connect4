#include <iostream>
#include <unistd.h>
#include "Point.h"
#include "Strategy.h"


#include"test.h"



using namespace std;

/*
	���Ժ����ӿ�,�ú������Կ�ƽ̨����,ÿ�δ��뵱ǰ״̬,Ҫ�����������ӵ�,�����ӵ������һ��������Ϸ��������ӵ�,��Ȼ�Կ�ƽ̨��ֱ����Ϊ��ĳ�������
	
	input:
		Ϊ�˷�ֹ�ԶԿ�ƽ̨ά����������ɸ��ģ����д���Ĳ�����Ϊconst����
		M, N : ���̴�С M - ���� N - ���� ����0��ʼ�ƣ� ���Ͻ�Ϊ����ԭ�㣬����x��ǣ�����y���
		top : ��ǰ����ÿһ���ж���ʵ��λ��. e.g. ��i��Ϊ��,��_top[i] == M, ��i������,��_top[i] == 0
		_board : ���̵�һά�����ʾ, Ϊ�˷���ʹ�ã��ڸú����տ�ʼ���������Ѿ�����ת��Ϊ�˶�ά����board
				��ֻ��ֱ��ʹ��board���ɣ����Ͻ�Ϊ����ԭ�㣬�����[0][0]��ʼ��(����[1][1])
				board[x][y]��ʾ��x�С���y�еĵ�(��0��ʼ��)
				board[x][y] == 0/1/2 �ֱ��Ӧ(x,y)�� ������/���û�����/�г������,�������ӵ㴦��ֵҲΪ0
		lastX, lastY : �Է���һ�����ӵ�λ��, ����ܲ���Ҫ�ò�����Ҳ������Ҫ�Ĳ������ǶԷ�һ����
				����λ�ã���ʱ��������Լ��ĳ����м�¼�Է������ಽ������λ�ã�����ȫȡ�������Լ��Ĳ���
		noX, noY : �����ϵĲ������ӵ�(ע:�ʵ����?���top�Ѿ����㴦���˲������ӵ㣬Ҳ����˵���ĳһ��
				������ӵ�����ǡ�ǲ������ӵ㣬��ôUI�����еĴ�����Ѿ������е�topֵ�ֽ�����һ�μ�һ������
				��������Ĵ�����Ҳ���Ը�����ʹ��noX��noY��������������ȫ��Ϊtop������ǵ�ǰÿ�еĶ�������,
				��Ȼ�������ʹ��lastX,lastY�������п��ܾ�Ҫͬʱ����noX��noY��)
		���ϲ���ʵ���ϰ����˵�ǰ״̬(M N _top _board)�Լ���ʷ��Ϣ(lastX lastY),��Ҫ���ľ�������Щ��Ϣ�¸������������ǵ����ӵ�
	output:
		������ӵ�Point
*/
extern "C" Point *getPoint(const int M, const int N, const int *top, const int *_board,
						   const int lastX, const int lastY, const int noX, const int noY)
{
	/*
		��Ҫ������δ���
	*/
	int x = -1, y = -1; //���ս�������ӵ�浽x,y��
	int **board = new int *[M];
	for (int i = 0; i < M; i++)
	{
		board[i] = new int[N];
		for (int j = 0; j < N; j++)
		{
			board[i][j] = _board[i * N + j];
		}
	}


	/*
		�������Լ��Ĳ������������ӵ�,Ҳ���Ǹ�����Ĳ�����ɶ�x,y�ĸ�ֵ
		�ò��ֶԲ���ʹ��û�����ƣ�Ϊ�˷���ʵ�֣�����Զ����Լ��µ��ࡢ.h�ļ���.cpp�ļ�
    */


	// for (int i = N-1; i >= 0; i--) {
	// 	if (top[i] > 0) {
	// 		x = top[i] - 1;
	// 		y = i;
	// 		break;
	// 	}
	// }


	std::vector<std::vector<int>> board_vector(M, std::vector<int>(N));

	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			board_vector[i][j] = board[i][j];
		}
	}

	board_vector[noX][noY] = 3;


    // ʹ�ó�ʼ���� board ���� initial_state
    GameState initial_state(board_vector, {lastX, lastY}, 1, M, N); // �������һ�����û��µ��壬��ǰ�ֵ���������

    // ���� MCTS ���󲢻�ȡ��Ѷ���
    MonteCarloTreeSearch mcts(initial_state);
    Point best_action = mcts.get_best_action(5000000);
	x = best_action.x; y = best_action.y;







	/*
		��Ҫ������δ���
	*/
	clearArray(M, N, board);
	return new Point(x, y);
}

/*
	getPoint�������ص�Pointָ�����ڱ�soģ���������ģ�Ϊ��������Ѵ���Ӧ���ⲿ���ñ�so�е�
	�������ͷſռ䣬����Ӧ�����ⲿֱ��delete
*/
extern "C" void clearPoint(Point *p)
{
	delete p;
	return;
}

/*
	���top��board����
*/
void clearArray(int M, int N, int **board)
{
	for (int i = 0; i < M; i++)
	{
		delete[] board[i];
	}
	delete[] board;
}

/*
	�������Լ��ĸ�������������������Լ����ࡢ�����������µ�.h .cpp�ļ�������ʵ������뷨
*/




struct MCnode
{
	int a, b;



};

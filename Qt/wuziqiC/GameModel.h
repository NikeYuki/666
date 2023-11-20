#ifndef GAMEMODEL_H
#define GAMEMODEL_H
//五子棋游戏模型类
#include<vector>
//游戏类型，双人还是AI
enum GameType{
    MAN,
    AI
};
//游戏状态
enum GameStatus{
    PLAYING,
    WIN,
    DEAD
};
//棋盘尺寸
const int BOARD_GRAD_SIZE=15;
const int MARGIN=30;//棋盘边缘空隙
const int CHESS_RADIUS=15;//棋子半径
const int MARK_SIZE=6;//落子标记边长
const int BLOCK_SIZE=40;//格子的大小
const int POS_OFFSET=BLOCK_SIZE*0.4;//20 鼠标点击的模糊距离上限
const int AI_THINK_TIME=700;//AI下棋的思考时间

class GameModel{
public:
    GameModel(){}
public:
    //存储当前游戏棋盘和棋子的情况，空白为0，黑子为1，白子为-1
    std::vector<std::vector<int>> gameMapVec;
    //存储各个点位达到的评分情况，作为AI下棋依据
    std::vector<std::vector<int>> scoreMapVec;
    //标示下棋方，true为黑方，false为白方
    bool playerFlag;
    GameType gameType;//游戏模式，双人还是与AI
    GameStatus gameStatus;//游戏状态

    void startGame(GameType type);//开始游戏
    void calculateScore();//计算评分
    void actionByPerson(int row,int col);//人下棋
    void actionByAI (int &clickRow,int &clickCol);//AI下棋
    void updateGameMap(int row,int col);//每次落子后更新棋盘
    bool isWin(int row,int col);//判断游戏是否胜利
    bool isDeadGame();//判断是否和棋
};
#endif // GAMEMODEL_H

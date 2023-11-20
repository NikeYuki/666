#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QPainter>
#include <math.h>
#include <QMessageBox>
#include <QTimer>
#include <QPushButton>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //设置窗口大小
    setFixedSize(MARGIN*2+BLOCK_SIZE*BOARD_GRAD_SIZE,MARGIN*2+BLOCK_SIZE*BOARD_GRAD_SIZE);
    QPushButton *btn1 =new QPushButton("切换至双人模式",this);         //简单设置两个可以切换模式的按钮
    btn1->resize(100,30);
    btn1->show();
    connect(btn1,&QPushButton::clicked,[this](){
        game_type = MAN;
    });

    QPushButton *btn2 =new QPushButton("切换至AI对战模式",this);       //按钮2
    btn2->resize(100,30);
    btn2->move(150,0);
    btn2->show();
    connect(btn2,&QPushButton::clicked,[this](){
        game_type = AI;
    });
    initGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    //绘制棋盘
    painter.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    for(int i=0;i<BOARD_GRAD_SIZE+1;i++){
        //从左到右，第（i+1）条竖线
        painter.drawLine(MARGIN+BLOCK_SIZE*i,MARGIN,MARGIN+BLOCK_SIZE*i,size().height()-MARGIN);
        //从上到下，第（i+1）条横线
        painter.drawLine(MARGIN,MARGIN+BLOCK_SIZE*i,size().width()-MARGIN,MARGIN+BLOCK_SIZE*i);
        }

    //绘制选中点
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    //绘制落子标记
    if(clickPosRow>0 && clickPosRow<BOARD_GRAD_SIZE && clickPosCol>0 && clickPosCol<BOARD_GRAD_SIZE && game->gameMapVec[clickPosRow][clickPosCol]==0){
        if(game->playerFlag)
            brush.setColor(Qt::black);
        else
            brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.drawRect(MARGIN+BLOCK_SIZE*clickPosCol-MARK_SIZE/2,MARGIN+BLOCK_SIZE*clickPosRow-MARK_SIZE/2,MARK_SIZE,MARK_SIZE);

    }

    //绘制棋子
    for(int i=0;i<BOARD_GRAD_SIZE;i++)
        for(int j=0;j<BOARD_GRAD_SIZE;j++){
            if(game->gameMapVec[i][j]==1){
                brush.setColor(Qt::black);

                painter.setBrush(brush);
                painter.drawEllipse(MARGIN+BLOCK_SIZE*j-CHESS_RADIUS,MARGIN+BLOCK_SIZE*i-CHESS_RADIUS,2*CHESS_RADIUS,2*CHESS_RADIUS);
            }
            else if(game->gameMapVec[i][j]==-1){
                brush.setColor(Qt::white);

                painter.setBrush(brush);
                painter.drawEllipse(MARGIN+BLOCK_SIZE*j-CHESS_RADIUS,MARGIN+BLOCK_SIZE*i-CHESS_RADIUS,2*CHESS_RADIUS,2*CHESS_RADIUS);
            }
        }
    //判断输赢
    if (clickPosRow>0&&clickPosRow<BOARD_GRAD_SIZE&&clickPosCol>0&&clickPosCol<BOARD_GRAD_SIZE&&(game->gameMapVec[clickPosRow][clickPosCol]==1||game->gameMapVec[clickPosRow][clickPosCol]==-1)){
        if(game->isWin(clickPosRow,clickPosCol)&&game->gameStatus==PLAYING){
            game->gameStatus=WIN;
            QString str;
            if(game->gameMapVec[clickPosRow][clickPosCol]==1)
                str="黑棋";
            else if(game->gameMapVec[clickPosRow][clickPosCol]==-1)
                str="白棋";
            QMessageBox::StandardButton btnValue=QMessageBox::information(this,"五子棋决战",str+"胜利！");
            //重置游戏状态，避免死循环
            if(btnValue==QMessageBox::Ok){
                game->startGame(game_type);
                game->gameStatus=PLAYING;
            }

        }
    }


}
void MainWindow::initGame(){
    //初始化游戏模型
    game=new GameModel;
    initAIGame();

}

void MainWindow::initAIGame(){
    game_type=AI;
    game->gameStatus=PLAYING;
    //再数据模型中进行初始化功能
    game->startGame(game_type);
    update();

}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    //通过鼠标的hover确定落子的标记
    int x=event->x();
    int y=event->y();
    //棋盘的边缘不能落子
    if (x>=MARGIN+BLOCK_SIZE/2 && x<size().width()-MARGIN-BLOCK_SIZE/2 && y>=MARGIN+BLOCK_SIZE/2 && y< size().height()-MARGIN-BLOCK_SIZE/2){
        //获取最近的左上角的点
        int col=(x-MARGIN)/BLOCK_SIZE;
        int row=(y-MARGIN)/BLOCK_SIZE;

        int leftTopPosX=MARGIN+BLOCK_SIZE*col;
        int leftTopPosY=MARGIN+BLOCK_SIZE*row;
        //根据距离算出合适的点击距离，一共四个点，根据半径选择最近的
        clickPosRow =-1;//初始化最终的值
        clickPosCol=-1;
        int len = 0;//计算完成后取整

        selectPos=false;

        //确定一个误差在范围内的点，且只可能确定一个出来
        len=sqrt((x-leftTopPosX)*(x-leftTopPosX)+(y-leftTopPosY)*(y-leftTopPosY));
        if (len<POS_OFFSET){
            clickPosRow=row;
            clickPosCol=col;
            if (game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
        len=sqrt((x-leftTopPosX-BLOCK_SIZE)*(x-leftTopPosX-BLOCK_SIZE)+(y-leftTopPosY)*(y-leftTopPosY));
        if (len<POS_OFFSET){
            clickPosRow=row;
            clickPosCol=col+1;
            if (game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
        len=sqrt((x-leftTopPosX)*(x-leftTopPosX)+(y-leftTopPosY-BLOCK_SIZE)*(y-leftTopPosY-BLOCK_SIZE));
        if (len<POS_OFFSET){
            clickPosRow=row+1;
            clickPosCol=col;
            if (game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
        len=sqrt((x-leftTopPosX-BLOCK_SIZE)*(x-leftTopPosX-BLOCK_SIZE)+(y-leftTopPosY-BLOCK_SIZE)*(y-leftTopPosY-BLOCK_SIZE));
        if (len<POS_OFFSET){
            clickPosRow=row+1;
            clickPosCol=col+1;
            if (game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
    }
    //存了坐标之后也要重绘
    update();

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(selectPos==false){
        return;
    }else{
        //在落子前，把落子标记再设置为false
        selectPos=false;
    }
    //由人来下棋
    chessOneByPerson();
    if(game_type==AI){
        //AI下棋
        QTimer::singleShot(AI_THINK_TIME,this,SLOT(chessOneByAI()));
    }
}
void MainWindow::chessOneByPerson(){
    //根据当前存储的坐标下子
    //只有有效点击才下子，并且该处没有子
    if(clickPosRow!= -1 && clickPosCol !=-1 && game->gameMapVec[clickPosRow][clickPosCol]==0){
        //在游戏的数据模型中落子
        game->actionByPerson(clickPosRow,clickPosCol);
        //播放落子音效，待实现

        //重绘
        update();
    }

}

void MainWindow::chessOneByAI()
{
    game->actionByAI(clickPosRow,clickPosCol);
    //QSound::play(":sound/chessone.wav");
    update();
}

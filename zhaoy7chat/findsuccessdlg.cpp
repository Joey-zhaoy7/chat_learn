#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include <QDir>
#include "applyfriend.h"
FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindSuccessDlg)
{
    ui->setupUi(this);

    //添加对话框标题
    setWindowTitle("添加");

    //隐藏掉对话边框栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    // 计算过程（二进制）：
    // Qt::Window:              0000 0000 0000 0001
    // Qt::FramelessWindowHint: 0000 0000 1000 0000
    // 按位或 (|):              0000 0000 1000 0001
    // 结果：两个标志都被保留！

    // 获取当前应用程序的路径 程序一般保存在static目录下
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path +
                                                QDir::separator() + "static"+QDir::separator()+"head_123.jpg");
    //显示图片
    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_lb->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
    ui->add_friend_btn->SetState("normal","hover","press");
    this->setModal(true);



}

FindSuccessDlg::~FindSuccessDlg()
{
    qDebug() <<"FindSuccessDlg destructed!";
    delete ui;
}

void FindSuccessDlg::setSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    _si = si;
}

void FindSuccessDlg::on_add_friend_btn_clicked()
{
    qDebug() << "run add friend btn clicked";
    //to-do later....添加好友界面弹出
    this->hide();
    //弹出界面
    auto applyFriend = new ApplyFriend(_parent);
    qDebug() << "run add friend btn clicked2";
    applyFriend->SetSearchInfo(_si);
    applyFriend->setModal(true);
    applyFriend->show();
}


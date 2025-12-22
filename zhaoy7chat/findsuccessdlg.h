#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include "userdata.h"

namespace Ui {
class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();

    //设置用户信息
    void setSearchInfo(std::shared_ptr<SearchInfo> si);

private:
    Ui::FindSuccessDlg *ui;
    std::shared_ptr<SearchInfo> _si;

    //缓存parent？
    QWidget* _parent;

private slots:
    //点击按钮
    void on_add_friend_btn_clicked();
};

#endif // FINDSUCCESSDLG_H

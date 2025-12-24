#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include <QDialog>
#include "clickedlabel.h"
#include "userdata.h"
#include "friendlabel.h"

namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();
    //初始化一些label 测试用
    void InitTipLbs();
    //添加标签
    void AddTipLbs(ClickedLabel*, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    //事件过滤器 处理滚动区域隐藏显示
    bool eventFilter(QObject *obj, QEvent *event);
    //设置搜索的信息 存起来
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private:
    Ui::ApplyFriend *ui;

    //添加标签的位置
    QPoint _label_point;
    QPoint _tip_cur_point;



    //已经创建好的标签
    QMap<QString, ClickedLabel*> _add_labels;
    std::vector<QString> _add_label_keys;




    //用来在输入框显示添加新好友的标签
    QMap<QString, FriendLabel*> _friend_labels;
    std::vector<QString> _friend_label_keys;

    void addLabel(QString name);
    std::vector<QString> _tip_data;

    std::shared_ptr<SearchInfo> _si;
    //重置标签 位置
    void resetLabels();
public slots:
    //显示更多label标签
    void ShowMoreLabel();
    //输入label按下回车触发将标签加入展示栏
    void SlotLabelEnter();
    //点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString);
    //通过点击tip实现增加和减少好友便签
    void SlotChangeFriendLabelByTip(QString, ClickLbState);
    //输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString& text);
    //输入框输入完成
    void SlotLabelEditFinished();
    //输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void SlotApplySure();
    //处理取消回调
    void SlotApplyCancel();
};

#endif // APPLYFRIEND_H

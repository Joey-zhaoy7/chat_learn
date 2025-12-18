#ifndef CHATVIEW_H
#define CHATVIEW_H
#include <QScrollArea>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTime>

class ChatView : public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent = Q_NULLPTR);
    void appendChatItem(QWidget* item);
    void prependChatItem(QWidget* item);
    void inssertChatItem(QWidget* before, QWidget* item);
protected:
    bool eventFilter(QObject* o, QEvent* e) override;
    void paintEvent(QPaintEvent* event) override;
private slots:
    void onVScrollBarMoved(int min, int max);
private:
    void initStyleSheet();//初始化样式表
private:
    QVBoxLayout* m_pVl;
    QScrollArea* m_pScrollArea;
    bool isAppended;
};

#endif // CHATVIEW_H

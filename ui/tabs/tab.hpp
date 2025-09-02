#ifndef ABSTRACT_TAB
#define ABSTRACT_TAB


#include <QWidget>



class Tab : virtual public QWidget {
  Q_OBJECT

public:
    explicit Tab(QWidget *parent = nullptr) : QWidget(parent) {}


private: 
    virtual void doSomething() = 0;




}

#endif // !ABSTRACT_TAB

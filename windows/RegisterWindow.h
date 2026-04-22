#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QResizeEvent>
#include "../services/AuthService.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterWindow; }
QT_END_NAMESPACE

class RegisterWindow : public QWidget
{
public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

private:
    void handleRegister();
    void goBack();

    void resizeEvent(QResizeEvent *event) override;
    void applyResponsiveLayout();

    Ui::RegisterWindow *ui;
    AuthService authService;
};

#endif // REGISTERWINDOW_H

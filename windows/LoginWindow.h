#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QResizeEvent>
#include "../services/AuthService.h"
#include "../models/User.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget
{
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private:
    void handleLogin();
    void openRegister();

    void resizeEvent(QResizeEvent *event) override;
    void applyResponsiveLayout();

    Ui::LoginWindow *ui;
    AuthService authService;
};

#endif // LOGINWINDOW_H

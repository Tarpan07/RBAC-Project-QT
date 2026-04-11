#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "../services/AuthService.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void handleLogin();
    void openRegister();

private:
    void resizeEvent(QResizeEvent *event) override;
    void applyResponsiveLayout();

    Ui::LoginWindow *ui;
    AuthService authService;
};

#endif // LOGINWINDOW_H
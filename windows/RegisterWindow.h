#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include "../services/AuthService.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterWindow; }
QT_END_NAMESPACE

class RegisterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow();

private slots:
    void handleRegister();
    void goBack();

private:
    void resizeEvent(QResizeEvent *event) override;
    void applyResponsiveLayout();

    Ui::RegisterWindow *ui;
    AuthService authService;
};

#endif // REGISTERWINDOW_H
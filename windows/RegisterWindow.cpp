#include "RegisterWindow.h"
#include "ui_RegisterWindow.h"
#include "LoginWindow.h"

#include <QGuiApplication>
#include <QResizeEvent>
#include <QScreen>
#include <QBoxLayout>
#include <QSizePolicy>

using namespace std;

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::RegisterWindow)
{
    ui->setupUi(this);

    resize(1180, 820);
    setMinimumSize(1020, 720);
    QRect sg = QGuiApplication::primaryScreen()->availableGeometry();
    move((sg.width() - width()) / 2, (sg.height() - height()) / 2);

    ui->iconFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    int librarianIndex = ui->roleCombo->findText("Librarian", Qt::MatchFixedString);
    if (librarianIndex >= 0) {
        ui->roleCombo->removeItem(librarianIndex);
    }
    ui->librarianChip->setVisible(false);
    ui->engineLabel->setAlignment(Qt::AlignCenter);
    ui->orbitOuterLayout->setAlignment(ui->engineLabel, Qt::AlignHCenter);
    ui->orbitOuterLayout->setStretch(0, 1);
    ui->orbitOuterLayout->setStretch(1, 3);
    ui->orbitOuterLayout->setStretch(2, 1);
    ui->shellLayout->setStretch(0, 5);
    ui->shellLayout->setStretch(1, 3);
    ui->heroLayout->setStretch(3, 1);

    setStyleSheet(R"(
        QWidget#RegisterWindow {
            background:
                qradialgradient(cx: 0.14, cy: 0.12, radius: 0.5,
                                fx: 0.14, fy: 0.12,
                                stop: 0 rgba(224, 163, 35, 90),
                                stop: 1 rgba(224, 163, 35, 0)),
                qradialgradient(cx: 0.86, cy: 0.10, radius: 0.46,
                                fx: 0.86, fy: 0.10,
                                stop: 0 rgba(31, 111, 84, 75),
                                stop: 1 rgba(31, 111, 84, 0)),
                qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1,
                                stop: 0 #fff9ec,
                                stop: 0.50 #f7e7c7,
                                stop: 1 #e6c68e);
            color: #1e1b16;
            font-family: 'Trebuchet MS';
        }
        QFrame#shellFrame {
            background: rgba(255, 249, 236, 120);
            border: 1px solid rgba(30, 27, 22, 28);
            border-radius: 36px;
        }
        QFrame#heroFrame {
            background: transparent;
            border: none;
        }
        QScrollArea#cardScrollArea, QWidget#cardScrollContent {
            background: transparent;
            border: none;
        }
        QLabel#eyebrowLabel {
            color: #ac4f2d;
            font-size: 12px;
            font-weight: 700;
            letter-spacing: 2px;
            text-transform: uppercase;
        }
        QLabel#heroTitleLabel {
            color: #1e1b16;
            font-family: Georgia;
            font-size: 34px;
            font-weight: 700;
        }
        QLabel#heroCopyLabel {
            color: #716657;
            font-size: 15px;
            background: transparent;
        }
        QFrame#orbitFrame {
            background: rgba(255, 249, 236, 190);
            border: 1px solid rgba(30, 27, 22, 36);
            border-radius: 42px;
        }
        QLabel#engineLabel {
            min-width: 180px;
            min-height: 180px;
            max-width: 180px;
            max-height: 180px;
            border-radius: 90px;
            background: #0f3f31;
            color: #fffaf0;
            font-size: 18px;
            font-weight: 800;
            padding: 16px;
        }
        QLabel#studentChip, QLabel#facultyChip, QLabel#staffChip, QLabel#librarianChip {
            border: 1px solid rgba(30, 27, 22, 28);
            border-radius: 18px;
            background: #fff9ec;
            color: #1e1b16;
            font-size: 13px;
            font-weight: 700;
            min-height: 36px;
            min-width: 118px;
            padding: 6px 14px;
        }
        QFrame#cardFrame {
            background: rgba(255, 251, 244, 225);
            border: 1px solid rgba(30, 27, 22, 28);
            border-radius: 32px;
        }
        QFrame#iconFrame {
            background: rgba(255, 255, 255, 220);
            border: 1px solid rgba(30, 27, 22, 24);
            border-radius: 22px;
        }
        QLabel#iconLabel {
            color: #0f3f31;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#titleLabel {
            color: #1e1b16;
            font-family: Georgia;
            font-size: 24px;
            font-weight: 700;
        }
        QLabel#subtitleLabel {
            color: #716657;
            font-size: 13px;
            margin-bottom: 8px;
        }
        QLabel {
            font-size: 13px;
            font-weight: 700;
            color: #716657;
            margin-top: 4px;
            background: transparent;
            border: none;
        }
        QLineEdit, QComboBox {
            background: rgba(255, 249, 236, 0.86);
            border: 1px solid rgba(30, 27, 22, 28);
            border-radius: 18px;
            padding: 12px 16px;
            padding-right: 16px;
            color: #1e1b16;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 1px solid #1f6f54;
        }
        QComboBox::drop-down {
            width: 18px;
            border: none;
            background: transparent;
        }
        QComboBox::down-arrow {
            width: 10px;
            height: 10px;
        }
        QComboBox QAbstractItemView {
            background: #fff9ec;
            border: 1px solid rgba(30, 27, 22, 28);
            color: #1e1b16;
            selection-background-color: rgba(31, 111, 84, 0.16);
        }
        QPushButton#registerButton {
            background: #0f3f31;
            color: #fffaf0;
            border-radius: 24px;
            padding: 10px 14px;
            font-weight: 700;
            font-size: 15px;
        }
        QPushButton#registerButton:hover {
            background: #1f6f54;
        }
        QPushButton#registerButton:pressed {
            background: #0b2d22;
        }
        QPushButton#backButton {
            background: rgba(255, 249, 236, 0.72);
            color: #0f3f31;
            border: 1px solid rgba(30, 27, 22, 28);
            border-radius: 24px;
            padding: 10px 12px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton#backButton:hover {
            background: rgba(255, 243, 207, 0.88);
        }
        QLabel#statusLabel {
            color: #ac4f2d;
            font-size: 12px;
        }
    )");

    connect(ui->registerButton, &QPushButton::clicked,
            this, &RegisterWindow::handleRegister);

    connect(ui->backButton, &QPushButton::clicked,
            this, &RegisterWindow::goBack);

    connect(ui->roleCombo, &QComboBox::currentTextChanged,
            this, [=](QString role) {
                if (role.toUpper() == "STUDENT") {
                    ui->studentIdEdit->setEnabled(true);
                    ui->studentIdEdit->setPlaceholderText("Enter Student ID");
                } else {
                    ui->studentIdEdit->setEnabled(false);
                    ui->studentIdEdit->clear();
                    ui->studentIdEdit->setPlaceholderText("Not required");
                }
            });

    ui->studentIdEdit->setEnabled(true);
    applyResponsiveLayout();
}

RegisterWindow::~RegisterWindow()
{
    delete ui;
}

void RegisterWindow::handleRegister()
{
    string name = ui->nameEdit->text().toStdString();
    string email = ui->emailEdit->text().toStdString();
    string role = ui->roleCombo->currentText().toStdString();
    string studentId = ui->studentIdEdit->text().toStdString();
    string password = ui->passwordEdit->text().toStdString();
    string confirm = ui->confirmPasswordEdit->text().toStdString();

    if (name.empty() || email.empty() || password.empty()) {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText("Please fill all required fields.");
        return;
    }

    if (password != confirm) {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText("Passwords do not match.");
        return;
    }

    string result = authService.registerUser(name, email, password, role, studentId);

    if (result == "SUCCESS") {
        ui->statusLabel->setStyleSheet("color: #1f6f54;");
        ui->statusLabel->setText("Account created successfully!");
        goBack();
    } else {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText(QString::fromStdString(result));
    }
}

void RegisterWindow::goBack()
{
    LoginWindow *login = new LoginWindow();
    login->show();
    this->close();
}

void RegisterWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    applyResponsiveLayout();
}

void RegisterWindow::applyResponsiveLayout()
{
    const bool stacked = width() < 1240;

    ui->shellLayout->setDirection(stacked ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    ui->roleIdLayout->setDirection(stacked ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    ui->authWrapFrame->setMaximumWidth(stacked ? QWIDGETSIZE_MAX : 540);
    ui->authWrapFrame->setMinimumWidth(stacked ? 0 : 460);
    ui->shellLayout->setStretch(0, stacked ? 0 : 5);
    ui->shellLayout->setStretch(1, stacked ? 0 : 3);
    ui->heroFrame->setMinimumWidth(stacked ? 0 : 460);
    ui->heroTitleLabel->setStyleSheet(stacked ? "font-size: 28px;" : "");
}

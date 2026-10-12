#pragma once

// Welcome screen shown on startup when no project was given on the command
// line. Lists recent projects and offers New / Open actions. Pure UI: all
// project logic lives in CWindow / MenuActionController.

#include <QDialog>

class CWindow;

class WelcomeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WelcomeDialog(CWindow* window, QWidget* parent = nullptr);

private:
    void openRecent(const QString& path);

    CWindow* _window;
};

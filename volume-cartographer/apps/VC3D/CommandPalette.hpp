#pragma once

#include <QDialog>
#include <QList>
#include <QPointer>
#include <QString>

class QAction;
class QLabel;
class QLineEdit;
class QListWidget;
class QMainWindow;
class QToolButton;

class CommandPalette : public QDialog
{
    Q_OBJECT

public:
    explicit CommandPalette(QWidget* parent = nullptr);
    static QToolButton* createLauncher(QMainWindow* window, QWidget* parent);
    void setActions(const QList<QAction*>& actions);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    struct Command {
        QPointer<QAction> action;
        QString title;
    };

    void updateResults();
    void activateCurrent();

    QList<Command> _commands;
    QLineEdit* _search;
    QListWidget* _results;
    QLabel* _empty;
};

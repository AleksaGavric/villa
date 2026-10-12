#pragma once

// A simple disclosure section: a header button that expands/collapses its
// content. Used to group the segmentation workflow into Grow / Refine /
// Advanced without relying on QToolBox (whose styled tabs misrender).

#include <QWidget>

class QPropertyAnimation;
class QToolButton;
class QVBoxLayout;

class CollapsibleSection : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsibleSection(const QString& title,
                                QWidget* parent = nullptr,
                                bool expanded = true);

    void setExpanded(bool expanded);
    [[nodiscard]] bool isExpanded() const { return _expanded; }

    QVBoxLayout* contentLayout() { return _contentLayout; }

private:
    void updateToggleText();

    QToolButton* _toggle{nullptr};
    QWidget* _content{nullptr};
    QVBoxLayout* _contentLayout{nullptr};
    QPropertyAnimation* _anim{nullptr};
    QString _title;
    bool _expanded;
};

// Simple disclosure section for workflow grouping.

#include "CollapsibleSection.hpp"

#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

CollapsibleSection::CollapsibleSection(const QString& title,
                                       QWidget* parent,
                                       bool expanded)
    : QWidget(parent)
    , _title(title)
    , _expanded(expanded)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    _toggle = new QToolButton(this);
    _toggle->setProperty("sectionHeader", true);
    _toggle->setCheckable(true);
    _toggle->setChecked(expanded);
    _toggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _toggle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    updateToggleText();
    connect(_toggle, &QToolButton::toggled, this, &CollapsibleSection::setExpanded);
    layout->addWidget(_toggle);

    _content = new QWidget(this);
    _contentLayout = new QVBoxLayout(_content);
    _contentLayout->setContentsMargins(0, 4, 0, 0);
    _contentLayout->setSpacing(12);
    _content->setVisible(expanded);
    if (!expanded) {
        _content->setMaximumHeight(0);
    }
    layout->addWidget(_content);

    _anim = new QPropertyAnimation(_content, "maximumHeight", this);
    _anim->setDuration(280);
    _anim->setEasingCurve(QEasingCurve::InOutCubic);
}

void CollapsibleSection::setExpanded(bool expanded)
{
    if (_expanded == expanded && _anim->state() != QPropertyAnimation::Running) {
        return;
    }
    _expanded = expanded;
    if (_toggle->isChecked() != expanded) {
        _toggle->setChecked(expanded);
    }
    updateToggleText();

    // Smooth Apple-like disclosure animation.
    _anim->stop();
    const int startH = _content->height();
    int endH;
    if (expanded) {
        _content->setVisible(true);
        _content->setMaximumHeight(QWIDGETSIZE_MAX);
        endH = _content->sizeHint().height();
        _content->setMaximumHeight(startH);
    } else {
        endH = 0;
    }
    _anim->setStartValue(startH);
    _anim->setEndValue(endH);
    if (!expanded) {
        connect(_anim, &QPropertyAnimation::finished, this, [this]() {
            if (!_expanded) {
                _content->setVisible(false);
            }
        }, Qt::SingleShotConnection);
    } else {
        disconnect(_anim, &QPropertyAnimation::finished, this, nullptr);
    }
    _anim->start();
}

void CollapsibleSection::updateToggleText()
{
    // Chevron indicates state; no icon files needed.
    _toggle->setText((_expanded ? QStringLiteral("▼ ") : QStringLiteral("▶ ")) + _title);
    _toggle->setArrowType(_expanded ? Qt::DownArrow : Qt::RightArrow);
}

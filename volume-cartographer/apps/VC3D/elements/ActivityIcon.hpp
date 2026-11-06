#pragma once

#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace vc3d {
enum class ActivityIcon { Project, Segment, View, Display, Ink, Unwrap };

inline QIcon activityIcon(ActivityIcon icon)
{
    QIcon result;
    for (const auto mode : {QIcon::Normal, QIcon::Active, QIcon::Disabled}) {
        for (const auto state : {QIcon::Off, QIcon::On}) {
            for (const int size : {24, 48}) {
                QPixmap pixmap(size, size);
                pixmap.fill(Qt::transparent);
                QPainter painter(&pixmap);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.scale(size / 24.0, size / 24.0);
                const QColor color(mode == QIcon::Disabled ? "#484f58" :
                                   state == QIcon::On ? "#58a6ff" : "#c9d1d9");
                painter.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                QPainterPath path;
                switch (icon) {
                case ActivityIcon::Project:
                    path.moveTo(3, 7);
                    path.lineTo(3, 4);
                    path.lineTo(10, 4);
                    path.lineTo(12, 7);
                    path.lineTo(21, 7);
                    path.lineTo(21, 20);
                    path.lineTo(3, 20);
                    path.closeSubpath();
                    path.moveTo(3, 10);
                    path.lineTo(21, 10);
                    break;
                case ActivityIcon::Segment:
                    path.moveTo(12, 3);
                    path.lineTo(22, 8);
                    path.lineTo(12, 13);
                    path.lineTo(2, 8);
                    path.closeSubpath();
                    path.moveTo(2, 12);
                    path.lineTo(12, 17);
                    path.lineTo(22, 12);
                    path.moveTo(2, 16);
                    path.lineTo(12, 21);
                    path.lineTo(22, 16);
                    break;
                case ActivityIcon::View:
                    path.moveTo(2, 12);
                    path.cubicTo(7, 3, 17, 3, 22, 12);
                    path.cubicTo(17, 21, 7, 21, 2, 12);
                    path.addEllipse(QPointF(12, 12), 3, 3);
                    break;
                case ActivityIcon::Display:
                    for (int x : {5, 12, 19}) {
                        path.moveTo(x, 3);
                        path.lineTo(x, 21);
                    }
                    path.addRoundedRect(QRectF(2, 7, 6, 4), 1, 1);
                    path.addRoundedRect(QRectF(9, 14, 6, 4), 1, 1);
                    path.addRoundedRect(QRectF(16, 5, 6, 4), 1, 1);
                    break;
                case ActivityIcon::Ink:
                    path.moveTo(12, 3);
                    path.cubicTo(10, 7, 5, 11, 5, 15);
                    path.cubicTo(5, 23, 19, 23, 19, 15);
                    path.cubicTo(19, 11, 14, 7, 12, 3);
                    path.moveTo(9, 15);
                    path.cubicTo(9, 17, 10, 18, 12, 18);
                    break;
                case ActivityIcon::Unwrap:
                    path.moveTo(3, 5);
                    path.lineTo(9, 3);
                    path.lineTo(15, 5);
                    path.lineTo(21, 3);
                    path.lineTo(21, 19);
                    path.lineTo(15, 21);
                    path.lineTo(9, 19);
                    path.lineTo(3, 21);
                    path.closeSubpath();
                    path.moveTo(9, 3);
                    path.lineTo(9, 19);
                    path.moveTo(15, 5);
                    path.lineTo(15, 21);
                    break;
                }
                painter.drawPath(path);
                painter.end();
                result.addPixmap(pixmap, mode, state);
            }
        }
    }
    return result;
}
}

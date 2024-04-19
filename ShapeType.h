#ifndef SHAPETYPE_H
#define SHAPETYPE_H

#include <QString>

enum class ShapeType {
    Rectangle,
    Circle,
    Line,
    Brush,
    Polygon,
    Freehand,
    Arrow,
    Star,
    Heart,
    BubbleSpeech,


};

inline QString getShapeTypeStr(ShapeType sh) {
    switch (sh) {
    case ShapeType::Rectangle: return "Rectangle";
    case ShapeType::Circle: return "Circle";
    case ShapeType::Line: return "Line";
    case ShapeType::Brush: return "Brush";
    case ShapeType::Polygon: return "Polygon";
    case ShapeType::Freehand: return "Freehand";
    case ShapeType::Arrow: return "Arrow";
    case ShapeType::Star: return "Star";
    case ShapeType::Heart: return "Heart";
    case ShapeType::BubbleSpeech: return "Bubble Speech";
    default: return "Unknown";
    }
}

#endif



#ifndef SHAPEEDITOR_H
#define SHAPEEDITOR_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QKeyEvent>
#include <QMouseEvent>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

// Base Shape class
class Shape {
protected:
    int x, y;
    int width, height;
    QColor color;
    bool is_selected;

public:
    Shape(int x, int y);
    virtual ~Shape() = default;

    virtual void draw(QPainter& painter) = 0;
    virtual bool contains(const QPoint& point) const = 0;

    // Getters and setters
    void set_color(const QColor& color) { this->color = color; }
    void set_selected(bool selected) { is_selected = selected; }
    bool get_selected() const { return is_selected; }
    QColor get_color() const { return color; }
    int get_width() const { return width; }
    int get_height() const { return height; }
    int get_x() const { return x; }
    int get_y() const { return y; }
};

// Derived shape classes
class Circle : public Shape {
public:
    Circle(int x, int y);
    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) const override;
};

class Rectangle : public Shape {
public:
    Rectangle(int x, int y);
    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) const override;
};

// Canvas widget
class CanvasWidget : public QWidget {
    Q_OBJECT

private:
    std::vector<Shape*> shapes;
    QString current_shape_type;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

public:
    CanvasWidget(QWidget* parent = nullptr);
    void set_current_shape_type(const QString& shape_type);
};

// Main window
class ShapeEditor : public QMainWindow {
    Q_OBJECT

private:
    CanvasWidget* canvas;

    void create_menu();

private slots:
    void set_shape_type(const QString& shape_type);

public:
    ShapeEditor(QWidget* parent = nullptr);
};

#endif // SHAPEEDITOR_H

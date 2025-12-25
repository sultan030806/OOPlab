#ifndef SHAPEEDITOR_H
#define SHAPEEDITOR_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QPolygon>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

// Shape container class
class ShapesContainer {
private:
    std::vector<class Shape*> shapes;

public:
    void add(Shape* shape);
    void remove(Shape* shape);
    std::vector<Shape*> get_all() const;
    void clear_selected();
    size_t size() const;
    ~ShapesContainer();
};

// Base Shape class
class Shape {
protected:
    int x, y;
    int width, height;
    QColor color;
    QColor selection_color;
    int line_width;
    int selection_line_width;
    bool is_selected;

public:
    Shape(int x, int y);
    virtual ~Shape() = default;

    virtual void draw(QPainter& painter) = 0;
    virtual bool contains(const QPoint& point) const = 0;
    bool move(int dx, int dy, int canvas_width, int canvas_height);
    bool resize(int dw, int dh, int canvas_width, int canvas_height);

    // Getters and setters
    void set_color(const QColor& color) { this->color = color; }
    void set_selected(bool selected) { is_selected = selected; }
    bool get_selected() const { return is_selected; }
    QColor get_color() const { return color; }
    int get_width() const { return width; }
    int get_height() const { return height; }
    int get_x() const { return x; }
    int get_y() const { return y; }

    // New setters for width and height
    void set_width(int w) { width = w; }
    void set_height(int h) { height = h; }
    void set_x(int x) { this->x = x; }
    void set_y(int y) { this->y = y; }

    static QString get_shape_type_name(const Shape* shape);
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

class Square : public Shape {
public:
    Square(int x, int y);
    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) const override;
};

class Triangle : public Shape {
public:
    Triangle(int x, int y);
    void draw(QPainter& painter) override;
    bool contains(const QPoint& point) const override;
};

// Canvas widget
class CanvasWidget : public QWidget {
    Q_OBJECT

private:
    ShapesContainer shapes_container;
    QString current_shape_type;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

public:
    CanvasWidget(QWidget* parent = nullptr);
    void set_current_shape_type(const QString& shape_type);
    ShapesContainer& get_shapes_container() { return shapes_container; }
};

// Main window
class ShapeEditor : public QMainWindow {
    Q_OBJECT

private:
    CanvasWidget* canvas;

    void create_menu();
    void create_toolbar();

private slots:
    void set_shape_type(const QString& shape_type);

public:
    ShapeEditor(QWidget* parent = nullptr);
};

#endif // SHAPEEDITOR_H

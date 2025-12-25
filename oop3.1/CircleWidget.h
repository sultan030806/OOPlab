#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <vector>

class Circle {
public:
    static const int RADIUS = 20;

    Circle(int x, int y);
    void draw(QPainter& painter) const;
    bool contains(int x, int y) const;
    int getX() const { return x; }
    int getY() const { return y; }

private:
    int x, y;
};

class CircleWidget : public QWidget {
    Q_OBJECT

public:
    CircleWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    std::vector<Circle> circles;
};

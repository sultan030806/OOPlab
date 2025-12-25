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

bool isSelected() const { return selected; }
    void setSelected(bool sel) { selected = sel; }
    
private:
    int x, y;
    bool selected = false;  // Добавлено
};

class CircleStorage {  // Новый класс
public:
    void addCircle(std::shared_ptr<Circle> circle);
    void clearSelection();
    std::vector<std::shared_ptr<Circle>>& getCircles() { return circles; }
    
private:
    std::vector<std::shared_ptr<Circle>> circles;
};

class CircleWidget : public QWidget {

class CircleWidget : public QWidget {
    Q_OBJECT

public:
    CircleWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    CircleStorage storage;  // Заменили vector на CircleStorage
};

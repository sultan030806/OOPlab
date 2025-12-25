#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMainWindow>
#include <QLabel>
#include <QStatusBar>
#include <QApplication>
#include <vector>
#include <memory>

// Класс круга
class Circle {
public:
    static const int RADIUS = 20; // Постоянный радиус для всех кругов

    Circle(int x, int y);

    void draw(QPainter& painter) const;
    bool contains(int x, int y) const;
    bool isSelected() const { return selected; }
    void setSelected(bool sel) { selected = sel; }
    int getX() const { return x; }
    int getY() const { return y; }

private:
    int x, y;
    bool selected;
};

// Кастомный контейнер для хранения кругов
class CircleStorage {
public:
    void addCircle(std::shared_ptr<Circle> circle);
    void clearSelection();
    void removeSelected();
    std::vector<std::shared_ptr<Circle>>& getCircles() { return circles; }
    const std::vector<std::shared_ptr<Circle>>& getCircles() const { return circles; }

private:
    std::vector<std::shared_ptr<Circle>> circles;
};

// Виджет-холст для отрисовки кругов
class CircleWidget : public QWidget {
    Q_OBJECT

public:
    CircleWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    CircleStorage storage;

    // Для вывода сообщений в GUI вместо консоли
    void showMessage(const QString& message);
};

// Главное окно приложения
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
};

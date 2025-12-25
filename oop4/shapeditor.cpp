#include "shapeditor.h"

// Shape implementation
Shape::Shape(int x, int y) : x(x), y(y), width(50), height(50),
color(0, 0, 255), is_selected(false) {}

// Circle implementation
Circle::Circle(int x, int y) : Shape(x, y) {}

void Circle::draw(QPainter& painter) {
    QPen pen(color, 2);
    painter.setPen(pen);
    painter.drawEllipse(get_x(), get_y(), get_width(), get_height());

    if (is_selected) {
        QPen selection_pen(QColor(255, 0, 0), 3);
        painter.setPen(selection_pen);
        painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
        painter.drawEllipse(get_x() - 3, get_y() - 3, get_width() + 6, get_height() + 6);
    }
}

bool Circle::contains(const QPoint& point) const {
    int center_x = get_x() + get_width() / 2;
    int center_y = get_y() + get_height() / 2;
    int radius = get_width() / 2;
    int dx = point.x() - center_x;
    int dy = point.y() - center_y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// Rectangle implementation
Rectangle::Rectangle(int x, int y) : Shape(x, y) {
    width = 80;
    height = 40;
}

void Rectangle::draw(QPainter& painter) {
    QPen pen(color, 2);
    painter.setPen(pen);
    painter.drawRect(get_x(), get_y(), get_width(), get_height());

    if (is_selected) {
        QPen selection_pen(QColor(255, 0, 0), 3);
        painter.setPen(selection_pen);
        painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
        painter.drawRect(get_x() - 3, get_y() - 3, get_width() + 6, get_height() + 6);
    }
}

bool Rectangle::contains(const QPoint& point) const {
    return (point.x() >= get_x() && point.x() <= get_x() + get_width() &&
        point.y() >= get_y() && point.y() <= get_y() + get_height());
}

// CanvasWidget implementation
CanvasWidget::CanvasWidget(QWidget* parent) : QWidget(parent) {
    current_shape_type = "";
}

void CanvasWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (Shape* shape : shapes) {
        shape->draw(painter);
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x();
        int y = event->pos().y();

        // Create new shape if type is selected
        if (!current_shape_type.isEmpty()) {
            Shape* new_shape = nullptr;

            if (current_shape_type == "rectangle") {
                new_shape = new Rectangle(x, y);
            }
            else if (current_shape_type == "circle") {
                new_shape = new Circle(x, y);
            }

            if (new_shape) {
                shapes.push_back(new_shape);
                std::cout << "Created shape at (" << x << ", " << y << ")" << std::endl;
            }
        }

        update();
    }
}

void CanvasWidget::set_current_shape_type(const QString& shape_type) {
    current_shape_type = shape_type;
}

// ShapeEditor implementation
ShapeEditor::ShapeEditor(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Vector Graphics Editor");
    resize(800, 600);

    canvas = new CanvasWidget(this);
    setCentralWidget(canvas);

    create_menu();
}

void ShapeEditor::create_menu() {
    QMenuBar* menu_bar = menuBar();

    // Shapes menu
    QMenu* shapes_menu = menu_bar->addMenu("Shapes");

    QAction* rectangle_action = new QAction("Rectangle", this);
    QAction* circle_action = new QAction("Circle", this);

    shapes_menu->addAction(rectangle_action);
    shapes_menu->addAction(circle_action);

    connect(rectangle_action, &QAction::triggered, this, [this]() { set_shape_type("rectangle"); });
    connect(circle_action, &QAction::triggered, this, [this]() { set_shape_type("circle"); });
}

void ShapeEditor::set_shape_type(const QString& shape_type) {
    canvas->set_current_shape_type(shape_type);
    std::cout << "Selected shape: " << shape_type.toStdString() << std::endl;
}
